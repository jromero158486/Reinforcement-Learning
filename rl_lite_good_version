#include <Arduino.h>
#include <Servo.h>

// Motores
#define ENA 11
#define ENB 12
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// Ultrasonido
#define TRIG 9
#define ECHO 8

// Servo (solo para apuntar al frente)
#define SERVO_PIN 6
Servo headServo;

// Velocidad
int baseSpeed = 190;
int balance   = 10;

// RL
const int NUM_STATES  = 4;   // 0..3 según distancia
const int NUM_ACTIONS = 5;   // FWD, LEFT, RIGHT, BACK, STOP

const int ACT_FWD   = 0;
const int ACT_LEFT  = 1;
const int ACT_RIGHT = 2;
const int ACT_BACK  = 3;
const int ACT_STOP  = 4;

float alpha     = 0.35;
float gamma_val = 0.85;
float epsilon   = 0.6;   // empezamos explorando bastante

float Q[NUM_STATES][NUM_ACTIONS];
int lastAction = -1;

// ---------------- MOTORES ----------------
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

// --------------- ULTRASONIDO --------------
float getRawDistance() {
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

float getDistanceSmooth() {
  float d = 0;
  for (int i = 0; i < 3; i++) {
    d += getRawDistance();
  }
  return d / 3.0;
}

// ------------- DISTANCIA → ESTADO ----------
int getState(float d) {
  if (d < 8)   return 0;  // muy cerca
  if (d < 15)  return 1;  // cerca
  if (d < 30)  return 2;  // medio
  return 3;               // libre
}

// ------------- RECOMPENSA ------------------
float computeReward(float dist, float newDist, int state, int action) {
  float r = 0.0;

  if (dist < 8) r -= 8.0;  // muy cerca → castigo fuerte

  if (newDist > dist + 3) r += 4.0;   // se alejó del obstáculo
  if (newDist < dist - 3) r -= 4.0;   // se acercó

  if (action == ACT_FWD && dist > 30) r += 3.0; // avanzar libre

  if (action == ACT_STOP) r -= 1.0;   // no queremos parar mucho

  if (action == ACT_BACK && dist < 12) r += 2.0; // retroceder cuando está cerca puede ser bueno

  // castigo suave por repetir siempre lo mismo
  if (action == lastAction) r -= 0.3;

  return r;
}

// -------- SELECCIÓN DE ACCIÓN --------------
int selectAction(int state) {
  float rnd = random(0,1000) / 1000.0;
  if (rnd < epsilon) {
    return random(0, NUM_ACTIONS);
  }

  float bestVal = -9999.0;
  int bestA = 0;
  for (int a = 0; a < NUM_ACTIONS; a++) {
    if (Q[state][a] > bestVal) {
      bestVal = Q[state][a];
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

// ----------------- SETUP -------------------
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
  headServo.write(90); // siempre al frente
  delay(300);

  for (int s = 0; s < NUM_STATES; s++) {
    for (int a = 0; a < NUM_ACTIONS; a++) {
      Q[s][a] = 0.0;
    }
  }

  randomSeed(analogRead(A0));
  stopCar();
}

// ----------------- LOOP --------------------
void loop() {
  headServo.write(90);   // mirar al frente
  delay(50);

  float dist = getDistanceSmooth();
  int state = getState(dist);

  int action = selectAction(state);
  executeAction(action);
  delay(180);

  headServo.write(90);
  delay(50);
  float newDist = getDistanceSmooth();
  int nextState = getState(newDist);

  float reward = computeReward(dist, newDist, state, action);

  float maxNext = -9999.0;
  for (int a = 0; a < NUM_ACTIONS; a++) {
    if (Q[nextState][a] > maxNext) {
      maxNext = Q[nextState][a];
    }
  }

  Q[state][action] += alpha * (reward + gamma_val * maxNext - Q[state][action]);

  lastAction = action;

  // epsilon decay suave
  epsilon = max(0.1f, epsilon * 0.9995f);

  Serial.print("dist:");
  Serial.print(dist);
  Serial.print(" newDist:");
  Serial.print(newDist);
  Serial.print(" S:");
  Serial.print(state);
  Serial.print(" A:");
  Serial.print(action);
  Serial.print(" R:");
  Serial.print(reward);
  Serial.print(" eps:");
  Serial.println(epsilon);

  delay(40);
}
