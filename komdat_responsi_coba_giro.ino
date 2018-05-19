#include <Wire.h>

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;
// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain

#define PIN_CS 6
#define PIN_CLK 7
#define PIN_DIN 5

void maxTransfer(uint8_t address, uint8_t value) {

    digitalWrite(PIN_CS, LOW);                             // Ensure LOAD/CS is LOW
    shiftOut(PIN_DIN,PIN_CLK,MSBFIRST,address);            // Send the register address
    shiftOut(PIN_DIN,PIN_CLK,MSBFIRST,value);              // Send the value     
    digitalWrite(PIN_CS, HIGH);
    delay(10);
}


void test()
    {
      maxTransfer(0x0F, 0x01); //test
      delay(1000);
      maxTransfer(0x0F, 0x00);

      maxTransfer(0x09, 0xFF); //mode B

      maxTransfer(0x0A, 0x0D); //intensitas
      
      maxTransfer(0x0B, 0x00);  // cacah digit
      
      maxTransfer(0x0C, 0x01);  // Turn on chip
    }

void tampil(int i)
{
  String s = String(i);
  int l = s.length();

  maxTransfer(0x2C,0);
  maxTransfer(0x2B,l-1);
  maxTransfer(0x2C,1);
  
  for( int j = 1; j<=l; j++){
    if(s[l-j] == "-")
    {
       maxTransfer(j,10);
    }else{
       maxTransfer(j,int(s[l-j]));
    }
  }
}

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup(){
  pinMode(PIN_CS,OUTPUT);
  pinMode(PIN_CLK,OUTPUT);
  pinMode(PIN_DIN,OUTPUT);
  test();
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(57600);// Debugging only
    if (!driver.init())
         Serial.println("init failed");
}
void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  //
  
  //send radio
   const char *msg = strdup((String(GyX)+"__").c_str());

    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(500);
    
  tampil(GyX);
}
