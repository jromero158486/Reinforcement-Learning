// ===============================
//        PINES MOTORES
// ===============================
#define ENA 11
#define ENB 12

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// ===============================
//      PINES ULTRASONICO
// ===============================
#define TRIG 9
#define ECHO 8

// ===============================
//         VELOCIDAD
// ===============================
int baseSpeed = 190;
int balance   = 10;

// ===============================
//       FUNCIONES MOTORES
// ===============================
void forward() {
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed - balance);
}

void backward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed - balance);
}

void leftTurn(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void rightTurn(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

void stopCar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ===============================
//       SENSOR ULTRASONICO
// ===============================
float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 25000);
  float dist = duration * 0.034 / 2;

  if (dist == 0) dist = 100;
  return dist;
}

// ===============================
//              SETUP
// ===============================
void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  randomSeed(analogRead(A0)); // para giros aleatorios

  stopCar();
}

// ===============================
//               LOOP
// ===============================
void loop() {

  float dist = getDistance();

  // -------- EXPLORACIÓN REAL --------
  if (dist < 12) {

    // 1. Retroceder fuerte
    backward();
    delay(350);

    // 2. Girar aleatoriamente
    if (random(0, 2) == 0) {
      leftTurn();
    } else {
      rightTurn();
    }
    delay(400);

    // 3. Avanzar sin medir (escape)
    forward();
    delay(500);

    stopCar();
    delay(80);

  } else {
    // Camino libre → avanzar normal
    forward();
  }

  delay(40);
}
