#include <Ticker.h>
#include <B31DGMonitor.h>

void CycleF();

//Define Input Pins on ESP32C
#define T1_LED 19
#define T2_INPUT 7
#define T3_INPUT 2
#define T4_INPUT 1
#define T4_LED 0

Ticker Cycle; //Define Ticker 
B31DGCyclicExecutiveMonitor monitor;  //Define Cyclic Executive Monitor

int T2_Freq = 0;
int T3_Freq = 0;
int T4_counter = 0;
int T2_NextCycle = 0;
double reading1 = 0;
double reading2 = 0;
double reading3 = 0;
double reading4 = 0;
double read_average;
int counter = 0;

//B31DGMonitor monitor;

void setup(void)
{
  Serial.begin(9600); // Starts the serial communication and sets baud rate to 9600
  
  //Setup Pins
  pinMode(T1_LED, OUTPUT);
  pinMode(T2_INPUT, INPUT);
  pinMode(T3_INPUT, INPUT);
  pinMode(T4_INPUT, INPUT);
  pinMode(T4_LED, OUTPUT); 

  Cycle.attach(4000, CycleF); 

  monitor.startMonitoring(); // all the tasks should be released after this, this time can be queried using getTimeStart()
}

void task1() {
   monitor.jobStarted(1);  // i between [1..5], i.e. 1 is digital output, 5 is serial print
   //Perform task1 - Signal Pulse
   digitalWrite(T1_LED, HIGH);
   delayMicroseconds(200);
   digitalWrite(T1_LED, LOW);
   delayMicroseconds(50);
   digitalWrite(T1_LED, HIGH);
   delayMicroseconds(30);
   digitalWrite(T1_LED, LOW);
   monitor.jobEnded(1);
}

void task2() {
   monitor.jobStarted(2);
   //Perform task2 - Measure Frequency
   int Freq_Input_High = pulseIn(T2_INPUT, HIGH);
   //int Freq_Input_Low = pulseIn(T2_INPUT, LOW);
   T2_Freq = 1 / (2*Freq_Input_High);
   monitor.jobEnded(2);
}

void task3() {
   monitor.jobStarted(3);
   //Perform task3 - Measure Frequency
   int Freq_Input_High = pulseIn(T3_INPUT, HIGH);
   //int Freq_Input_Low = pulseIn(T3_INPUT, LOW);
   T3_Freq = 1 / (2*Freq_Input_High);
   monitor.jobEnded(3);
}

void task4() {
   monitor.jobStarted(4);
   //Perform task4 - Measure Potentiometer Signal
   if (T4_counter == 4)  //Reset counter after 4 readings
   {
     T4_counter = 0;
   }

   if (T4_counter == 0)
   {
     reading1 = analogRead(T4_INPUT);  //Read Analog Signal
   }
   else if (T4_counter == 1)
  {
    reading2 = analogRead(T4_INPUT);  //Read Analog Signal
  }
    else if (T4_counter == 2)
  {
    reading3 = analogRead(T4_INPUT);  //Read Analog Signal
  }
    else if (T4_counter == 3)
  {
    reading4 = analogRead(T4_INPUT);  //Read Analog Signal
  }
   read_average = (reading1 + reading2 + reading3 + reading4) / 4;  //Calculate average

   if (read_average > 2047)
   {
     digitalWrite(T4_LED, HIGH);  //Error LED High
   }
   else
   {
     digitalWrite(T4_LED, LOW); //Error LED Low
   }
   T4_counter++;  //Increment Task 4 Counter
   monitor.jobEnded(4);
}

void task5() {
   monitor.jobStarted(5);
   //Perform task5 - Display task2 and task3 frequencies
   //Set 333Hz as minimum threshold
  if(T2_Freq < 333)
  {
  T2_Freq = 333;
  }
  //Set 1000Hz as Maximum Threshold
  else if(T2_Freq > 1000)
  {
    T2_Freq = 1000;
  }
  //Set 500Hz a Minimum Threshold
  if(T3_Freq < 500)
  {
    T3_Freq = 500;
  }
  //Set 1000Hz as Maximum Threshold
  else if(T3_Freq > 1000)
  {
    T3_Freq = 1000; 
  }
  T2_Freq = (T2_Freq - 333) * 0.127;  //Scaling to between 0 - 99
  T3_Freq = (T3_Freq - 500) * 0.198;   //Scaling to between 0 - 99
  Serial.printf("T2 Frequency = %d, T3 Frequency = %d \n", T2_Freq, T3_Freq); // print T2 and T3 Frequencies
  monitor.jobEnded(5);
}

void CycleF()
{
  //Reset Frame Counter after highest common denominator 
  if(counter > 50)
  {
    counter = 0;
  }
  task1();
  if(counter % 2 == 0)
  {
    task3();
  }
  if(counter % 5 ==0 && counter % 2 == 0)
  {
    T2_NextCycle = 1;
  }
  else if(T2_NextCycle = 1)
  {
    task2();
    T2_NextCycle = 0;
  }
  if(counter % 5 == 0)
  {
    task4();
  }
  if(counter % 25 == 0)
  {
    task5();
  }
  counter++;

}

// simple example of cyclic executive (version not using Ticker)
void loop(void) 
{
  // uncomment this to find execution time of given tasks 
  /*
  unsigned long bT = micros();
  for (int i=0; i<1; i++) {
    task5();
  }
  unsigned long timeItTook = micros()-bT;
  Serial.print("Duration SerialOutput Job = ");
  Serial.print(timeItTook/1);
  exit(0);
  */
}
