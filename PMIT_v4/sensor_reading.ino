void temp()
{
  
  t = dht.readTemperature();         
 
  Serial.print("Temperature=");
  Serial.println(t);
  //return t;
   
  }

void hum()
{
  h = dht.readHumidity();
  
  Serial.print("Humidity=");
  Serial.println(h);
  //return h;
  }

void co2(){


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


  Serial.print("co2 in ppm=");
  Serial.println(co2ppm);
  //return co2ppm; 
}


