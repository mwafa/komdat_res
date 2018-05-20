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

int tanda[5] = {
  0b01111011,
  0b01111101,
  0b01000000,
  0b00000001,
  0b00001000
};

void maxTransfer(uint8_t address, uint8_t value) {

    digitalWrite(PIN_CS, LOW);                             // Ensure LOAD/CS is LOW
    shiftOut(PIN_DIN,PIN_CLK,MSBFIRST,address);            // Send the register address
    shiftOut(PIN_DIN,PIN_CLK,MSBFIRST,value);              // Send the value     
    digitalWrite(PIN_CS, HIGH);
}


void test()
    {
      maxTransfer(0x0F, 0x01); //test
      delay(1000);
      maxTransfer(0x0F, 0x00);

      maxTransfer(0x09, 0b00111111); //mode B

      maxTransfer(0x0A, 0x0D); //intensitas
      
      maxTransfer(0x0B, 0x07);  // cacah digit
      
      maxTransfer(0x0C, 0x01);  // Turn on chip
    }

void tampil(String a,int tipe, int aksis)
{
  int koma = a.indexOf(".");
  
  
  if(koma>-1){
    a.remove(koma,1);
  }
  int panjang = a.length();
  maxTransfer(0x0B, 7);  // cacah digit
  for( int j = 1; j<=panjang; j++){
      int index = panjang-j;
      if(String(a[index]) == "-"){
        maxTransfer(j,10);
      }else{
         int nilai = index == koma-1 ? 0xF0|String(a[index]).toInt():String(a[index]).toInt();
         maxTransfer(j,nilai);
      }
  }
  for(int j = panjang+1 ; j <= 8;j++ )
  {
    maxTransfer(j,0x0F); 
  }
  maxTransfer(7,tanda[tipe]);
  maxTransfer(8,tanda[aksis+2]);
  
}

void kirim(String nama,int16_t data){
  
 //send radio

   const char *msg = strdup((nama+":"+String(data)+"__").c_str());
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(500);
    
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
kirim("AcX",AcX);
tampil(String(AcX),1,0);
kirim("AcY",AcY);
tampil(String(AcY),1,1);
kirim("AcZ",AcZ);
tampil(String(AcZ),1,2);
kirim("GyX",GyX);
tampil(String(GyX),0,0);
kirim("GyY",GyY);
tampil(String(GyY),0,1);
kirim("GyZ",GyZ);
tampil(String(GyZ),0,2);
}
