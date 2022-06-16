#include <Wire.h>
#include "BluetoothSerial.h"


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//Infrared
#define DATA_PIN A0
float IR_WAARDE = 0;
byte IR_data[8] = {};
int i = 0;
//motor pin
int motor_left[] = {18, 5};
int motor_right[] = {2, 15};

//byte to dec
int myVariable = 0;
//Afstand sensor
int echo = 23;
int trig = 19;
long duration;
int distance;
//=============Bluethooth================
char bTComand;
bool manueel = true;
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  // Setup motors
  int i;
  for (i = 0; i < 2; i++) {
    pinMode(motor_left[i], OUTPUT);
    pinMode(motor_right[i], OUTPUT);
  }
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  //=============Bluethooth================
  SerialBT.begin("SS_Robotwagen"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  
  //INFRAED
  pinMode(DATA_PIN, INPUT);

}

void loop() {
	IR_WAARDE = 4800 / (analogRead(DATA_PIN) - 20);

  //=============Bluethooth================
  if (SerialBT.available()) {
    char incomData = SerialBT.read();
    bTComand = incomData;
  }
  switch (bTComand) {
    case 0:
      drive_forward(90);
      break;
    case 1:
      turn_left(90);
      break;
    case 2:
      drive_backward(90);
      break;
    case 3:
      turn_right(90);
      break;
    case 4:
      manueel = !manueel;
      break;
    default:
      motor_stop();
      break;
  }

  if (!manueel) {
    Wire.requestFrom(8, 8);    // request 8 bytes from peripheral device #8
    //Serial.println("IR data");
    while (Wire.available()) { // peripheral may send less than requested
      byte c = Wire.read(); // receive a byte as character
      //Serial.print(c);         // print the character
      IR_data[i] = c;
      i++;
    }
    i = 0;
    for (byte i = 0; i < 8; i = i + 1) {
      Serial.print(IR_data[i]);
    }
    Serial.println("");
    //Binar naar decimaal omzetten
    myVariable = binNaarDec(IR_data);
    Serial.println(myVariable);
    Serial.println("");


    //Afstand van de ultrasoon sensor bepallen
    int d = afstand();
    Serial.print("Afstand: ");
    Serial.println(d);
    //Als afstand klieneer dan 4cm moet de auto stoppen
    if (d < 4) {
      motor_stop();
    }
    //Als afstand kleiner is dan 10 cm moet de auto bij kruisput links of recht gaan
    else if (d < 10) {
      Serial.println("in liks of r");
      //als de IR sensor volledig op de pad is gaat die gewoon recht door
      if (myVariable == 255) {
        Serial.println("RechtDoor af < 10");
        //motor_stop();
        drive_forward(90);
      }
      //Gaat naar linksdraaien
      else if (myVariable >= 48) {
        Serial.println("LINKS af < 10");
        //motor_stop();
        if (myVariable > 240 && myVariable < 250) {
          Serial.println("LINKS af < 10");
          //motor_stop();
          turn_left(255);
          delay(150);
        }
        else {
          turn_left(90);
        }
      }
      //Gaat rechtdoor
      else if (myVariable < 48 && myVariable > 15) {
        //48 voor eerste links
        //15 voor recht
        Serial.println("recht door af < 10");
        //Gaat naar rechtdraaien
        if (myVariable > 25 && myVariable < 35) {
          Serial.println("RECHTS af < 10");
          //motor_stop();
          turn_right(255);
          delay(150);
          //turn_right(255);
        }
        else {
          drive_forward(90);
        }

      }
      //Gaat naar rechtdraaien
      else if (myVariable <= 15) {
        Serial.println("RECHTS af < 10");
        //motor_stop();
        turn_right(90);
      }
      else {
        motor_stop();
      }
    }
    else {
      Serial.println("in normaal");
      if (myVariable == 255) {
        Serial.println("RechtDoor");
        //motor_stop();
        drive_forward(180);
      }
      //Gaat naar linksdraaien
      else if (myVariable >= 48) {
        Serial.println("LINKS");
        //motor_stop();
        turn_left(100);
      }
      //Gaat rechtdoor
      else if (myVariable < 48 && myVariable > 15) {
        //48 voor eerste links
        //15 voor recht
        Serial.println("recht door");
        drive_forward(180);
      }
      //Gaat naar rechtssdraaien
      else if (myVariable <= 15) {
        Serial.println("RECHTS");
        //motor_stop();
        turn_right(100);
      }
      else {
        motor_stop();
      }

    }

  }
  delay(50);
}

//Gaat 8 bit naar decimaal omzetten
unsigned int binNaarDec(byte s[])
{
  int n = 0;
  int i;

  for (i = 0; i < 8; ++i) {
    n <<= 1;
    n += s[i] - 0;
  }

  return n;
}

//motor gaat stoppen

void motor_stop() {
  analogWrite(motor_left[0], LOW);
  analogWrite(motor_left[1], LOW);

  analogWrite(motor_right[0], LOW);
  analogWrite(motor_right[1], LOW);
  delay(25);
}

//wagen gaat achteruit
void drive_backward(int v) {
  analogWrite(motor_left[0], LOW);
  analogWrite(motor_left[1], v);

  analogWrite(motor_right[0], LOW);
  analogWrite(motor_right[1], v);
}
//wagen gaat vooruit
void drive_forward(int v) {

  analogWrite(motor_left[0], v);
  analogWrite(motor_left[1], LOW);

  analogWrite(motor_right[0], v);
  analogWrite(motor_right[1], LOW);
}
//wagen gaat rechts afslaan
void turn_right(int v) {
  analogWrite(motor_left[0], LOW);
  analogWrite(motor_left[1], v);

  analogWrite(motor_right[0], v);
  analogWrite(motor_right[1], LOW);
}
//wagen gaat linksafslaan
void turn_left(int v) {
  analogWrite(motor_left[0], v);
  analogWrite(motor_left[1], LOW);

  analogWrite(motor_right[0], LOW);
  analogWrite(motor_right[1], v);
}

//Gaat de afstand van de Ultrasoon sensor bepallen.

int afstand() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance;
}
