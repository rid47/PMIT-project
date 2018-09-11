// Getting data from app

//Geyser on/off

BLYNK_WRITE(V4)
{
  loadData = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V4 button value is: ");
  Serial.println(loadData);
  if(loadData==1)
  {
  Serial.println("loadData is 1");
  digitalWrite(loadPin,HIGH);
  }
  if(loadData==0){
    Serial.println("loadData is 0");
    digitalWrite(loadPin,LOW);
  }
 
  
}

//AC on/off

BLYNK_WRITE(V5)
{
  acData = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V5 button value is: ");
  Serial.println(acData);
  if(acData==1)
  {
  Serial.println("acData is 1");
  digitalWrite(acPin,HIGH);
  }
  if(acData==0){
    Serial.println("acData is 0");
    digitalWrite(acPin,LOW);
  }
 
  
}


//FAN on/off

BLYNK_WRITE(V6)
{
  fanData = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V6 button value is: ");
  Serial.println(fanData);
  if(fanData==1)
  {
  Serial.println("fanData is 1");
  digitalWrite(fanPin,HIGH);
  }
  if(fanData==0){
    Serial.println("fanData is 0");
    digitalWrite(fanPin,LOW);
  }
 
}
