#include <Wire.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

// Alamat dari sensor
#define ADDR 0x23  

// Mendefinisikan pin dari SPI
#define PIN_CS 6
#define PIN_CLK 7
#define PIN_DIN 5


RH_ASK driver;

// Funsi untuk menyalakan sensor (mematikan _sleep mode_)
void hidupkan(){
  Wire.beginTransmission(ADDR);
  Wire.write(0x01);
  Wire.endTransmission();

}

// Funsi untuk kirim SPI dari arduino ke 7Segment
void maxTransfer(uint8_t address, uint8_t value) {

    digitalWrite(PIN_CS, LOW);                             // pin CS harus LOW sebelum krim data
    shiftOut(PIN_DIN,PIN_CLK,MSBFIRST,address);            // Kirim alamat (perintah)
    shiftOut(PIN_DIN,PIN_CLK,MSBFIRST,value);              // Kirim nilai (parameter)  
    digitalWrite(PIN_CS, HIGH);                            // Pengiriman selesai maka CS dibuat HIGH
}



void test()
    {
      maxTransfer(0x0F, 0x01); // Test - Menyalakan semua lampu di 7segment
      delay(1000);
      maxTransfer(0x0F, 0x00); // Mengembalikan dari keadaan test

      maxTransfer(0x09, 0xFF); // Mode B (data didecode)

      maxTransfer(0x0A, 0x0D); // Pengaturan intensitas dari 7segment
      
      maxTransfer(0x0B, 0x07); // Alamat 0x0B untuk mengatur jumlah digit yang menyala 7 -> 8 digit menyala. 
      
      maxTransfer(0x0C, 0x01);  // Menghidupkan MAX7219 (agar tidak dalam sleep mode).
    }

void tampil(String a) // Funsi untuk menampilkan ke 7segment
{
  int koma = a.indexOf("."); // Menentukan letak tanda koma (.)
  
  if(koma>-1){
    a.remove(koma,1); // Tanda koma dihapus karena koma tidak menjadi 1 digit.
  }
  int panjang = a.length();
  maxTransfer(0x0B, panjang-1);  // Banyak digit yang dinyalakan sesuai dengan panjang angka.
  for( int j = 1; j<=panjang; j++){
      int index = panjang-j;
      if(String(a[index]) == "-"){ // Jika terdapat tanda (-) maka akan ditampilkan (-).
        maxTransfer(j,10);         // Dalam mode B tanda (-) sama dengan 10.
      }else{
        // Penentuan apakah ada koma (.) atau tidak pada setiap digit.
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
  // Deklarasi pin sebagai OUTPUT untuk mengirim data dengan mode SPI
  pinMode(PIN_CS,OUTPUT);
  pinMode(PIN_CLK,OUTPUT);
  pinMode(PIN_DIN,OUTPUT);

  test();  // Test 7segment

  driver.init();
  Wire.begin();

  Serial.begin(57600);
  
}

void loop() {
  hidupkan();
  Wire.beginTransmission(ADDR);
  Wire.write(0x13);  // Perintah untuk membaca sensor pada low resolution.
  Wire.endTransmission(false);
  delay(16);         // Waktu delay antara perintah dan respon (sesuai datasheet).
  Wire.requestFrom(ADDR,2,true);
  uint16_t lux = Wire.read()<<8|Wire.read();
  Serial.println(lux);
  
  tampil(String(lux)); //Menampilkan pada 7segment
  
  kirim("Lux",lux);    //Pengiriman data melalui radio
  delay(500);
  
}
