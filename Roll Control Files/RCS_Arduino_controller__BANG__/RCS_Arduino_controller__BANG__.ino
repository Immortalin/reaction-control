#include <Wire.h>

#define A_GAIN 0.932    // [deg/LSB]
#define G_GAIN 1.466    // [deg/s/LSB]

#define DT 0.002         // [s/loop] loop period
#define A 0.98         // complementary filter constant

#define KP 1.8          // proportional controller gain [LSB/deg/loop]
#define KD 0.4          // derivative controller gain [LSB/deg/loop]
#define KI 0.5          // Integral controller gain

float accelX = 0.0;
float angleX = 0.0;      // [deg]
float rateX = 0.0;       // [deg/s]
float outputX = 0.0;     // [LSB] (100% voltage to motor is 255LSB)

float accelZ = 0.0;
float angleZ = 0.0;      // [deg]
float rateZ = 0.0;       // [deg/s]
float outputZ = 0.0;     // [LSB] (100% voltage to motor is 255LSB)

float accelY = 0.0;
float angleY = 0.0;      // [deg]
float rateY = 0.0;       // [deg/s]
float outputY = 0.0;     // [LSB] (100% voltage to motor is 255LSB)

// LED pin assignments
int rollCW = 11;
int rollCCW = 10;
int pitch = 5;
int yaw = 6;

const int MPU=0x68;  // I2C address of the MPU-6050
float AcX;
float AcY;
float AcZ;
float GyX;
float GyY;
float GyZ;
float Tmp;
float d = 0;


int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 100;           // interval at which to blink (milliseconds)

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(rollCW, OUTPUT);
  pinMode(rollCCW, OUTPUT);
  pinMode(pitch, OUTPUT);
  pinMode(yaw, OUTPUT);
  Serial.begin(9600);
}


void IMU(){ //IMU function for MPU-6050 
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read(); 
  AcZ=Wire.read()<<8|Wire.read(); 
  Tmp=Wire.read()<<8|Wire.read(); 
  GyX=Wire.read()<<8|Wire.read(); 
  GyY=Wire.read()<<8|Wire.read(); 
  GyZ=Wire.read()<<8|Wire.read();
  
  // adjusting roll rates with negatives
  if (GyX >= 32768) {
  GyX = GyX-65536;
  }
  if (GyY >= 32768) {
  GyY = GyY-65536;
  }
  if (GyZ >= 32768) {
  GyZ = GyZ-65536;
  }
  ////////////////////////////////////////////////////////////
  //output for Pitch tilt
  rateX = GyX / 131;  
  Serial.print(" | rateX = ");
  Serial.print(rateX);
  //set accell rate
  accelY = AcY / 16384;
  Serial.print(" | accY = ");
  Serial.print(accelY);
  //determine the angle
  angleZ = A * (angleZ + rateZ * DT) + (1 - A) * accelY;  
  Serial.print(" | angZ = ");
  Serial.print(angleZ);
  //set the scalar reaction 
  outputZ = (angleZ * KP + rateZ * KD) ;
  Serial.print(" | outZ = ");
  Serial.print(outputZ);
  ////////////////////////////////////////////////////////////
  //output for Yaw tilt
  rateY = GyY / 131;  
  Serial.print(" | rateY = ");
  Serial.print(rateY);
  //set accell rate
  accelZ = AcZ / 16384;
  Serial.print(" | accZ = ");
  Serial.print(accelZ);
  //determine the angle
  angleY = A * (angleY + rateY * DT) + (1 - A) * accelZ;  
  Serial.print(" | angY = ");
  Serial.print(angleY);
  //set the scalar reaction 
  outputY = (angleY * KP + rateY * KD) ;
  Serial.print(" | outY = ");
  Serial.print(outputY);
  ////////////////////////////////////////////////////////////
  //output for roll rate
  rateZ = GyZ / 131;  
  Serial.print(" | rateZ = ");
  Serial.println(rateZ);
   
}

void blink() //function for blink without  delay using preset interval for PWM
{ 
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   
    if (ledState == LOW){
      ledState = HIGH;
  
    }
    else{
      ledState = LOW;
  }
  }
}

void roll() { //function to control roll
    //acvitate solenoids to roll clockwise
  if (rateX >= 2) { 
    blink();
    digitalWrite(rollCW,ledState);
  }
  //acvitate solenoids to roll counter-clockwise  
  else if (rateX <= -2) {
    blink();
    digitalWrite(rollCCW,ledState);
  }
 //sweetspot!! no roll action to solenoids
  else {
    digitalWrite(rollCW,LOW);
    digitalWrite(rollCCW,LOW);    
  }
}

void loop() {
  
  IMU(); //recall IMU function
  
  roll(); //roll rate controller 
 

  delay(1);

}
