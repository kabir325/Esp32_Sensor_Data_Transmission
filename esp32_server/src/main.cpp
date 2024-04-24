#include <Arduino.h>    // to work with esp32 board using arduino framework

#include <WiFi.h>       // to setup esp32 as an access point

//for MAX30102 pulse sensor
#include <Wire.h>       // to use I2C ports 21 and 22
#include "MAX30105.h"   // library for sensor
#include "heartRate.h"  

// for DHT11 sensor
#include "DHTesp.h"     

// access point credentials and secret key
const char *ssid = "Sensor_server";
const char *password = "random_string_formation";
const char *secretKey = "1j8fh3r08fs03bf320n"; 

WiFiServer server(80);
WiFiClient client;

MAX30105 particleSensor;
DHTesp dht;

const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 

float beatsPerMinute;
int beatAvg;

void pin_init(){
  pinMode(25,OUTPUT); 
  pinMode(26,OUTPUT);
  pinMode(33,OUTPUT);
}

void MAX30102_setup(){
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Max30102 setup sucessful\n");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A); 
  particleSensor.setPulseAmplitudeGreen(0); 

  for(int i=0;i<10;i++){
    digitalWrite(33,HIGH);
    delay(100);
    digitalWrite(33,LOW);
    delay(100);
  }
}

void DHT11_setup(){
  dht.setup(19, DHTesp::DHT11);
}

void wifi_begin(){
  WiFi.softAP(ssid, password); 

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();

  digitalWrite(33,HIGH);
}

void setup(){
  Serial.begin(115200);

  Serial.println("Setup started");

  pin_init();

  MAX30102_setup();

  DHT11_setup();

  wifi_begin();

  Serial.println("Setup finished");
}

float temp_value(){
  Serial.println("Reading Temperature sensor data");
  float temp,sum=0;
  int count=500;
  while(count>0){
    temp=dht.getTemperature();
    sum=sum+temp;
    count--;
    delay(10);
  }
  sum=sum/100;
  return sum;
}

int pulse_value(){
  Serial.println("Reading pulse sensor data");
  long irValue;
  unsigned long startTime = millis();
  beatsPerMinute=0;
  beatAvg=0;

  irValue = particleSensor.getIR();

  while (irValue < 10000) {
    irValue = particleSensor.getIR();
    delay(10); 
  }

  startTime = millis();
  while ((millis() - startTime < 10000)&&(irValue>10000)) { 
    irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute; 
        rateSpot %= RATE_SIZE; 

        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
    delay(5); 
  }

  delay(1000);
  return beatAvg;
}

void share_data(){
  String Reading;
  Serial.println("Sharing Sensor data");
  float temp_Reading;
  int pulse_Reading;

  while (client.connected()){
    Reading="";
    temp_Reading=temp_value();
    pulse_Reading=pulse_value();

    Reading="Temp: "+String(temp_Reading)+"C, "+"Pulse: "+String(pulse_Reading)+"BPM";
    client.println(Reading);
    Serial.println(Reading);
    digitalWrite(26,HIGH);
    delay(100);
    digitalWrite(26,LOW);
    delay(1000);
  }
  digitalWrite(25,LOW);
}

void Authenticate_client(){
  String receivedKey = client.readStringUntil('\n');
  receivedKey.trim(); 
  
  if (receivedKey.equals(secretKey)) {
    client.println("Authentication successful. Welcome!");
    Serial.println("Client authenticated");
    digitalWrite(25,HIGH);
    share_data();
  }
  else {
    client.println("Authentication failed. Unauthorized access!");
    client.stop();
    Serial.println("Client disconnected");                                  
    digitalWrite(26,HIGH);
    delay(1000);
    digitalWrite(26,LOW);
  }
}

void connect_client(){
  while (!client) {
    Serial.print(".");
    client = server.available();
    delay(1000);
  }
  Serial.println();
  Serial.println("New client connected");
  digitalWrite(25, HIGH);
  delay(1000);
  digitalWrite(25,LOW);

  Authenticate_client();
}

void loop()
{
  if(!client.connected()){
    connect_client();
  }
}
/*
PINS:
  19: DHT11 sensor data pin
  21: MAX30102 sensor SDA pin
  22: MAX30102 sensor SCL pin
  25: Indicator LED blue
  26: Indicator LED white
  33: Indicator LED red
*/

