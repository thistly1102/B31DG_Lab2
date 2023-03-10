#include <Ticker.h>
#include <B31DGMonitor.h>

//Define Input Pins on ESP32C
#define T1_LED 18
#define T2_INPUT 0
#define T3_INPUT 1
#define T4_INPUT 2
#define T4_LED 19

Ticker Cycle; //Define Ticker 
B31DGCyclicExecutiveMonitor monitor;  //Define Cyclic Executive Monitor

float Freq_Input_High = 0;
int T2_Freq = 0;  //Frequency Reading for Task 2
int T3_Freq = 0;  //Frequency Reading for Task 3
unsigned int slot = 0;
int T2_NextCycle = 0; //Variable to Prevent Task 2 and Task 3 running in the same frame

//Reading Values for Task 4
double reading1 = 0;
double reading2 = 0;
double reading3 = 0;
double reading4 = 0;
double read_average = 0;

int frame_counter = 0;  //frame counter for cyclic executive operation

void setup(void)
{
  Serial.begin(9600); // Starts the serial communication and sets baud rate to 9600
    
  //Setup Input and LED Pins
  pinMode(T1_LED, OUTPUT);
  pinMode(T2_INPUT, INPUT);
  pinMode(T3_INPUT, INPUT);
  pinMode(T4_INPUT, INPUT);
  pinMode(T4_LED, OUTPUT);

  Cycle.attach_ms(4, CycleF); //Ticker Interrupts every 4000us (4ms)
  //Prevent Fale Violations from B31DG Monitor
  task1(); 
  delayMicroseconds(2000);

  monitor.startMonitoring(); // all the tasks should be released after this, this time can be queried using getTimeStart()
}

void task1() {
   monitor.jobStarted(1);  // i between [1..5], i.e. 1 is digital output, 5 is serial print
   //Perform task1 - Signal Pulse
   digitalWrite(T1_LED, HIGH);  //Set LED HIGH
   delayMicroseconds(200);  //Delay 200us
   digitalWrite(T1_LED, LOW);  //Set LED Low
   delayMicroseconds(50); //Delay 50us
   digitalWrite(T1_LED, HIGH);  //Set LED HIGH
   delayMicroseconds(30); //Delay 50us  
   digitalWrite(T1_LED, LOW); //Set LED LOW
   monitor.jobEnded(1);
}

void task2() {
   monitor.jobStarted(2);
   //Perform task2 - Measure Frequency
   Freq_Input_High = pulseIn(T2_INPUT, HIGH, 4000);
   T2_Freq = 1000000 / (2*Freq_Input_High); //Hz
   //Serial.print(T2_Freq);
   monitor.jobEnded(2);
}

void task3() {
   monitor.jobStarted(3);
   //Perform task3 - Measure Frequency
   Freq_Input_High = pulseIn(T3_INPUT, HIGH, 4000); //us
   T3_Freq = 1000000 / (2*Freq_Input_High);   //Hz
   Serial.print(T3_Freq);
   monitor.jobEnded(3);
}

void task4() {
   monitor.jobStarted(4);
   //Perform task4 - Measure Potentiometer Signal
  //Obtain Input Readings
    reading1 = analogRead(T4_INPUT);  //Read Analog Signal
    reading2 = analogRead(T4_INPUT);  //Read Analog Signal
    reading3 = analogRead(T4_INPUT);  //Read Analog Signal
    reading4 = analogRead(T4_INPUT);  //Read Analog Signal
   read_average = (reading1 + reading2 + reading3 + reading4) / 4;  //Calculate average

  if (read_average > 2047)
  {
    digitalWrite(T4_LED, HIGH);  //Error LED High
  }
  else
  {
    digitalWrite(T4_LED, LOW); //Error LED Low
  }
  //Serial.print(read_average);
   
  monitor.jobEnded(4);
}

void task5() {
   //Operate every 25 frames (100ms)
   if(frame_counter % 25 == 0)
   {
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
      // print T2 and T3 Frequencies
      Serial.print("\n T2 Frequency = ");
      Serial.print(T2_Freq);
      Serial.print("\n T3 Frequency = ");
      Serial.print(T3_Freq);
      Serial.print("\n");
      monitor.jobEnded(5);
   }
}

void CycleF()
{
  //200 frames is the smallest common denominator for the task periods
  if(frame_counter > 199)
  {
    frame_counter = 0;
  }
  //Each Frame is 4ms
  //Tasks are run in their respective frames
  //Tasks are arranged such that the total run time within each frame does not exceed the frame length
  //Task 1 is run every frame (4ms)
  //Task 2 and Task 3 are never run in the same frame as the time taken would exceed the frame length
  //Task 5 is called every 5 frames, however will only be run every 25 frames (100ms)
  slot = frame_counter % 10;
  switch (slot)
  {
    case 0: task1();            task3();  task4();  task5();  break;
    case 1: task1();  task2();                                break; 
    case 2: task1();            task3();                      break;
    case 3: task1();                                          break;
    case 4: task1();            task3();                      break;
    case 5: task1();  task2();            task4();  task5();  break;
    case 6: task1();            task3();                      break;
    case 7: task1();                                          break;
    case 8: task1();            task3();                      break;
    case 9: task1();                                          break;
  }
  frame_counter++;  
}

// simple example of cyclic executive (version not using Ticker)
void loop(void) 
{
  // uncomment this to find execution time of given tasks 
  /*
  unsigned long bT = micros();
  for (int i=0; i<1; i++) {
    task3();
  }
  unsigned long timeItTook = micros()-bT;
  Serial.print("Duration SerialOutput Job = ");
  Serial.print(timeItTook);
  exit(0);
  */
}
