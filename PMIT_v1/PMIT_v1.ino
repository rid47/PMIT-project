//Download adafruit unified sensor library if you haven't already
//-----------------------------Including required libraries-------------------------------------------------//

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//-----------------------------Defining required pins-------------------------------------------------------//

#define co2_sensor A0
#define dht_dpin D3 
#define DHTTYPE DHT22

long data_publishing_interval=1000;

//--------------------------------WiFi and MQTT credentials-----------------------------------------------//

const char* ssid = "home";
const char* password = "ridwanmizan";
const char* mqtt_server = "iot.eclipse.org";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(dht_dpin, DHTTYPE); 

int data1=0;
int data2=0;
int data3=0;

unsigned long lastMsg = 0;
unsigned long previousMillis = 0;

void setup()
{ 

  Serial.begin(115200);
  Serial.println("I'm here");
  pinMode(dht_dpin,INPUT);
  pinMode(co2_sensor,INPUT);

  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server,mqttPort);
}

//----------------------------------------Main Loop------------------------------------//

void loop(){

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  data1=temp();
  data2=hum();
  delay(200);
  data3=co2();
  
  
  if (data1<50){

  sensor_data_publish();
  
  }

}


//-------------------------------Reading Sensor Data--------------------------------------------//


int temp()
{
  
  int t = dht.readTemperature();         
  return t;
  Serial.print("Temperature=");
  Serial.println(t);
   
  }

int hum()
{
  int h = dht.readHumidity();
  return h;
  Serial.print("Humidity=");
  Serial.println(h);
  }

int co2(){

int co2now[10];//long array for co2 readings
int co2raw=0;  //long for raw value of co2
int co2comp = 0;   //long for compensated co2 
int co2ppm = 0;    //long for calculated ppm
int sum=0;
for (int x=0;x<10;x++){

co2now[x]=analogRead(co2_sensor);
sum=sum+co2now[x];
}
co2raw=sum/10;
co2raw=co2raw-55;
co2ppm=map(co2raw,0,1024,300,2000);
return co2ppm; 

  Serial.print("C02 in ppm=");
  Serial.println(co2ppm);
}

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
      
      //publising "device online" to mushroom/online to remotely check if device is online

      client.publish("home/online","Device online");
     
     //once connected to MQTT broker, subscribe command if any
     //----------------------Subscribing to required topics-----------------------//

     
      //client.subscribe("mushroom/user_input");
      //Serial.println("Subsribed to topic: mushroom/user_input");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()


//------------------------------Publishing sensor data every 5 minutes----------------------------------//


void sensor_data_publish(){

  unsigned long now=millis();
  if(now-lastMsg>data_publishing_interval){
    
    lastMsg=now;
    String msg=""; 
    msg= msg+ data2+","+data1+","+data3;
    char message[68];
    msg.toCharArray(message,68);
   //Serial.println(msg);
    Serial.println(message);
    client.publish("home/sensor_data",message);
  }
}



  
 

