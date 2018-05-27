#include <SFE_BMP180.h>
#include <Wire.h>

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

#define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters

#define PIN_CS 6
#define PIN_CLK 7
#define PIN_DIN 5

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


void kirim(double P)
{
  char *msg = strdup(("P="+String(P)+"__").c_str());
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("REBOOT");
  
  pinMode(PIN_CS,OUTPUT);
  pinMode(PIN_CLK,OUTPUT);
  pinMode(PIN_DIN,OUTPUT);
  test();
  driver.init();
  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
}

void loop()
{
  char status;
  double T,P,p0,a;

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
  
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");

          

        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
  
  tampil(String(P)); // tampilkan ke 7segment

  kirim(P); //kirim via radio
  
  delay(1000);  // Pause for 5 seconds.
}
