//-----------------------------Including required libraries----------------------------------------------------------//


#include <DNSServer.h>
#include <Ticker.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


//-----------------------------Defining required pins---------------------------------------------------------------//



#define eggPin1 D3
#define eggPin2 D4
#define eggPin3 D5
#define eggPin4 D6
#define eggPin5 D7
#define eggPin6 D8



//---------------------------------------REQUIRED VARIABLES-------------------------------------------------------//


int eggPresence1=0;
_Bool flag1=false;
int eggPresence2=0;
_Bool flag2=false;
int eggPresence3=0;
_Bool flag3=false;
int eggPresence4=0;
_Bool flag4=false;
int eggPresence5=0;
_Bool flag5=false;
int eggPresence6=0;
_Bool flag6=false;

int eggCount=0;
char Data,resetData;// for receiving MQTT payload
char eggData[2];
String msg;
int eggThreshold=2;

//--------------------------------WiFi and MQTT credentials---------------------------------------------------------//

//const char* ssid = "home";
//const char* password = "ridwanmizan";
const char* mqtt_server = "iot.eclipse.org";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);



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


//-------------------------SET UP FUNCTION----------------------------------------//


void setup() {

  Serial.begin(115200);

  secondTick.attach(1,ISRwatchdog); // Attaching ISRwatchdog function with ticker
   
  pinMode(eggPin1,INPUT);
  pinMode(eggPin2,INPUT);
  pinMode(eggPin3,INPUT);
  pinMode(eggPin4,INPUT);
  pinMode(eggPin5,INPUT);
  pinMode(eggPin6,INPUT);

  setup_wifi();// Setting up WiFi
  
  client.setServer(mqtt_server,mqttPort);// Setting MQTT server and port
  
  client.setCallback(callback);// calling back callback function when new messages arrives 


}


//-------------------------MAIN LOOP----------------------------------------------//

void loop() {


  watchdogCount=0;// Reseting the watchdog count

  if (!client.connected()) {
    reconnect();// Connecting to MQTT client & subscribing to required topics
  }
  client.loop();


//---------------------------------Reading Button Status------------------------------------//
   
  eggPresence1=digitalRead(eggPin1);
  eggPresence2=digitalRead(eggPin2);
  eggPresence3=digitalRead(eggPin3);
  eggPresence4=digitalRead(eggPin4);
  eggPresence5=digitalRead(eggPin5);
  eggPresence6=digitalRead(eggPin6);
  
//  Serial.println(eggPresence1);
  delay(1000);

//Suggestion: use string concat and for loop: https://stackoverflow.com/questions/308695/how-do-i-concatenate-const-literal-strings-in-c//

// ------------------Checking Slot 1---------------------------//  
  
  if(eggPresence1==1 && flag1==true)
  
  {
  Serial.println("Pot 1 empty");
  flag1=false;
  eggCount-=1;
  }
  
  else if (eggPresence1==0 && flag1==false)    
  {
  Serial.println("Pot 1 is occupied");
  flag1=true;
  eggCount+=1;
  }

  // ------------------Checking Slot 2---------------------------//
  
  if(eggPresence2==1 && flag2==true)
  
  {
  Serial.println("Pot 2 empty");
  flag2=false;
  eggCount-=1;
  }
  
  else if (eggPresence2==0 && flag2==false)    
  {
  Serial.println("Pot 2 is occupied");
  flag2=true;
  eggCount+=1;
  }

// ------------------Checking Slot 3---------------------------//

   if(eggPresence3==1 && flag3==true)
  
  {
  Serial.println("Pot 3 empty");
  flag3=false;
  eggCount-=1;
  }
  
  else if (eggPresence3==0 && flag3==false)    
  {
  Serial.println("Pot 3 is occupied");
  flag3=true;
  eggCount+=1;
  }

// ------------------Checking Slot 4---------------------------//
   
   if(eggPresence4==1 && flag4==true)
  
  {
  Serial.println("Pot 4 empty");
  flag4=false;
  eggCount-=1;
  }
  
  else if (eggPresence4==0 && flag4==false)    
  {
  Serial.println("Pot 4 is occupied");
  flag4=true;
  eggCount+=1;
  }

// ------------------Checking Slot 5---------------------------//

   if(eggPresence5==1 && flag5==true)
  
  {
  Serial.println("Pot 5 empty");
  flag5=false;
  eggCount-=1;
  }
  
  else if (eggPresence5==0 && flag5==false)    
  {
  Serial.println("Pot 5 is occupied");
  flag5=true;
  eggCount+=1;
  }

// ------------------Checking Slot 6---------------------------//

   if(eggPresence6==1 && flag6==true)
  
  {
  Serial.println("Pot 6 empty");
  flag6=false;
  eggCount-=1;
  }
  
  else if (eggPresence6==0 && flag6==false)    
  {
  Serial.println("Pot 6 is occupied");
  flag6=true;
  eggCount+=1;
  }
  
  // Publishing while egg is less then threshold;

  if(eggCount<eggThreshold){

    client.publish("Refrigerator/eggAlert","1");
  }
  }


//-------------------------------------------------------WiFi-------------------------------------------//

void setup_wifi() {


    WiFiManager wifiManager;
    //wifiManager.resetSettings();
    wifiManager.autoConnect("Smart Refrigerator", "admin1234");
    Serial.println("Connected.");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
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

      client.publish("Refrigerator/online","Device online");
     
     //once connected to MQTT broker, subscribe command if any


//--------------------------------------Subscribing to required topics----------------------------------//

     
      client.subscribe("Refrigerator/egg_status");
      Serial.println("Subsribed to topic: Refrigerator/egg_status");

      client.subscribe("Refrigerator/reset");
      Serial.println("Subscribed to topic: Refrigerator/reset");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()


//------------------------------------------Callback funtion-------------------------------------//


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);



//-------------------------------Checking Refrigerator Status from App---------------------------//


if(strcmp(topic,"Refrigerator/egg_status")==0){


  for(int i=0;i<length;i++){


    Serial.print((char)payload[i]);
    Data=payload[i];
  
  if (Data=='1'){

    msg=""; 
    msg= msg+eggCount;
    
    msg.toCharArray(eggData,2);
   //Serial.println(msg);
   Serial.println(eggData);
   client.publish("Refrigerator/egg_data",eggData);

   
  }
 }
}
//----------------------------------Restarting the board from Engineer's end----------------------------//

  
  if(strcmp(topic, "Refrigerator/reset") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    resetData=payload[i];


    if (resetData=='1')
    {

      Serial.println("Resetting Device.........");
       ESP.restart();
      }}}}// End of callback function
