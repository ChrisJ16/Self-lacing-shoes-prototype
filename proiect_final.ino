#include <Servo.h>
#include <TimerOne.h>
Servo myservo;

#define D2 2
#define D8 8


float lightSensorThreshold = 100;
float hallSensorThreshold = 100;

bool end1 = false;
bool end2 = false;


float compute_stable_average(int n, int pin) {
  const int numSamples = 100;
  float values[numSamples];
  float average = 0;
  int i = 0;

  while (true) {
    values[i % numSamples] = analogRead(pin);
    average = (average * i + values[i % numSamples]) / (i + 1);

    bool isStable = true;
    for (int j = 0; j < numSamples; j++) {
      if (abs(values[j] - average) > n) {
        isStable = false;
        break;
      }
    }
    if (isStable) {
      break;
    }
    i++;
  }
  return average;
}


void setup() {
  // put your setup code here, to run once:
  analogReference(DEFAULT);
  
  // configuram porturile de intrare pentru semnalele analogice
  pinMode(A0, INPUT); // hall
  pinMode(A5, INPUT); // lumina
  pinMode(D2, INPUT);
  
  // resetam servo-motorul
  myservo.attach(9); // atasam motorul servo la pinul 9
  myservo.write(90);
  myservo.write(180);

  //activam timer
  //Timer1.initialize(10000000);
  //Timer1.attachInterrupt(verifiyThresholds);

  //initializam pragurile
  lightSensorThreshold = compute_stable_average(10, A5);
  hallSensorThreshold = compute_stable_average(100, A1);  
  Serial.begin(9600);
}

void verifiyThresholds(void){
    
}

bool hasLightValueChanged(int analogPin, int threshold, int margin) {
  static bool isChangedLight = false;
  int currentValue = analogRead(analogPin);

  if (currentValue < threshold - margin && !isChangedLight) 
  {
    Serial.println("Once Light");
    isChangedLight = true;
    return true;
  } else 
    return false;
}

bool hasMagneticFieldBeenDetected(int analogValue, int digitalVal, int threshold, int margin)
{
  static bool isChangedHall = false;

  if (analogValue < threshold - margin && digitalVal == 0 && !isChangedHall) 
  {
    Serial.println("Once Hall");
    isChangedHall = true;
    return true;
  } else
    return false;
}

void loop() {
  
  if(!end1 || !end2){
    int hallVal1 = analogRead(A0); // valoarea analogica primita de la senzorul ce detecteaza efectul Hall
    int hallVal2 = digitalRead(D2); // valoarea digitala
    int lightVal = analogRead(A5); // valoare analog citita de la senzorul de lumina

    if (hasLightValueChanged(A5, lightSensorThreshold, 50)) 
    {
      // Valoarea citita de la A0 s-a schimbat drastic, deci inseamna ca vom incepe procesul de legare a pantofului
      end1 = true;
      myservo.write(-180);
      delay(1000);
    }

    if(hasMagneticFieldBeenDetected(hallVal1, hallVal2, hallSensorThreshold, 50)){
        // dezlegam siretul daca detectam efectul hall generat de campul magnetic al unui magnet
      end2 = true;
      myservo.write(180); 
    }
  }
  else if(end1 && end2){
    //Serial.println("Papucii nu mai sunt purtati! Sleep mode...");
    //Aici se reseteaza tot
    //end1 = true;
    //end2 = true;
  }
}
