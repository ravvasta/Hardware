#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu_neck;
Adafruit_MPU6050 mpu_back;

// Feedback pins
const int vibePin = 5;
const int leftLedPin = 6;
const int rightLedPin = 7;

// Posture thresholds (adjust based on your needs)
const float NECK_FORWARD_THRESHOLD = -20.0; // degrees
const float NECK_LEFT_THRESHOLD = -5.0;
const float NECK_RIGHT_THRESHOLD = 5.0;
const float BACK_HUNCH_THRESHOLD = -15.0;

// Variables for sensor data
float neckPitch, neckRoll, backPitch;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  Serial.println("Serial communication established");
  
  // Initialize feedback outputs
  pinMode(vibePin, OUTPUT);
  pinMode(leftLedPin, OUTPUT);
  pinMode(rightLedPin, OUTPUT);
  Serial.println("Output pins initialized");
  
  // Initialize I2C
  Wire.begin();
  Serial.println("I2C initialized");
  
  // In setup():
  Serial.println("Attempting to initialize neck sensor (0x68)...");
  if (!mpu_neck.begin(0x68)) {  // AD0 LOW/unconnected NECK
    Serial.println("Failed to find neck MPU6050 chip");
    while (1) {
      Serial.println("Check neck sensor wiring (AD0 to GND)");
      delay(1000);
    }
  }

  Serial.println("Attempting to initialize back sensor (0x69)...");
  if (!mpu_back.begin(0x69)) {  // AD0 HIGH BACK
    Serial.println("Failed to find back MPU6050 chip");
    while (1) {
      Serial.println("Check back sensor wiring (AD0 to 3.3V)");
      delay(1000);
    }
  }
  Serial.println("Back sensor initialized successfully");
  
  // [Rest of your setup code...]
}
  
  

void loop() {
  // Read sensor data
  sensors_event_t a_neck, g_neck, temp_neck;
  sensors_event_t a_back, g_back, temp_back;
  
  mpu_neck.getEvent(&a_neck, &g_neck, &temp_neck);
  mpu_back.getEvent(&a_back, &g_back, &temp_back);
  
  // Calculate orientation angles (simplified)
  neckPitch = atan2(-a_neck.acceleration.x, sqrt(a_neck.acceleration.y*a_neck.acceleration.y + a_neck.acceleration.z*a_neck.acceleration.z)) * 180/PI;
  neckRoll = atan2(a_neck.acceleration.y, a_neck.acceleration.z) * 180/PI;
  backPitch = atan2(-a_back.acceleration.x, sqrt(a_back.acceleration.y*a_back.acceleration.y + a_back.acceleration.z*a_back.acceleration.z)) * 180/PI;
  
  // Check posture and provide feedback
  checkPosture();
  
  // Print data for debugging
  Serial.print("Neck Pitch: "); Serial.print(neckPitch);
  Serial.print("°\tNeck Roll: "); Serial.print(neckRoll);
  Serial.print("°\tBack Pitch: "); Serial.print(backPitch);
  Serial.println("°");
  
  delay(100); // Adjust as needed
}

void calibrateSensors() {
  Serial.println("Calibrating sensors... Stay still for 5 seconds");
  
  // Blink LEDs and vibrate to indicate calibration start
  digitalWrite(leftLedPin, HIGH);
  digitalWrite(rightLedPin, HIGH);
  digitalWrite(vibePin, HIGH);
  delay(500);
  digitalWrite(leftLedPin, LOW);
  digitalWrite(rightLedPin, LOW);
  digitalWrite(vibePin, LOW);
  
  // Simple calibration - just wait for stable position
  delay(5000);
  
  // Blink to indicate calibration complete
  for(int i=0; i<3; i++) {
    digitalWrite(leftLedPin, HIGH);
    digitalWrite(rightLedPin, HIGH);
    delay(200);
    digitalWrite(leftLedPin, LOW);
    digitalWrite(rightLedPin, LOW);
    delay(200);
  }
  
  Serial.println("Calibration complete!");
}

void checkPosture() {
  bool badPosture = false;
  
  // Reset feedback
  digitalWrite(leftLedPin, LOW);
  digitalWrite(rightLedPin, LOW);
  digitalWrite(vibePin, LOW);
  
  // Check neck forward tilt (looking down)
  if(neckPitch < NECK_FORWARD_THRESHOLD) {
    Serial.println("Bad posture: Neck too far forward");
    digitalWrite(leftLedPin, HIGH);
    digitalWrite(rightLedPin, HIGH);
    digitalWrite(vibePin, HIGH);
    badPosture = true;
  }
  
  // Check neck left tilt
  if(neckRoll < NECK_LEFT_THRESHOLD) {
    Serial.println("Bad posture: Leaning left");
    digitalWrite(leftLedPin, HIGH);
    digitalWrite(vibePin, HIGH);
    badPosture = true;
  }
  
  // Check neck right tilt
  if(neckRoll > NECK_RIGHT_THRESHOLD) {
    Serial.println("Bad posture: Leaning right");
    digitalWrite(rightLedPin, HIGH);
    digitalWrite(vibePin, HIGH);
    badPosture = true;
  }
  
  // Check back hunch
  if(backPitch < BACK_HUNCH_THRESHOLD) {
    Serial.println("Bad posture: Back hunched");
    digitalWrite(leftLedPin, HIGH);
    digitalWrite(rightLedPin, HIGH);
    digitalWrite(vibePin, HIGH);
    badPosture = true;
  }
  
  // If no bad posture detected, give positive feedback occasionally
  static unsigned long lastGoodFeedback = 0;
  if(!badPosture && millis() - lastGoodFeedback > 10000) {
    // Quick green flash (if using RGB LEDs) or blink
    digitalWrite(leftLedPin, HIGH);
    digitalWrite(rightLedPin, HIGH);
    delay(100);
    digitalWrite(leftLedPin, LOW);
    digitalWrite(rightLedPin, LOW);
    lastGoodFeedback = millis();
  }
}
