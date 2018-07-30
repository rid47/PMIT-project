//-----------------------------Including required libraries-------------------------------------------------//
#include "SPIFFS.h"
#include "WiFi.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include<ESP32Ticker.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>
Ticker secondTick;

//--------------------- Array to initialize pin and store data----------------------------------------------//

int data[6];
int counter[6]={0};
int totalEggs=0,previousTotal=0;
char eggsCount[4];

//-----------------------------Defining required pins-------------------------------------------------------//

int slot[6]={36,39,34,35,32,33};

//----------------------------------WiFi and MQTT credentials-----------------------------------------------//

//const char* ssid = "real08";
//const char* password = "rashedmizan";
const char* mqtt_server = "iot.eclipse.org";
const int mqttPort = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

//------------------------------------------------------WATCH DOG SUBROUTINE-------------------------------//

volatile int watchdogCount=0;
void ISRwatchdog(){


  watchdogCount++;
  if(watchdogCount==100){


    //Serial.println();
    Serial.print("The watch dog bites......");
    ESP.restart();
  }
}

//--------------------------------------------------------SET UP FUNCTION---------------------------------//


void setup()
{ 

  Serial.begin(115200);
  
  // Initializing pin as input
  
  for(int i=0;i<sizeof(slot)/sizeof(int);++i){

      pinMode(slot[i],INPUT);
    
  }
  
  // Setting intial analogRead data value to zero
  
  for (int i=0;i<sizeof(data)/sizeof(int);++i)

  {
    //Serial.println("---------------------Initialization-------------------------");

    data[i]=0;
    Serial.print("data");
    Serial.print(i);
    Serial.println(data[i]);
  }

  WiFiManager wifiManager;
  secondTick.attach(1,ISRwatchdog);
  if (!wifiManager.autoConnect("Egg Tray", "admin1234")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  Serial.println("connected");//connected to wifi
  
  //Establishing MQTT connection
  
  client.setServer(mqtt_server,mqttPort);
  client.setCallback(callback);
  
}

//--------------------------------------------------Main Loop------------------------------------//

void loop(){

//  Serial.print("Watch dog counter:");
    watchdogCount=0;
//  Serial.println(watchdogCount);
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  for(int i=0;i<sizeof(data)/sizeof(int);++i)//looping till the end of the array
  
  {

//    Serial.print("slot:");
//    Serial.println(slot[i]);
    data[i]=analogRead(slot[i]);
    //data[i]=digitalRead(slot[i]);
    delay(300);
    Serial.print("data");
    Serial.print(i);
    Serial.print(":");
    Serial.println(data[i]);
    
    // Incresing or decreasing counter based on sensor reading
    
    if(data[i]<3000 && counter[i]==0){
        Serial.println("I'm in counter increasing loop");
        counter[i]++;
        Serial.print("counter[");
        Serial.print(i);
        Serial.print("]:");
        Serial.println(counter[i]);

        // Counting total number of eggs

        for (int j=0;j<sizeof(counter)/sizeof(int);j++){

              totalEggs+=counter[j];
              
  
                }
                
                Serial.print("Total number of eggs available:");
                Serial.println(totalEggs);

                

               //Push notification when total eggs is ==1

                
                if(totalEggs==1){

                 
                 
                  
                  client.publish("home/eggAlert","1");
                  Serial.print("published to home/eggAlert:");
                  Serial.println(eggsCount);

                  
                  }
                //Publishing updated data

                  String eggs="";  
                  eggs=eggs+totalEggs;
                 
                  eggs.toCharArray(eggsCount,4);
//                  client.publish("home/eggs",eggsCount);
//                  Serial.print("published to home/eggs:");
//                  Serial.println(eggsCount);

//                String pubString =String(totalEggs);
//                char message_buff[]=""; 
//                pubString.toCharArray(message_buff, pubString.length()+1); 
//                Serial.println(pubString); 
//                client.publish("home/eggs", message_buff);
                  totalEggs=0;//resetting the totalEggs after publishing updated data
                }

    if(data[i]>3000 && counter[i]==1)
    {
       Serial.println("I'm in counter decreasing loop");
       counter[i]--;
       Serial.print("counter[");
       Serial.print(i);
       Serial.print("]:");
       Serial.println(counter[i]);

       // Counting total number of eggs

      for (int j=0;j<sizeof(counter)/sizeof(int);j++){

      totalEggs+=counter[j];
      }
      Serial.print("Total number of eggs available:");
      Serial.println(totalEggs);  
        
        
      //Push notification when total eggs is ==1

                
                if(totalEggs==1){

                 
                 
                  
                  client.publish("home/eggAlert","1");
                  Serial.print("published to home/eggAlert:");
                  Serial.println(eggsCount);

                  
                  }
                
        
        
        
        String eggs="";
        eggs=eggs+totalEggs;
        eggs.toCharArray(eggsCount,4);
//        client.publish("home/eggs",eggsCount);
//        Serial.print("published to home/eggs:");
//        Serial.println(eggsCount);

//      String pubString =String(totalEggs);
//      char message_buff[]=""; 
//      pubString.toCharArray(message_buff, pubString.length()+1); 
//      Serial.println(pubString); 
//      client.publish("home/eggs", message_buff); 
        totalEggs=0;
//      
      }
      


      //Publishing updated data

   

}//end of reading and counting eggs

}// end of main loop
  
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
      client.publish("Refrigerator/online","Device online");
     //once connected to MQTT broker, subscribe command if any
     //----------------------Subscribing to required topics-----------------------//

     
      client.subscribe("home/eggStatus");
      Serial.println("Subsribed to topic: home/eggStatus");
      client.subscribe("home2/reset");
      Serial.println("Subsribed to topic: home/reset");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()



//-----------------------Callback function-------------------------------------//


void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  
//-----------------------Publishing egg tray status upon request-------------//

if(strcmp(topic, "home/eggStatus") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data=payload[i];
    
  
      if (data=='1')
    {
      client.publish("home/eggs",eggsCount);
      Serial.print("published to home/eggs:");
      Serial.println(eggsCount);
      }
  }}




//---------------------------Reset the board from Engineer's end----------------------------//

if(strcmp(topic, "home2/reset") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data2=payload[i];
    
  
    if (data2=='1')
    {
      
      Serial.println("Resetting NodeMCU.........");
      ESP.restart();
      }}}
}//End of callback
  



  

