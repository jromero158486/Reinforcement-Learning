// =======================================================
//      Q-LEARNING BÁSICO PARA CARRO CON ULTRASÓNICO
//      Estados = distancia, Acciones = movimiento
// =======================================================

#include <Arduino.h>

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

// ---------------- VELOCIDAD ---------------------
int baseSpeed = 190;
int balance   = 10;   // corrige recta (ajústalo según tu carro)

// ---------------- RL: ESTADOS Y ACCIONES --------
const int NUM_STATES  = 4;   // S0, S1, S2, S3 según distancia
const int NUM_ACTIONS = 5;   // FWD, LEFT, RIGHT, BACK, STOP

// Indices de acciones para que sea más legible
const int ACT_FWD   = 0;
const int ACT_LEFT  = 1;
const int ACT_RIGHT = 2;
const int ACT_BACK  = 3;
const int ACT_STOP  = 4;

// ---------------- PARÁMETROS Q-LEARNING ----------
float alpha = 0.35;       // tasa de aprendizaje
float gamma_val = 0.85;   // factor de descuento
float epsilon = 0.55;     // prob. de explorar (acción aleatoria)

float Q[NUM_STATES][NUM_ACTIONS];  // tabla Q

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

  long duration = pulseIn(ECHO, HIGH, 25000); // timeout ~4m
  float dist = duration * 0.034 / 2;

  if (dist == 0) dist = 120;   // lectura inválida → asumimos lejos
  return dist;
}

// =======================================================
//             MAPEO: DISTANCIA → ESTADO
// =======================================================
//
// S0: muy cerca  (0–8 cm)
// S1: cerca      (8–15 cm)
// S2: medio      (15–30 cm)
// S3: libre      (>30 cm)
//
int getState(float d) {
  if (d < 8)   return 0;   // muy cerca / casi choque
  if (d < 15)  return 1;   // cerca
  if (d < 30)  return 2;   // rango medio
  return 3;                // lejos / libre
}

// =======================================================
//              FUNCIÓN DE RECOMPENSA
// =======================================================
//
// Usa distancia anterior y nueva.
// Ideas:
// - Castigo grande si está muy cerca
// - Premio si se aleja del obstáculo
// - Premio si avanza estando libre
// - Castigo leve si se queda quieto
//
float computeReward(float dist, float newDist, int state, int action) {
  float r = 0.0;

  // Castigo fuerte si está peligrosamente cerca
  if (dist < 8) {
    r -= 8.0;
  }

  // Si la nueva distancia es mayor → se alejó del obstáculo
  if (newDist > dist + 3) {
    r += 4.0;
  }

  // Si se acercó más al obstáculo
  if (newDist < dist - 3) {
    r -= 4.0;
  }

  // Recompensa por avanzar en zona libre
  if (action == ACT_FWD && dist > 30) {
    r += 3.0;
  }

  // Castigo leve por quedarse parado
  if (action == ACT_STOP) {
    r -= 1.0;
  }

  // Retroceder en zona muy cercana puede ser bueno
  if (action == ACT_BACK && dist < 12) {
    r += 2.0;
  }

  return r;
}

// =======================================================
//           SELECCIÓN DE ACCIÓN (POLÍTICA ε-GREEDY)
// =======================================================
int selectAction(int state) {
  // Exploración
  float rnd = random(0, 1000) / 1000.0;  // 0.0–0.999
  if (rnd < epsilon) {
    return random(0, NUM_ACTIONS);       // acción aleatoria
  }

  // Explotación: mejor acción según Q
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

// Ejecutar acción elegida
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

  // Inicializar Q en 0 (Arduino ya lo hace, pero por claridad)
  for (int s = 0; s < NUM_STATES; s++) {
    for (int a = 0; a < NUM_ACTIONS; a++) {
      Q[s][a] = 0.0;
    }
  }

  randomSeed(analogRead(A0)); // para exploración

  stopCar();
}

// =======================================================
//                        LOOP
// =======================================================
//
// 1. Medir distancia y obtener estado S
// 2. Elegir acción A (ε-greedy)
// 3. Ejecutar acción A un ratito
// 4. Medir nueva distancia y estado S'
// 5. Calcular recompensa R
// 6. Actualizar Q[S][A]
// 7. Repetir
//
void loop() {
  // 1) Estado actual
  float dist = getDistance();
  int state = getState(dist);

  // 2) Elegir acción
  int action = selectAction(state);

  // 3) Ejecutar acción un tiempo
  executeAction(action);
  delay(180);   // duración del movimiento (ajustable)

  // 4) Nuevo estado
  float newDist = getDistance();
  int nextState = getState(newDist);

  // 5) Recompensa
  float reward = computeReward(dist, newDist, state, action);

  // 6) Q-Learning update
  float maxNext = -9999.0;
  for (int a = 0; a < NUM_ACTIONS; a++) {
    if (Q[nextState][a] > maxNext) {
      maxNext = Q[nextState][a];
    }
  }

  Q[state][action] += alpha * (reward + gamma_val * maxNext - Q[state][action]);

  // 7) Telemetría simple para ver el aprendizaje en Serial
  Serial.print("dist:");   Serial.print(dist);
  Serial.print("  newDist:"); Serial.print(newDist);
  Serial.print("  S:");    Serial.print(state);
  Serial.print("  S':");   Serial.print(nextState);
  Serial.print("  A:");    Serial.print(action);
  Serial.print("  R:");    Serial.println(reward);

  delay(40);  // pequeño respiro
}
