//follow connection below

const int boost1 = 2; //boost, connect valve to the pump to pin 2
const int boost2 = 3; //boost, connect valve to dump the water to pin 4
const int relay1 = 4; //relay switch, pump water up the reservoir or dump water
const int relay2 = 5; //relay switch, pump water down to water the plants
const int subPump = 8; //submersible pump, regulate pH

/*pH sensor connection
 * Red -> 5V
 * Blue -> A0
 * Black -> GND
 */

//time duration for different processes, according to preference
int pumpTime = 5000; //pump for 5 second
int delaySystem = 5000; //delay system for 1 second to make sure all water is pumped up to the reservoir
int releaseTime = 5000; //release water from reservoir for 1 minute
int waterDrip = 5000; //let water to drip to the bottom of the container for 2 hours
int regTime = 5000; //time to release ammonia

//below for pH sensor
#define SensorPin 0          //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(boost1, OUTPUT);
  pinMode(boost2, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(subPump, OUTPUT);  
  Serial.begin(9600);  
  Serial.println("All System Ready");
}

void loop() {
  //this is how the whole system works
  /*pump water up to reservoir
                    three cycle*/
  //first cycle
  watering();
  pumpToRes();
  phSensor();
  
  //second cycle
  watering();
  pumpToRes();
  phSensor();
  
  //third cycle
  watering();
  
  /*water is used to water for three times,
                    water will be disposed after this*/
  
  dispose();
}


//main processes below (aka functions)
void pumpToRes(){
  relayInvert(relay1, HIGH); //pin 6
  valveOn(boost1); //pin 2
  Serial.println("Water now pumped up to reservoir!");
  delay(pumpTime); 
  valveOff(boost1);//pin 2
  relayInvert(relay1, LOW);//pin 6
  delay(delaySystem);
}

void dispose(){
  relayInvert(relay1, HIGH); //pin 6
  valveOn(boost2); //pin 4
  Serial.println("water disposed out of the system!");
  delay(pumpTime);
  valveOff(boost2);//pin 4
  relayInvert(relay1, LOW);//pin 6
  delay(delaySystem);
}

void watering(){
  //Use this instead!
  relayInvert(relay2, HIGH);
  delay(releaseTime);
  relayInvert(relay2, LOW);
  delay(waterDrip);
/*Below for debugging
  digitalWrite(relay2, HIGH);
  Serial.println("Watering now...");
  delay(releaseTime);
  digitalWrite(relay2, LOW);
  delay(delaySystem);*/
}

void phSensor(){
  Serial.println("Measuring pH...");
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" "); 
  if (phValue < 7){
    digitalWrite(subPump, HIGH);
    Serial.println("Regulating pH....");
  }else{
    digitalWrite(subPump, LOW);
  }
  while (phValue < 7){
    phSensor();
  }
  delay(delaySystem);
}


void relayInvert(int relayNo,boolean bin){
  //make inverted logic to normal logic
  boolean state = !bin; //Use this instead!!
  //boolean state = bin; //for debugging purpose
  digitalWrite(relayNo, state);
}

void valveOn(int boostNo){
  digitalWrite(boostNo, HIGH);
}

void valveOff(int boostNo){
  digitalWrite(boostNo, LOW);
}
