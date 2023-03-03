//#include "B31DGMonitor.h"

#define T1_LED 19
#define T2_INPUT 7
#define T3_INPUT 2
#define T4_INPUT 1
#define T4_LED 0

int T2_Freq;
int T3_Freq;
int counter = 0;
double reading1 = 0;
double reading2 = 0;
double reading3 = 0;
double reading4 = 0;
double read_average;

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

  //monitor.startMonitoring(); // all the tasks should be released after this, this time can be queried using getTimeStart()
}

void task1() {
   //monitor.jobStarted(1);  // i between [1..5], i.e. 1 is digital output, 5 is serial print
   //Perform task1 - Signal Pulse
   digitalWrite(T1_LED, HIGH);
   delayMicroseconds(200);
   digitalWrite(T1_LED, LOW);
   delayMicroseconds(50);
   digitalWrite(T1_LED, HIGH);
   delayMicroseconds(30);
   digitalWrite(T1_LED, LOW);
   //monitor.jobEnded(1);
}

void task2() {
   //monitor.taskStart(2);
   //Perform task2 - Measure Frequency
   int Freq_Input_High = pulseIn(T2_INPUT, HIGH);
   int Freq_Input_Low = pulseIn(T2_INPUT, LOW);
   T2_Freq = 1 / (Freq_Input_High + Freq_Input_Low);
   //monitor.jobEnded(2);
}

void task3() {
   //monitor.taskStart(3);
   //Perform task3 - Measure Frequency
   int Freq_Input_High = pulseIn(T3_INPUT, HIGH);
   int Freq_Input_Low = pulseIn(T3_INPUT, LOW);
   T3_Freq = 1 / (Freq_Input_High + Freq_Input_Low);
   //monitor.jobEnded(3);
}

void task4() {
   //monitor.taskStart(4);
   //Perform task4 - Measure Potentiometer Signal
   if (counter == 4)  //Reset counter after 4 readings
   {
     counter = 0;
   }

   if (counter == 0)
   {
     reading1 = analogRead(T4_INPUT);  //Read Analog Signal
   }
   else if (counter == 1)
  {
    reading2 = analogRead(T4_INPUT);  //Read Analog Signal
  }
    else if (counter == 2)
  {
    reading3 = analogRead(T4_INPUT);  //Read Analog Signal
  }
    else if (counter == 3)
  {
    reading4 = analogRead(T4_INPUT);  //Read Analog Signal
  }
   read_average = (reading1 + reading2 + reading3 + reading4) / 4;  //Calculate average

   if (read_average > 2047)
   {
     digitalWrite(T4_LED, HIGH);
   }
   else
   {
     digitalWrite(T4_LED, LOW);
   }

   counter = counter + 1;
   //monitor.jobEnded(4);
}

void task5() {
   //monitor.taskStart(5);
   //Perform task5 - Display task2 and task3 frequencies
  if(T2_Freq < 333)
  {
  T2_Freq = 333;
  }
  else if(T2_Freq > 1000)
  {
    T2_Freq = 1000;
  }
  if(T3_Freq < 500)
  {
    T3_Freq = 500;
  }
  else if(T3_Freq > 1000)
  {
    T3_Freq = 1000; 
  }
  T2_Freq = (T2_Freq - 333)/(1000-333)*(99 - 0);  //Scaling to between 0 - 99
  T3_Freq = (T3_Freq - 500)/(1000-500)*(99 - 0);   //Scaling to between 0 - 99
  Serial.printf("%d, %d \n", T2_Freq, T3_Freq); // print T2 and T3 Frequencies
   //monitor.jobEnded(5);
}

// simple example of cyclic executive (version not using Ticker)
void loop(void) 
{
   // this cyclic executive executes taskA and then taskB in each new frame
   task1();
   task2();

   delayMicroseconds(100); 
}
