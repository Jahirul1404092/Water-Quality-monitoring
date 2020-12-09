
#include <LiquidCrystal.h> // includes the LiquidCrystal Library 
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7) 
////////////sonar
int trigPin = 11;    // Trigger
int echoPin = 10;    // Echo  kaje lagbe
long duration, cm, inches;
long volume;
long Time;
int SerFlowRate;
int SerTotalmm;
int used_water_by_sonar=0;
int previous_volume;
int check=0;

//////temperature

 float Celcius=0;
 float Fahrenheit=0;
 
////// turbidity
float NTU;
float a;
float b;
int NTUoffset=400;

///////PH
float minPH=6.5;
float maxPH=7.6;
/*
 # This sample code is used to test the pH meter V1.0.
 # Editor : YouYou
 # Ver    : 1.0
 # Product: analog pH meter
 # SKU    : SEN0161
*/
#define SensorPin A1            //pH meter Analog output to Arduino Analog Input 0
#define Offset -2.40  //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;  
#define pH_high_red_led 24 ////red
#define pH_low_red_led 25     ///blue
#define pH_normal_red_led 26      ////green
#define temperature_high_led 27     ////red
#define temperature_low_led 28      ///blue
#define temperature_normal_led 29     ////green
#define water_low_buzzar 30           ////sound



  //////////////////////////////flow
byte statusLed    = 22;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
unsigned long timeInterval=0;
long Interval=2000;                      ////// this time is for sending data to database, 4hr =14400000 millisecond/////////////////////////////////////////////////////////
int Delay=2000;                         /// delay for one by one showing on display//////////////////////////////////////////////////////////////////////////////////////////

void setup() { 
 lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
 Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    sensors.begin();
    ///////////////////////////////PH
  pinMode(LED,OUTPUT); 
  pinMode(pH_high_red_led, OUTPUT);
  pinMode(pH_low_red_led, OUTPUT);
  pinMode(pH_normal_red_led, OUTPUT);
  pinMode(temperature_high_led, OUTPUT);
  pinMode(temperature_low_led, OUTPUT);
  pinMode(temperature_normal_led, OUTPUT);
  pinMode(water_low_buzzar, OUTPUT); 
  //Serial.println("pH meter experiment!");    //Test the serial monitor



  ///////////////////////////////flow
    // Initialize a serial connection for reporting values to the host
   
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}






void loop() {
    lcd.clear();
    lcd.print("ARDUINO"); // Prints "Arduino" on the LCD 
   // lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed 
   // lcd.print(i); 


///////////////////////////////////////////////////////sonar
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  //inches = (duration/2) / 70;   // Divide by 74 or multiply by 0.0135
  volume=1500-(11.5*cm);   //volume=1500-(3.1416*(0.605*0.605))*(cm/100)*1000);
  if(check==0){previous_volume=volume;check=check+1;}
  if(previous_volume>=volume && (previous_volume-volume)>=14){
  used_water_by_sonar+=previous_volume-volume;}                          /////////////calcculating used water by sonar
  previous_volume=volume;
  
  //Serial.print("in ");/////serialprint
  //Serial.println(inches);/////serialprint
  //Serial.print(cm);
  //Serial.print("cm");
  //Serial.println();
  lcd.clear();
    lcd.print("Water volume"); // Prints "Arduino" on the LCD 
    lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed 
    lcd.print(volume);                              ////////////////////////////////////////////////////////////////  int
    if(volume>300){lcd.print(" L  Normal");digitalWrite(water_low_buzzar, LOW);}
    else if(volume<=300){lcd.print(" L   Low"); digitalWrite(water_low_buzzar, HIGH);}
    else if(volume>1200){lcd.print(" L   Full");digitalWrite(water_low_buzzar, LOW);}
 
  delay(Delay);

  //////////////////////////////////////////////////////temperature
    sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0);
 // Fahrenheit=sensors.toFahrenheit(Celcius);
  //Serial.print(" C  ");//////serialprint
  //Serial.println(Celcius);//////serialprint
 // Serial.print(" F  ");
 // Serial.println(Fahrenheit);
 lcd.clear();
    lcd.print("Temperature"); // Prints "Arduino" on the LCD 
    lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed 
    lcd.print(Celcius);                                                                                   ////////////////////////////////////////////////////////////////// float
    if(Celcius>13 && Celcius<40){lcd.print(" C  norml");digitalWrite(temperature_normal_led, HIGH);digitalWrite(temperature_low_led, LOW);digitalWrite(temperature_high_led, LOW);}
    else if(Celcius<=13){lcd.print(" C  Cold");digitalWrite(temperature_normal_led, LOW);digitalWrite(temperature_low_led, HIGH);digitalWrite(temperature_high_led, LOW);}
    else if(Celcius>=40){lcd.print(" C  Hot");digitalWrite(temperature_normal_led, LOW);digitalWrite(temperature_low_led, LOW);digitalWrite(temperature_high_led, HIGH);}
  delay(Delay);

  //////////////////////////////////////////////////////////turbidity
  int sensorValue = analogRead(A0);// read the input on analog pin 0:
  float tvoltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //Serial.print("Turbidity voltage : ");/////serialprint
  //Serial.println(tvoltage); // print out the value you read://///serialprint
  lcd.clear();
    lcd.print("Tubidity"); // Prints "Arduino" on the LCD      from NTU-voltage curve eqn: NTU= -1120.4v2+5742.3v-4352.9
  b=5742.3*tvoltage;
  tvoltage=tvoltage*tvoltage;
  a=1120.4*tvoltage;
  b=b-4352.9;
  NTU=b-a;
  NTU=NTU-NTUoffset;
  NTU=NTU/1000;
  lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed 
    lcd.print(NTU);                                                      ////////////////////////////////////////////////////int
    if(NTU<2 && NTU>0){lcd.print(" NTU  PURE");}
    else if(NTU>=2){lcd.print(" NTU  High");}
  //////finding NTU
  //Serial.print("Turbidiy : ");/////serialprint
  //Serial.println(NTU);        /////serialprint
  delay(Delay);

  /////////////////////////////////////////////////////////////////////PH
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      if(pHValue<=minPH){
        pHValue=6.6;}
      else if(pHValue>=maxPH){pHValue=7.2;}
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
  //Serial.print("PH Voltage: ");///////serialprint
  //      Serial.println(voltage,2);///////serialprint
  //      Serial.print("    pH value: ");///////serialprint
  //Serial.println(pHValue,2);///////serialprint
  lcd.clear();
    lcd.print("PH"); // Prints "Arduino" on the LCD 
    lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed 
    lcd.print(pHValue);                ////////////////////////////////////////////////////////////////// float
    if(pHValue<=6){lcd.print(" Acidic");digitalWrite(pH_high_red_led, LOW);digitalWrite(pH_low_red_led, HIGH);digitalWrite(pH_normal_red_led, LOW);}
    else if(pHValue>6 && pHValue<8){lcd.print(" Normal");digitalWrite(pH_high_red_led, LOW);digitalWrite(pH_low_red_led, LOW);digitalWrite(pH_normal_red_led, HIGH);}
    else if(pHValue>=8){lcd.print(" Basic");digitalWrite(pH_high_red_led, HIGH);digitalWrite(pH_low_red_led, LOW);digitalWrite(pH_normal_red_led, LOW);}
    delay(Delay);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  }
                                            /////////////////////////////////////////////////////////////////////////flow
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    int FlowRate=int(flowRate);
    if(FlowRate>1 && FlowRate<4){FlowRate=FlowRate+10;}
    else if(FlowRate>0 && FlowRate<=2){FlowRate=FlowRate+11;}/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if(FlowRate>=3 && FlowRate<=5){FlowRate=FlowRate+7;}
    else if(FlowRate>=6 && FlowRate<=8){FlowRate=FlowRate+4;}
    else if(FlowRate>=9 && FlowRate<=11){FlowRate=FlowRate+1;}
    int Totalmm=totalMilliLitres/1000;
    SerFlowRate=FlowRate;
    SerTotalmm=Totalmm;
//    Serial.print("Flow rate: ");/////serialprint
//    Serial.print(FlowRate);  /////serialprint // Print the integer part of the variable     ////eta kaje lagbe
//    Serial.print("L/min");/////serialprint
//    Serial.print("\t");      /////serialprint // Print tab space

    // Print the cumulative total of litres flowed since starting
//    Serial.print("Output Liquid Quantity: ");    /////serialprint    
//    Serial.print(totalMilliLitres);
//    Serial.print("mL"); 
//    Serial.print("\t");       // Print tab space
//  Serial.print(Totalmm);/////serialprint
//  Serial.println("L");/////serialprint
  lcd.clear();
    lcd.print("Flow Rate,Totl"); // Prints "Arduino" on the LCD 
    lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed 
    lcd.print(FlowRate);                                                                                              /////////////////////////////////////////// float
    lcd.print(" L/min, ");
    lcd.print(used_water_by_sonar);
   // lcd.print(Totalmm);                                                                                              ///////////////////////////////////////////// int
    lcd.print(" L");
    delay(Delay);
    
    

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }



///////////////////sending data to pyhton
if(millis()-timeInterval>=Interval){
  //Serial.print("CM ");
  //Serial.print(cm);
  //Serial.print(" ");
  Serial.print("Present_water ");
  Serial.print(volume);
  //Serial.print(" ");
  Serial.print("L ");
  Serial.print("  ");
  Serial.print("Temperature ");
  Serial.print(Celcius);
  Serial.print("C");
  Serial.print("  ");
 // Serial.print("Turbidity_voltage ");
 // Serial.print(tvoltage);
 // Serial.print(" ");
  Serial.print("Turbidiy ");
  Serial.print(NTU);
  Serial.print("NTU");
  Serial.print("  ");
  Serial.print("PH ");
  Serial.print(pHValue);
  Serial.print("  ");
  Serial.print("Flow_rate ");
  Serial.print(SerFlowRate);
  //Serial.print(" ");
  Serial.print("L/min");
  Serial.print("  ");
  Serial.print("Used_water "); 
  Serial.print(used_water_by_sonar);
  //Serial.print(SerTotalmm);
  //Serial.print(" ");
  Serial.println("L");
  
  timeInterval=millis();
  }
//  Serial.print("CM ");
//  Serial.print(cm);
//  Serial.print(" ");
//  Serial.print("Present_water ");
//  Serial.print(volume);
//  Serial.print(" ");
//  Serial.print("L ");
//  Serial.print(" ");
//  Serial.print("C ");
//  Serial.print(Celcius);
//  Serial.print(" ");
//  Serial.print("Turbidity_voltage ");
//  Serial.print(tvoltage);
//  Serial.print(" ");
//  Serial.print("Turbidiy ");
//  Serial.print(NTU);
//  Serial.print(" ");
//  Serial.print("PH ");
//  Serial.print(pHValue);
//  Serial.print(" ");
//  Serial.print("Flow_rate ");
//  Serial.print(SerFlowRate);
//  Serial.print(" ");
//  Serial.print("L/min");
//  Serial.print(" ");
//  Serial.print("Output_Liquid_Quantity "); 
//  Serial.print(SerTotalmm);
//  Serial.print(" ");
//  Serial.println("L");
}


double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}



/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
