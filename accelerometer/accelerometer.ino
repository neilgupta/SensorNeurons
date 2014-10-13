#include <XBee.h>
#include <Wire.h>

const int MPU=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int threshold = 2000;
long lastAcX, lastAcY, lastAcZ, lastGyX, lastGyY, lastGyZ;

// Create an XBee object at the top of your sketch
XBee xbee = XBee();

// Specify the address of the remote XBee (this is the SH + SL)
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40682fb6);

void setup(){
  Serial.begin(9600);
  xbee.setSerial(Serial);
  
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  lastAcX = 0;
  lastAcY = 0;
  lastAcZ = 0;
  lastGyX = 0;
  lastGyY = 0;
  lastGyZ = 0;
}

void sendTx(long data) {
  // Create an array for holding the data you want to send.
  uint8_t payload[] = { 'A' };
  
  // Create a TX Request
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

  // Send your request
  xbee.send(zbTx);
}

void loop(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  //Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  // Filter data that has not significantly changed
  long diffAcX = lastAcX - AcX;
  diffAcX = abs(diffAcX);
  lastAcX = AcX;
  long diffAcY = lastAcY - AcY;
  diffAcY = abs(diffAcY);
  lastAcY = AcY;
  long diffAcZ = lastAcZ - AcZ;
  diffAcZ = abs(diffAcZ);
  lastAcZ = AcZ;
  long diffGyX = lastGyX - GyX;
  diffGyX = abs(diffGyX);
  lastGyX = GyX;
  long diffGyY = lastGyY - GyY;
  diffGyY = abs(diffGyY);
  lastGyY = GyY;
  long diffGyZ = lastGyZ - GyZ;
  diffGyZ = abs(diffGyZ);
  lastGyZ = GyZ;
  
  if (diffAcX > threshold) {
    Serial.print("AcX = "); Serial.print(AcX);
    sendTx(AcX);
  } else if (diffAcY > threshold) {
    Serial.print(" | AcY = "); Serial.print(AcY);
    sendTx(AcY);
  } else if (diffAcZ > threshold) {
    Serial.print(" | AcZ = "); Serial.print(AcZ);
    sendTx(AcZ);
  } else if (diffGyX > threshold) {
    Serial.print(" | GyX = "); Serial.print(GyX);
    sendTx(GyX);
  } else if (diffGyY > threshold) {
    Serial.print(" | GyY = "); Serial.print(GyY);
    sendTx(GyY);
  } else if (diffGyZ > threshold) {
    Serial.print(" | GyZ = "); Serial.println(GyZ);
    sendTx(GyZ);
  }
  
  // Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  
  delay(333);
}
