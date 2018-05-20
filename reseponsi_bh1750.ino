#include <Wire.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define ADDR 0x23

#define PIN_CS 6
#define PIN_CLK 7
#define PIN_DIN 5


RH_ASK driver;

void hidupkan(){
  Wire.beginTransmission(ADDR);
  Wire.write(0x01);
  Wire.endTransmission();

}


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

      maxTransfer(0x09, 0xFF); //mode B

      maxTransfer(0x0A, 0x0D); //intensitas
      
      maxTransfer(0x0B, 0x07);  // cacah digit
      
      maxTransfer(0x0C, 0x01);  // Turn on chip
    }

void tampil(String a)
{
  int koma = a.indexOf(".");
  
  
  if(koma>-1){
    a.remove(koma,1);
  }
  int panjang = a.length();
  maxTransfer(0x0B, panjang-1);  // cacah digit
  for( int j = 1; j<=panjang; j++){
      int index = panjang-j;
      if(String(a[index]) == "-"){
        maxTransfer(j,10);
      }else{
         int nilai = index == koma-1 ? 0xF0|String(a[index]).toInt():String(a[index]).toInt();
         maxTransfer(j,nilai);
      }
  }
}

void kirim(String nama,int16_t data){
  
 //send radio

   const char *msg = strdup((nama+":"+String(data)+"__").c_str());
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(500);
    
}
    

void setup() {
  pinMode(PIN_CS,OUTPUT);
  pinMode(PIN_CLK,OUTPUT);
  pinMode(PIN_DIN,OUTPUT);
  test();
  driver.init();
  Wire.begin();

  Serial.begin(57600);
  
}

void loop() {
  hidupkan();
  Wire.beginTransmission(ADDR);
  Wire.write(0x13);
  Wire.endTransmission(false);
  delay(16);
  Wire.requestFrom(ADDR,2,true);
  uint16_t lux = Wire.read()<<8|Wire.read();
  Serial.println(lux);
  
  tampil(String(lux));
  
  kirim("Lux",lux);
  delay(500);
  
}
