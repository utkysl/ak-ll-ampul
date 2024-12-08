#define BLYNK_TEMPLATE_ID "......"
#define BLYNK_TEMPLATE_NAME "......"
#define BLYNK_AUTH_TOKEN "......."
#define BLYNK_PRINT Serial  

char ssid[] = "....";    
char pass[] = "....";       
 
#include <ESP8266WiFi.h>     
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const String apiKey = ".......";
const String city = "......";
const String server = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

Adafruit_BMP085 bmp;
#define I2C_SCL D1
#define I2C_SDA D2
float dst,bt,bp,ba;
char dstmp[20],btmp[20],bprs[20],balt[20];
bool bmp085_present=true;
#define LED_PIN D0
BlynkTimer timer;

BLYNK_WRITE(V1) {
  int pinValue = param.asInt(); 
  digitalWrite(LED_PIN, pinValue); 
}


void sendSensor()
{

	if (!bmp.begin()) 
	{
           Serial.println("Could not find a valid BMP085 sensor, check wiring!");
              while (1) {}
        }

float bp =  bmp.readPressure()/100;
float ba =  bmp.readAltitude();
float bt =  bmp.readTemperature();
float dst = bmp.readSealevelPressure()/100;

Blynk.virtualWrite(V3, bp);
Blynk.virtualWrite(V11, ba);
Blynk.virtualWrite(V2, bt);
Blynk.virtualWrite(V13, dst);
}

void setup()
{
  Serial.begin(115200);
  delay(10);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(10);
	timer.setInterval(1000L, sendSensor);

}

void loop()
 {
  Blynk.run();
  timer.run();
  HTTPClient http; 
  WiFiClient client; 

  http.begin(client, server); 

  int httpCode = http.GET(); 

  if (httpCode > 0) { 
    String payload = http.getString(); 

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    float temperature = doc["main"]["temp"]; 
    float windSpeed = doc["wind"]["speed"];
    float windSpeedKmH = windSpeed * 3.6; 

    Blynk.virtualWrite(V5, temperature);
    Blynk.virtualWrite(V4, windSpeedKmH);


  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
 }
