#include <Wire.h>

#include <ACS712.h>

#include <ZMPT101B.h>

#include <LiquidCrystal_I2C.h>

#include <ZMPT101B.h>

#include <ACS712.h>

#include <Wire.h>

#include <LiquidCrystal_I2C.h>

 String apiKey = "VRA4F458WYC3D7MM";



// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.


float kWh=0;
float Units=0;
float Rupees=0
// ZMPT101B sensor connected to A0 pin of arduino
ZMPT101B voltageSensor(A0);

// 5 amps version sensor (ACS712_05B) connected to A1 pin of arduino
ACS712 currentSensor(ACS712_30A, A1);

void setup()
{
  Serial.begin(9600);

  // calibrate() method calibrates zero point of sensor,
  // It is not necessary, but may positively affect the accuracy
  // Ensure that no current flows through the sensor at this moment
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  delay(100);
  voltageSensor.calibrate();
  currentSensor.calibrate();
  Serial.println("Done!");
  lcd.init();
  lcd.backlight();
  Serial1.begin(115200); //microcontroller and ESP01
connectWiFi();
}

void loop()
{
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
   delay(100);
  voltageSensor.calibrate();
  currentSensor.calibrate();
  Serial.println("Done!");
  // To measure voltage/current we need to know the frequency of voltage/current
  // By default 50Hz is used, but you can specify desired frequency
  // as first argument to getVoltageAC and getCurrentAC() method, if necessary

  float U = voltageSensor.getVoltageAC();
  float I = currentSensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
  float P =U * I;
  float kWh=P/1000*(20/3600);
  float Units=Units+kWh;
  float Rupes=Units*10;
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
 delay(1000);
 lcd.clear(); // clear the screen
 lcd.setCursor(1,0); // set the cursor at 1st col and 1st row
 lcd.print(kWh);
 lcd.print("units ");
 Serial1.println("AT+CIPMUX=0\r\n");
delay(1000);

// TCP connection 
String cmd = "AT+CIPSTART=\"TCP\",\""; 
cmd += "184.106.153.149"; 
// api.thingspeak.com 
cmd += "\",80\r\n\r\n"; 

Serial1.println(cmd);
Serial.println(cmd);//JF01

delay(5000);

if(Serial1.find("ERROR"))
{ 
Serial.println("AT+CIPSTART error"); 
//return; 
}

// prepare GET string 

String getStr = "GET /update?api_key=";
 getStr += apiKey;
 getStr +="&field1=";
 getStr += I; 
getStr += "\r\n\r\n"; 

getStr +="&field2=";
 getStr += U; 
getStr += "\r\n\r\n"; 

getStr +="&field3=";
 getStr += P; 
getStr += "\r\n\r\n";

 getStr +="&field4=";
 getStr += Units; 
getStr += "\r\n\r\n"; 

getStr +="&field5=";
 getStr += I; 
getStr += "\r\n\r\n"; 

// send data length 

Serial.println(getStr);

cmd = "AT+CIPSEND="; 
cmd += String(getStr.length());
cmd+="\r\n";

Serial1.println(cmd); 
Serial.println(cmd);

delay(5000);

if(Serial1.find(">"))
{
Serial.println("connected to Cloud"); 
Serial1.print(getStr);
 }
 else
{ 
Serial1.println("AT+CIPCLOSE\r\n"); 
// alert user 

Serial.println("AT+CIPCLOSE");
 } 
// thingspeak needs 15 sec delay between updates 

delay(16000); 

 }

boolean connectWiFi()
{

Serial1.println("AT+CWMODE=1\r\n");// to configure esp01 to work in station mode

String cmd = "AT+CWJAP=\"";
cmd += "BLACK PEARL";
cmd += "\",\"";
cmd += "CAPTAINJS1";
cmd += "\"\r\n";
  
  Serial.println(cmd);//just for information
  Serial1.println(cmd);
  
  delay(10000);
  
   Serial1.print("AT+CWJAP?");//verify connectivity 
  
  if(Serial1.find("+CWJAP"))
  {
    Serial.println("OK, Connected to WiFi.");
    return true;
  }
  else{
    Serial.println("Can not connect to the WiFi.");
    return false;
  }




  delay(1000);
}
