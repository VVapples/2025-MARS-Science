#include <Arduino.h>

// --- PIN DEFINITIONS ---
const int EN_PIN = 23;
const int FR_PIN = 22;
const int SV_PIN = 21;

// --- PWM SETTINGS ---
const int pwmFreq = 5000;
const int pwmChannel = 0;
const int pwmResolution = 8;

void setup() {
  Serial.begin(115200);
  Serial.println("BLD-510B Motor Test (Inverted Logic)");

  pinMode(EN_PIN, OUTPUT);
  pinMode(FR_PIN, OUTPUT);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(SV_PIN, pwmChannel);

  // START WITH MOTOR DISABLED
  // If HIGH is Run, then LOW is Stop.
  digitalWrite(EN_PIN, LOW); 
  ledcWrite(pwmChannel, 0);   
  
  delay(1000);
}

void loop() {
  // --- STEP 1: SPIN FORWARD ---
  Serial.println("Direction: Forward");
  digitalWrite(FR_PIN, LOW); 
  
  // Set Speed
  ledcWrite(pwmChannel, 200); 
  
  // ENABLE MOTOR (Changed to HIGH)
  digitalWrite(EN_PIN, HIGH);
  
  delay(5000);

  // --- STEP 2: STOP ---
  Serial.println("Stopping...");
  
  // DISABLE MOTOR (Changed to LOW)
  digitalWrite(EN_PIN, LOW); 
  ledcWrite(pwmChannel, 0);   
  
  delay(2000); 

  // --- STEP 3: SPIN BACKWARD ---
  Serial.println("Direction: Reverse");
  digitalWrite(FR_PIN, HIGH);
  ledcWrite(pwmChannel, 200);
  
  // ENABLE MOTOR (Changed to HIGH)
  digitalWrite(EN_PIN, HIGH);
  
  delay(5000);

  // --- STEP 4: STOP AGAIN ---
  Serial.println("Stopping...");
  digitalWrite(EN_PIN, LOW);
  ledcWrite(pwmChannel, 0);
  delay(2000);
}