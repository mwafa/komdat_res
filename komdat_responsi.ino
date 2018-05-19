#define PIN_CS 5
#define PIN_CLK 6
#define PIN_DIN 7

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
       maxTransfer(j,0b1010);
    }else{
       maxTransfer(j,int(s[l-j]));
    }
  }
}


#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;


void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_CS,OUTPUT);
  pinMode(PIN_CLK,OUTPUT);
  pinMode(PIN_DIN,OUTPUT);
  test();
  Serial.begin(9600);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);

  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  
  tampil(lux);
  delay(200);

}
