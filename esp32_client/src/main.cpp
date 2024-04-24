#include <Arduino.h>
#include <dirent.h>
#include <WiFi.h>
#include <WiFiClient.h>

//WIFI AUTHENTICATION AND CONNECTION 
const char *ssid = "Sensor_server";
const char *password = "random_string_formation";
const char *secretKey = "1j8fh3r08fs03bf320n"; 

const IPAddress serverIP(192, 168, 4, 1);
const int serverPort = 80;

WiFiClient client;

//INITIALIZING THE PINS WE WILL USE
void pin_init(){
  pinMode(5,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
}

void setup(){
  Serial.begin(115200);

  Serial.println("Initializing......");

  pin_init();

  Serial.println("Initialization completed..");
  
  digitalWrite(5,HIGH);
}

void wifi_connect(){
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(7, HIGH);
  Serial.println();
}

void recieve_data(){
  String Reading;
  while(client.connected()){
    Reading=client.readStringUntil('\n');
    if(Reading!=""){
      Serial.println("Sensor values recieved from Server:");
      Serial.println(Reading);
    }
  }
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  Serial.println("helsdjghser");
}

void wifi_authentication() {
  
  if (client.connect(serverIP, serverPort)) {
    
    Serial.println("\nConnected to WiFi");
    client.println(secretKey);
    while (client.connected() && !client.available()) {
      delay(1); 
    }
    Serial.println("check1");
    Serial.println(client.readStringUntil('\n'));
    if(client.connected()){
      digitalWrite(8,HIGH);
      recieve_data();
    }
  }
  else {
    Serial.println("Unable to connect to server");
  }
}


void loop(){
  wifi_connect();

  wifi_authentication();
  
  while(1){
    // a button can be added which will make the program exit from the loop and run again and connect to the server again.
  }
}

/*
  what i can do is when ever i revieve data from the server i can store the data with the time the data was revcieved in an map 
  and in case of being disconnected from the server i can host my client as an access point and can connect devices to it and at a particular ip i can display all the values with the stipulated time

*/