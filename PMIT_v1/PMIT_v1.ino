int i=0;
//Download adafruit unified sensor library if you haven't already
//-----------------------------Including required libraries----------------------------------------------------------//

#include "DHT.h"
#include <DNSServer.h>
#include <Ticker.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h>    
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//-----------------------------Defining required pins---------------------------------------------------------------//

#define co2_sensor A0
#define dht_dpin D3
#define DHTTYPE DHT22
#define loadPin D5
#define acPin D6
#define fanPin D7
long data_publishing_interval=30000;



//-----------------------------Defining Required Global Variables--------------------------------------------------//


int t,h,co2now[10],co2raw,co2comp,co2ppm,sum;
char sensorData[68];
String msg;
char loginData,loadData,resetData;// for receiving MQTT payload


//------------------------Storing Load Status-------------------------------------------------------------------//

String lightStatus="0";
String acStatus="0";
String fanStatus="0";
String homeStatus;
char loadStatus[6];


//-------------------Storing temperature threshold from user end----------------------------------------------------//

int tempThreshold;
char tempThresholdData[6];


//-------------------Storing co2 threshold from user end----------------------------------------------------//

int co2Threshold;
char co2ThresholdData[6];


//--------------------------------WiFi and MQTT credentials---------------------------------------------------------//

const char* ssid = "Home";
const char* password = "12345678";
const char* mqtt_server = "iot.eclipse.org";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(dht_dpin, DHTTYPE); 

int tempData=0;
int humData=0;
int co2Data=0;

unsigned long lastMsg = 0;
unsigned long previousMillis = 0;



//--------------------ISR for implementing WatchDog--------------------------------------------------------------//


Ticker secondTick;
volatile int watchdogCount=0;
void ISRwatchdog(){

watchdogCount++;
if(watchdogCount==150){


    Serial.println();
    Serial.print("The watch dog bites......");
    //ESP.reset();
    ESP.restart();
  }
}


//--------------------------SETUP function to run only once-------------------------------------------------//


void setup()
{ 

  Serial.begin(115200);// Initializing Serial Monitor
  
  secondTick.attach(1,ISRwatchdog); // Attaching ISRwatchdog function with ticker


  //----------------Initializing sensor pins as input and load pins as output------------------------------//
  
  pinMode(dht_dpin,INPUT);
  pinMode(co2_sensor,INPUT);
  pinMode(loadPin,OUTPUT);
  pinMode(acPin,OUTPUT);
  pinMode(fanPin,OUTPUT);
  
  
  //--------------------------Intializing all loads in off condition-------------------------------------//

  digitalWrite(loadPin,HIGH);
  digitalWrite(acPin,HIGH);
  digitalWrite(fanPin,HIGH);
  
  dht.begin();// Beginning DHT to read humidity and tempearature
  setup_wifi();// Setting up WiFi
  
  client.setServer(mqtt_server,mqttPort);// Setting MQTT server and port
  
  client.setCallback(callback);// calling back callback function when new messages arrives 

}

//----------------------------------------Main Loop-----------------------------------------------------//

void loop(){

  watchdogCount=0;// Reseting the watchdog count

  if (!client.connected()) {
    reconnect();// Connecting to MQTT client & subscribing to required topics
  }
  client.loop();


//---------------------------------Reading sensor data------------------------------------------------//

  
  tempData=temp();
  humData=hum();
  delay(200);
  co2Data=co2();
  
  
  if (tempData<50){

  sensor_data_publish();
  
  }

  
//----------------------------------------Controlling AC based on user threshold---------------------//
  
  
  
  if(tempData>tempThreshold && acStatus=="0")
 {
  //Serial.println("AC On");
  digitalWrite(acPin,LOW);
  acStatus="1";
 } 
 
 else if (tempData<tempThreshold && acStatus=="1")

  {
    //Serial.println("AC off");
    digitalWrite(acPin,HIGH);
    acStatus="0";
  }


//----------------------------------------Controlling Exhaust fan based on user threshold---------------------//
  
  
  
  if(co2Data>co2Threshold && fanStatus=="0")
 {
  //Serial.println("FAN On");
  digitalWrite(fanPin,LOW);
  fanStatus="1";
 } 
 else if (co2Data<co2Threshold && fanStatus=="1")

  {
    //Serial.println("FAN off");
    digitalWrite(fanPin,HIGH);
    fanStatus="0";
  }

}


//----------------------------------Reading Sensor Data--------------------------------------------//


int temp()
{
  
  t = dht.readTemperature();         
 
  //Serial.print("Temperature=");
  //Serial.println(t);
  return t;
   
  }

int hum()
{
  h = dht.readHumidity();
  
  //Serial.print("Humidity=");
  //Serial.println(h);
  return h;
  }

int co2(){


co2raw=0;  //long for raw value of co2
co2comp = 0;   //long for compensated co2 
co2ppm = 0;    //long for calculated ppm
sum=0;
for (int x=0;x<10;x++){

co2now[x]=analogRead(co2_sensor);
sum=sum+co2now[x];
}
co2raw=sum/10;
co2raw=co2raw-55;
co2ppm=map(co2raw,0,1024,300,2000);


  //Serial.print("co2 in ppm=");
  //Serial.println(co2ppm);
  return co2ppm; 
}

//-------------------------------------------------------WiFi-------------------------------------------//

void setup_wifi() {

    /*
    WiFiManager wifiManager;
    //wifiManager.resetSettings();
    wifiManager.autoConnect("Home Automation", "admin1234");
    Serial.println("Connected.");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    */
    
  WiFi.begin(ssid,password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
}

//-------------------------------------------While client not conncected---------------------------------//

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if your MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      
      //publising "device online" to mushroom/online to remotely check if device is online

      client.publish("Sensor/online","Device online");
     
     //once connected to MQTT broker, subscribe command if any


//--------------------------------------Subscribing to required topics---------------------------------//

     
      client.subscribe("home/user_input");
      Serial.println("Subsribed to topic: home/user_input");
      client.subscribe("home/temp");
      Serial.println("Subscribed to topic: home/temp");
      client.subscribe("home/login");
      Serial.println("Subscribed to topic: home/co2");
      client.subscribe("home/co2");
      Serial.println("Subscribed to topic: home/login");
      client.subscribe("home/reset");
      Serial.println("Subscribed to topic: home/reset");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()


//--------------------------Publishing sensor data every 30 seconds----------------------------------//


void sensor_data_publish(){

  unsigned long now=millis();
  if(now-lastMsg>data_publishing_interval){
    
    lastMsg=now;
    msg=""; 
    msg= msg+tempData+","+humData+","+co2Data;
    
    msg.toCharArray(sensorData,68);
   //Serial.println(msg);
   Serial.println(sensorData);
   client.publish("home/sensor_data",sensorData);
  }
}



//------------------------------------------Callback funtion-------------------------------------//


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);



//-------------------------------Load Control Remotely--------------------------------------------------------//


if(strcmp(topic,"home/user_input")==0){


  for(int i=0;i<length;i++){


    Serial.print((char)payload[i]);
    loadData=payload[i];
  
  if (loadData=='1'){


    digitalWrite(loadPin,LOW);
    lightStatus="1";
  }
  else if(loadData=='0'){

    digitalWrite(loadPin,HIGH);
    lightStatus="0";
  }
}}


//-------------------------------Getting temperature threshold from user--------------------------------//


      if(strcmp(topic, "home/temp") == 0)
      {
        memset(tempThresholdData,0, sizeof(tempThresholdData));// Emptying the char array
        Serial.print("Message:");
        for (int i = 0; i < length; i++) {
        tempThresholdData[i]=payload[i];
    }
    
    
    tempThreshold=atoi(tempThresholdData);// Converting char array to int
    Serial.println(tempThreshold);
    
    }


//-------------------------------Getting co2 threshold from user--------------------------------//


      if(strcmp(topic, "home/co2") == 0)
      {
        memset(co2ThresholdData,0, sizeof(co2ThresholdData));// Emptying the char array
        Serial.print("Message:");
        for (int i = 0; i < length; i++) {
        co2ThresholdData[i]=payload[i];
    }
    
    
    co2Threshold=atoi(co2ThresholdData);// Converting the char array to int
    Serial.println(co2Threshold);
    
    
    }

//-------------------------Publishing Home status upon request from user app--------------------------//

  
  if(strcmp(topic, "home/login") == 0){
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    loginData=payload[i];


    if (loginData=='1')
    {

      homeStatus=acStatus+fanStatus+lightStatus;
      homeStatus.toCharArray(loadStatus,6);
      client.publish("home/load_status",loadStatus);
      Serial.print("Published load_status:");
      Serial.println(lightStatus);
    }}}


//----------------------------------Restarting the board from Engineer's end----------------------------//

  
  if(strcmp(topic, "home/reset") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    resetData=payload[i];


    if (resetData=='1')
    {

      Serial.println("Resetting Device.........");
       ESP.restart();
      }}}}// End of callback function
