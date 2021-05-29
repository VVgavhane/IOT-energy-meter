#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#include <ZMPT101B.h>

#include <ACS712.h>
float U;
float I;
float P;
float kWh=0;
float Units=0;
float Rupees=0;


String myAPIkey = "VRA4F458WYC3D7MM";

long writingTimer = 17; 
long startTime = 0;
long waitTime = 0;


boolean relay1_st = false; 
boolean relay2_st = false; 
unsigned char check_connection=0;
unsigned char times_check=0;
boolean error;


//#include "ZMPT101B.h"
//#include "ACS712.h"

/*
  This example shows how to measure the power consumption
  of devices in AC electrical system
*/

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD
// We have ZMPT101B sensor connected to A0 pin of arduino
// Replace with your version if necessary
ZMPT101B voltageSensor(A0);

// We have 30 amps version sensor connected to A1 pin of arduino
// Replace with your version if necessary
ACS712 currentSensor(ACS712_30A, A1);

void setup()
{
  Serial.begin(9600);

  // calibrate() method calibrates zero point of sensor,
  // It is not necessary, but may positively affect the accuracy
  // Ensure that no current flows through the sensor at this moment
  // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  delay(100);
  voltageSensor.calibrate();
  currentSensor.calibrate();
  Serial.println("Done!");
  lcd.init();
  lcd.backlight();
  Serial1.begin(115200); //microcontroller and ESP01
startTime = millis(); 
  Serial1.println("AT+RST");
  delay(2000);
  Serial.println("Connecting to Wifi");
   while(check_connection==0)
  {
    Serial.print(".");
  Serial1.print("AT+CWJAP=\"BLACK PEARL\",\"CAPTAINJS1\"\r\n");
  Serial1.setTimeout(5000);
 if(Serial1.find("WIFI CONNECTED\r\n")==1)
 {
 Serial.println("WIFI CONNECTED");
 break;
 }
 times_check++;
 if(times_check>3) 
 {
  times_check=0;
   Serial.println("Trying to Reconnect..");
  }
  }
}


void loop()
{
power();
    
  waitTime = millis()-startTime;   
  if (waitTime > (writingTimer*1000)) 
  {
    
    writeThingSpeak();
    startTime = millis();  
  }
    
  
}

void power(void)
{
  // To measure voltage/current we need to know the frequency of voltage/current
  // By default 50Hz is used, but you can specify desired frequency
  // as first argument to getVoltageAC and getCurrentAC() method, if necessary

  U = voltageSensor.getVoltageAC();
  I = currentSensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
   P = U * I;
   kWh=P/1000*(20/3600);
   Units=Units+kWh;
   Rupees=Units*10;
  Serial.println(String("U = ") + U + " V");
  Serial.println(String("I = ") + I + " A");
  Serial.println(String("P = ") + P + " Watts");
  Serial.println(String("Units = ") + Units + " ");
  Serial.println(String("Rupees = ") + Rupees + " $");
  lcd.setCursor(1,0); // set the cursor at 1st col and 1st row
 lcd.print(U);
 lcd.print("v ");
 lcd.print(I);
 lcd.print("A");
 lcd.setCursor(1,1); // set the cursor at 1st col and 2nd row
 lcd.print(P);                                                                                
 lcd.print("w");
 delay(2000);
 lcd.clear(); // clear the screen
 lcd.setCursor(1,0); // set the cursor at 1st col and 1st row
 lcd.print(kWh);
 lcd.print("units ");
  delay(2000);
  
}

void writeThingSpeak(void)
{
  startThingSpeakCmd();
  // preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += myAPIkey;
  getStr +="&field1=";
  getStr += I;
  getStr +="&field2=";
  getStr += U;
  getStr +="&field3=";
  getStr += P;
  getStr +="&field4=";
  getStr += Units;
  getStr +="&field5=";
  getStr += Rupees;
  getStr += "\r\n\r\n";
  GetThingspeakcmd(getStr); 
   
}

void startThingSpeakCmd(void)
{
  Serial1.flush();
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com IP address
  cmd += "\",80";
  Serial1.println(cmd);
  Serial.print("Start Commands: ");
  Serial.println(cmd);

  if(Serial1.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

String GetThingspeakcmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Serial1.println(cmd);
  Serial.println(cmd);

  if(Serial1.find(">"))
  {
    Serial1.print(getStr);
    Serial.println(getStr);
    delay(500);
    String messageBody = "";
    while (Serial1.available()) 
    {
      String line = Serial1.readStringUntil('\n');
      if (line.length() == 1) 
      { 
        messageBody = Serial1.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    Serial1.println("AT+CIPCLOSE");     
    Serial.println("AT+CIPCLOSE"); 
  } 
}
  
