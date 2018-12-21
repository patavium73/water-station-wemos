/* Stazione Meteo WiFi Wemos Mcu
 *  BME temperature and humidity data are uploaded to thingspeak using
 * WeMOS ESP8266 MCU.
 * 
 * Details: 
 * https://www.arduinotronics.com
 */
 
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Math.h"

/*
 * Sensor connection:
 * BME280 GND --> NodeMCU GND
 * BME280 3.3V --> NodeMCU 3V3
 * BME280 SDA --> NodeMCU D2
 * BME280 SCL --> NodeMCU D1
 */
// Time to sleep (in seconds):5 min.
const int sleepTimeS = 300;

// Sensor settings
//const uint8_t sensor_address = 0x77;
 const uint8_t sensor_address = 0x76;


#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

float prevTemp = 0;

const char* server = "api.thingspeak.com";
String apiKey =".................";
const char* MY_SSID = ".............."; 
const char* MY_PWD = "...............";
int sent = 0;
int PinBatt = A0;  
int ValPinBatt;
float Vpin_perc;
float Vbatt;  
float vb = 0;

//    ******************** S E T U P ***********************
void setup() {
  
  Serial.begin(115200);
  
  
 Serial.println(F("BME280 test"));
delay(100);
pinMode (PinBatt, INPUT);

  delay(100);
    bool status;
    
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
     delay(500); // let sensor boot up
    Serial.println("-- Default Test --");
    

    Serial.println();
    delay(800); 
  connectWifi();
  
}
//   ***************************** L O O P ********************
void loop() {

  // read data from sensor
    float Taria = bme.readTemperature();
    //temperature = 9.0/5.0 * temperature + 32.0;
    float hpa = (bme.readPressure()/100);
    float humidity = bme.readHumidity();
   float vb= analogRead(PinBatt);

   
ValPinBatt = analogRead(PinBatt);
  Vpin_perc = map (ValPinBatt, 0, 1023, 0,500);
  
   
 Vbatt= ((ValPinBatt/5.93)/10) ;
  
  Serial.print("Temperatura: ");
  Serial.println(Taria);
  Serial.print("Umidita: ");
  Serial.println(humidity);
  Serial.print("Pressione: ");
  Serial.println(hpa);
  Serial.print("Voltaggio: ");
  Serial.println(Vbatt);
  
  sendTeperatureTS(Taria, humidity,hpa, Vbatt);
  
  ESP.deepSleep(sleepTimeS*1000000, WAKE_RF_DEFAULT); // Sleep for 60 seconds
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void sendTeperatureTS(float Taria,float humidity,float hpa,float Vbatt)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(Taria);
   postStr += "&field2=";
   postStr += String(humidity);
    postStr += "&field3=";
   postStr += String(hpa);
   postStr += "&field4=";
   postStr += String(Vbatt);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
