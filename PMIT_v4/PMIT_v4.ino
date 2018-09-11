#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer; // Create a Timer object called "timer"! 


#include "DHT.h"

//Defining required pins

#define loadPin D5
int loadData=0;
#define acPin D6
int acData=0;
#define fanPin D7
int fanData=0;
#define co2_sensor A0
#define dht_dpin D3
#define DHTTYPE DHT22
DHT dht(dht_dpin, DHTTYPE);
int t,h,co2now[10],co2raw,co2comp,co2ppm,sum;

char auth[] = "6f9e1d82e02a402499728f40767947a8";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "real08";
char pass[] = "rashedmizan";

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(dht_dpin,INPUT);
  pinMode(co2_sensor,INPUT);
  pinMode(loadPin,OUTPUT);
  pinMode(fanPin,OUTPUT);
  pinMode(acPin,OUTPUT);

  digitalWrite(loadPin,LOW);
  digitalWrite(acPin,LOW);
  digitalWrite(fanPin,LOW);
  
    dht.begin();// Beginning DHT to read humidity and tempearature

    timer.setInterval(1000L,temp);
    timer.setInterval(1000L,hum);
    timer.setInterval(1000L,co2);
    timer.setInterval(1000L,sendTemperature);
    timer.setInterval(1000L,sendHumidity);
    timer.setInterval(1000L,sendCO2); 
    

}

void loop() {
  // put your main code here, to run repeatedly:

  Blynk.run();
  timer.run();

  
}



