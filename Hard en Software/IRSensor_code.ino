#include <Wire.h>

int dPin [8] = {2, 3, 4, 5, 6, 7, 8, 9}; //8 pinnen van de IR Sensor
int data[8] = {}; //Data variable die een arry is voor 8bit
int ir = A0; //IR enable pin
float waarde = 0;
void setup() {
  Wire.begin(8);//I2C opzetten als slave met een addres van 8
  Wire.onRequest(requestEvent);//data sturen als de master vraage met die requestEvent functie
  Serial.begin(115200);
  for (byte i = 0; i < 8; i = i + 1) {//Alle pinnnen van de IR senor als input
    pinMode(dPin[i], INPUT);
  }
  pinMode(ir, OUTPUT);
  digitalWrite(ir, HIGH); //IR pin op hoog zetten
}

void loop() {
	//Gaat de IR sensor waarde lezen en dat opslaan in de data variable
  for (byte i = 0; i < 8; i = i + 1) {
    data[i] = digitalRead(dPin[i]);
  }
  //Gaat dat printen om de zien
  for (byte i = 0; i < 8; i = i + 1) {
    Serial.print(data[i]);
  }
  Serial.println("");
  memset(data, 0, sizeof(data));
}

void requestEvent() {
	//Gaat de data variable sturen
  for (byte i = 0; i < 8; i = i + 1) {
    Wire.write(data[i]);
  }
}
