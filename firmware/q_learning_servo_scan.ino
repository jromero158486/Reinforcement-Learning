// =======================================================
//        Q-LEARNING CON SERVO ESCÁNER (L–C–R)
//        Estados = lado más libre (izq, cent, der, bloqueado)
// =======================================================

#include <Arduino.h>
#include <Servo.h>

// ---------------- PINES MOTORES ----------------
#define ENA 11
#define ENB 12

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// ---------------- PINES ULTRASONICO -------------
#define TRIG 9
#define ECHO 8

// ---------------- PIN SERVO ---------------------
#define SERVO_PIN 6
Servo headServo;

// ---------------- VELOCIDAD ---------------------
int baseSpeed = 190;
int balance   = 10;

// ---------------- RL: ESTADOS Y ACCIONES --------
const int NUM_STATES  = 4;   
const int NUM_ACTIONS = 5;   

const int ACT_FWD   = 0;
const int ACT_LEFT  = 1;
const int ACT_RIGHT = 2;
const int ACT_BACK  = 3;
const int ACT_STOP  = 4;

// ---------------- PARÁMETROS Q-LEARNING ----------
float alpha = 0.35;       
float gamma_val = 0.85;   
float epsilon = 0.55;     

float Q[NUM_STATES][NUM_ACTIONS];

// =======================================================
//                    MOTORES
// =======================================================
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed - balance);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed - balance);
}

void leftTurn() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void rightTurn() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// =======================================================
//                    ULTRASONICO
// =======================================================
float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 25000);
  float dist = duration * 0.034 / 2;

  if (dist <= 0) dist = 120;
  return dist;
}

// =======================================================
//             SERVO: ESCANEO L – C – R
// =======================================================
float senseAt(int angle) {
  headServo.write(angle);
  delay(200); // estabilizar servo
  return getDistance();
}

void scanDistances(float &L, float &C, float &R) {
  L = senseAt(150);   // izquierda
  C = senseAt(90);    // centro
  R = senseAt(30);    // derecha
}

// =======================================================
//             ESTADOS: lado más libre
// =======================================================
//  S0 = izquierda es mejor
//  S1 = centro es mejor
//  S2 = derecha es mejor
//  S3 = todo bloqueado (< 12 cm)
// =======================================================
int getState(float L, float C, float R) {

  float maxD = max(L, max(C, R));

  if (maxD < 12) return 3; // bloqueado

  if (maxD == L) return 0;
  if (maxD == C) return 1;
  if (maxD == R) return 2;

  return 1;
}

// =======================================================
//              FUNCIÓN DE RECOMPENSA
// =======================================================
float computeReward(float L, float C, float R, int state, int action) {
  float r = 0.0;

  // SI ESTÁ TODO BLOQUEADO
  if (state == 3) {
    if (action == ACT_BACK) r += 4;
    else r -= 3;
  }

  // Recompensa por tomar el camino más libre
  if (state == 0 && action == ACT_LEFT)  r += 5;
  if (state == 1 && action == ACT_FWD)   r += 5;
  if (state == 2 && action == ACT_RIGHT) r += 5;

  // Castigo por tomar el camino equivocado
  if (state == 0 && action == ACT_RIGHT) r -= 3;
  if (state == 2 && action == ACT_LEFT)  r -= 3;

  return r;
}

// =======================================================
//           SELECCIÓN DE ACCIÓN (ε-GREEDY)
// =======================================================
int selectAction(int state) {
  if ((random(0, 1000) / 1000.0) < epsilon)
    return random(0, NUM_ACTIONS);

  float best = -9999;
  int bestA = 0;

  for (int a = 0; a < NUM_ACTIONS; a++) {
    if (Q[state][a] > best) {
      best = Q[state][a];
      bestA = a;
    }
  }
  return bestA;
}

void executeAction(int a) {
  if (a == ACT_FWD)   forward();
  if (a == ACT_LEFT)  leftTurn();
  if (a == ACT_RIGHT) rightTurn();
  if (a == ACT_BACK)  backward();
  if (a == ACT_STOP)  stopCar();
}

// =======================================================
//                        SETUP
// =======================================================
void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  headServo.attach(SERVO_PIN);
  headServo.write(90);
  delay(300);

  randomSeed(analogRead(A0));

  stopCar();
}

// =======================================================
//                        LOOP
// =======================================================
void loop() {

  // -------- 1) Escaneo completo --------
  float L, C, R;
  scanDistances(L, C, R);

  // -------- 2) Estado según lado más libre --------
  int state = getState(L, C, R);

  // -------- 3) Selección de acción --------
  int action = selectAction(state);

  // -------- 4) Ejecutar --------
  executeAction(action);
  delay(180);

  // -------- 5) Nuevo escaneo --------
  float L2, C2, R2;
  scanDistances(L2, C2, R2);

  // -------- 6) Recompensa --------
  float reward = computeReward(L, C, R, state, action);

  // -------- 7) Q-Learning update --------
  float maxNext = -9999;
  int nextState = getState(L2, C2, R2);

  for (int a = 0; a < NUM_ACTIONS; a++) {
    if (Q[nextState][a] > maxNext)
      maxNext = Q[nextState][a];
  }

  Q[state][action] += alpha * (reward + gamma_val * maxNext - Q[state][action]);

  // -------- 8) Debug --------
  Serial.print("L:"); Serial.print(L);
  Serial.print(" C:"); Serial.print(C);
  Serial.print(" R:"); Serial.print(R);
  Serial.print(" | S:"); Serial.print(state);
  Serial.print(" A:"); Serial.print(action);
  Serial.print(" Rw:"); Serial.println(reward);

  delay(40);
}
