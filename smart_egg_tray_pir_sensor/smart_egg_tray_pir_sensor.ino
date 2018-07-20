//-----------------------------Including required libraries-------------------------------------------------//
#include "WiFi.h"
#include<ESP32Ticker.h>
Ticker secondTick;
#include <PubSubClient.h>

//-----------------------------Defining required pins-------------------------------------------------------//
#define slot1 34
#define slot2 35
#define slot3 32 
#define slot4 33
#define slot5 25
#define slot6 26


//--------------------------------WiFi and MQTT credentials-----------------------------------------------//

//const char* ssid = "real08";
//const char* password = "rashedmizan";
const char* mqtt_server = "182.48.84.180";
const int mqttPort = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

 
// Initializing variables to read pir sensors

int data1=0;
int data2=0;
int data3=0;
int data4=0;
int data5=0;
int data6=0;


//------------------------WATCH DOG SUBROUTINE-------------------------------//

//int watchdogCount=0;
volatile int watchdogCount=0;
void ISRwatchdog(){


  watchdogCount++;
  if(watchdogCount==100){


    //Serial.println();
    Serial.print("The watch dog bites......");
    ESP.restart();
  }
}

//--------------------------SET UP FUNCTION---------------------------------//


void setup()
{ 

  Serial.begin(115200);
  pinMode(slot1,INPUT);
  pinMode(slot2,INPUT);
  pinMode(slot3,INPUT);
  pinMode(slot4,INPUT);
  pinMode(slot5,INPUT);
  pinMode(slot6,INPUT);
  
  secondTick.attach(1,ISRwatchdog);
  setup_wifi();
  client.setServer(mqtt_server,mqttPort);
  
}

//----------------------------------------Main Loop------------------------------------//

void loop(){

  Serial.print("Watch dog counter:");
  watchdogCount=0;
  Serial.println(watchdogCount);
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  data1=analogRead(slot1);
  data2=analogRead(slot2);
  data3=analogRead(slot3);
  data4=analogRead(slot4);
  data5=analogRead(slot5);
  data6=analogRead(slot6);
  
}// end of main loop
  
  

//-----------------------------WiFi-------------------------------------------//

void setup_wifi() {
    
    delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

//---------------------------While client not conncected---------------------------------//

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if your MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
     //----------------------Subscribing to required topics-----------------------//

     
//      client.subscribe("mushroom/user_input");
//      Serial.println("Subsribed to topic: mushroom/user_input");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()




  

