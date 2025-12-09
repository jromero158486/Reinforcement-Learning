#include <Arduino.h>

// ====== PINES PWM (SEGÚN LO QUE ME DIJISTE) ======
#define ENA 11   // Velocidad Motor A
#define ENB 12   // Velocidad Motor B

// ====== PINES DE DIRECCIÓN ======
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// ====== PARÁMETROS DE VELOCIDAD ======
int baseSpeed = 220;   // velocidad general (0–255)
int balance   = 25;   // cuánto se corrige el lado débil/fuerte

// Si tu carro gira a la IZQUIERDA → balance positivo
// Si gira a la DERECHA → balance negativo

// ===============================
//       FUNCIONES DE MOVIMIENTO
// ===============================

// AVANZAR RECTO (CON PWM)
void forward(){
  // Direcciones (según corrección previa)
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, HIGH);

  // Velocidades con corrección
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed - balance);
}

// RETROCEDER RECTO
void backward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed - balance);
}

// GIRAR IZQUIERDA (SIN PWM, GIRO FUERTE)
void leftTurn(){
  digitalWrite(IN1, LOW);  
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

// GIRAR DERECHA
void rightTurn(){
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH); 
  digitalWrite(IN4, LOW);

  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
}

// DETENER
void stopCar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ===============================
//              SETUP
// ===============================
void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopCar();
}

// ===============================
//               LOOP
// ===============================
void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if      (c == 'U') forward();
    else if (c == 'D') backward();
    else if (c == 'L') leftTurn();
    else if (c == 'R') rightTurn();
    else if (c == 'S') stopCar();
  }
}
