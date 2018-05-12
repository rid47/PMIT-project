#define eggPin1 D0


//------------------------REQUIRED VARIABLES--------------------------------------//


int eggPresence1=0;
_Bool flag1=false;


//-------------------------SET UP FUNCTION----------------------------------------//


void setup() {

  Serial.begin(115200);
  pinMode(eggPin1,INPUT);

}


//-------------------------MAIN LOOP----------------------------------------------//

void loop() {
  
  eggPresence1=digitalRead(eggPin1);
  Serial.println(eggPresence1);
  delay(1000);

  
  
  if(eggPresence1==1 && flag1==true)
  
  {
  Serial.println("Pot 1 occupied");
  flag1=false;
  }
  
  else if (eggPresence1==0 && flag1==false)    
  {
  Serial.println("Pot 1 is empty");
  flag1=true;
  }

}
