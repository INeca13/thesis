#include <Adafruit_LiquidCrystal.h>

const int analogInPin2 = A1;
const int button = A0;
unsigned long t;
unsigned long tempo =1000;
unsigned long t2;
unsigned long tempo2 =500;
unsigned long t3;
unsigned long tempo3 =1000;
unsigned long t4;
unsigned long tempo4 =2000;
unsigned long t5;
unsigned long tempo5 = tempo4+1000;
int reading2 = 0;
int val2 = 0;
bool without = false;
bool crono = false;
// the transistor which controls the motor will be attached to digital pin 10
int tempControl = 10;
Adafruit_LiquidCrystal lcd_1(0);

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  t = millis();
  t2 = millis();
  t3 = millis();
  //t4 = millis();
  // make the transistor's pin an output:
  pinMode(tempControl, OUTPUT);
  pinMode(button,INPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
    // initialize the LED pins:
  for (int thisPin = 2; thisPin < 7; thisPin++) {
    pinMode(thisPin, OUTPUT);
  }
  //código para LCD
  lcd_1.begin(16, 2);
  lcd_1.print("Temp. atual");
  
  
}

// the loop routine runs over and over again forever:
void loop() {
  //Os valores de val2 têm um erro esquesito, ver o video 
  //de youtube sobre usar ecras LCD/OLED
  reading2 = analogRead(analogInPin2);
  val2 = map(((reading2/2.5 - 20) * 3.04), 0, 1023, -40, 125);//map(reading2, 61, 998, -40, 125);
  
  //https://capsistema.com.br/index.php/2020/11/09/guia-de-uso-do-sensor-de-temperatura-analogico-tmp36-com-arduino
  float voltage = reading2/2.51 * (5000 / 1024.0);
  // Converta a tensão na temperatura em Celsius:
  float temperature = ((voltage - 500) / 10);
  
  if (millis() - t >= tempo){
    Serial.println("Valor temperatura outra forma");
    Serial.println(temperature);
    //Serial.println(val2);
    t = millis();
    }
  if (temperature <= 50){
    analogWrite(tempControl, 255);
    }
  if ((temperature >= 50) && (temperature < 60)){
    analogWrite(tempControl, 150);
  }
  if ((temperature >= 60) && (temperature < 65)){
    analogWrite(tempControl, 75);
  }
  if (temperature >= 65){
    analogWrite(tempControl, 0);
  }
  if ((temperature >= 60) && (temperature <= 65)){
    digitalWrite(2, HIGH);
    digitalWrite(4, LOW);
  } else {
    digitalWrite(2, LOW);
    digitalWrite(4, HIGH);
  }
  //Código para LCD
  if (millis() - t3 >= tempo3 && !without){
  	lcd_1.setCursor(0, 1);
  	lcd_1.print(temperature);
  	lcd_1.setBacklight(1);
    t3 = millis();
    without = true;
    t2 = millis();
  }
  if (millis() - t2 >= tempo2 && without){
  	lcd_1.setBacklight(0);
    t2 = millis();
    without = false;
  }
  if (digitalRead(A0) == HIGH) {
    t4 = millis();
    t5 = millis();
    crono = true;
    digitalWrite(7,HIGH);
  }
  if (millis() - t4 >= tempo4 && crono){
    tone(8, 523, 100); 
  }
  if (millis() - t5 >= tempo5 && crono){
    crono = false;
    digitalWrite(7,LOW);
  }
}
  
