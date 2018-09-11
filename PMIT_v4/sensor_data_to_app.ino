// Sending data to app (device pushing data every 1 second

void sendTemperature()
{
  
  Blynk.virtualWrite(V0,t);
}

void sendHumidity()
{
  
  Blynk.virtualWrite(V1,h);
}

void sendCO2()
{
  
  Blynk.virtualWrite(V2,co2ppm);
}

