//display

#include <QC2Control.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define ENCODER_CLK 27
#define ENCODER_DT 25
#define ENCODER_SW 23


  QC2Control quickCharge(11, 13);

//extra time to signal good temperatures
bool extra_1 = true;
unsigned long t_extra_1;
unsigned long tempo_extra_1 = 180000;

bool extra_2 = true;
unsigned long t_extra_2;
unsigned long tempo_extra_2 = 180000;

// Variables to store encoder state
int last_clk_state = LOW;
int current_clk_state;
int current_dt_state;
int button_state;
int last_button_state = HIGH;
//int encoder_value = 0;

// Variables to store duty cycle and previous duty cycle
int duty_cycle = 0;
int previous_duty_cycle = 0;

int battery = 52;
int source = 0;


int buttonStartStop = 49;
int buttonReset = 47;
int buttonMinutesUp = 53;
int buttonMinutesDown = 51;
int chamber_1 = 42;
int chamber_2 = 40;

int led1Pin = 37;
int led2Pin = 36;
int led3Pin = 35;
int led4Pin = 34;
int led5Pin = 33;
int led6Pin = 32;

int buzzerPin1 = 50;
int buzzerPin2 = 48;

int hours = 0;
int minutes = 0;
int seconds = 0;
int timerRunning = 0;
int timerPaused = 0;

int hours_2 = 0;
int minutes_2 = 0;
int seconds_2 = 0;
int timerRunning_2 = 0;
int timerPaused_2 = 0;

// check if any button is pressed
unsigned long t_check;
unsigned long tempo_check =200;

// refresh time displayed when running
unsigned long t_refresh;
unsigned long tempo_refresh =1000;

//responsible for making the timer possible
unsigned long t_target;
unsigned long tempo_target = 0;

unsigned long t_restart;
unsigned long tempo_restart = 200;
unsigned long t_stop;
unsigned long tempo_stop = 200;

//will display the inicial screen after reseting or ending time
unsigned long t_reset;
unsigned long tempo_reset = 2000;

int time_running = 0;

//***************************************************************

// refresh time displayed when running
unsigned long t_refresh_2;
unsigned long tempo_refresh_2 = 1000;

//responsible for making the timer possible
unsigned long t_target_2;
unsigned long tempo_target_2 = 0;

unsigned long t_restart_2;
unsigned long tempo_restart_2 = 200;
unsigned long t_stop_2;
unsigned long tempo_stop_2 = 200;

//will display the inicial screen after reseting or ending time
unsigned long t_reset_2;
unsigned long tempo_reset_2 = 2000;

int time_running_2 = 0;

int options = 0; // is responsible for switching what is showed in the screen, as well as what is happening
                  // eg. if the timer is stoped, started, reseted

int chamber = 1;

int ligar = 0;

unsigned long t_encoder;
unsigned long tempo_encoder= 200;


unsigned long t_buzzer_both;
unsigned long t_buzzer_1;
unsigned long t_buzzer_2;
unsigned long tempo_buzzer_both = 300;
int flag_1 = 0;
int flag_2 = 0;
int flag_both = 0;

unsigned long t_back; // call back quando temperatura não é ideal
unsigned long tempo_back = 2000;

//******************************************************

int ThermistorPin = A7;
int ThermistorPin_2 = A0;
int Vo;
int Vo_2;
float R1 = 10000;
float logR2, R2, T, Tc, Tf,T_2,Tc_2,R2_2,logR2_2;
float resistance;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// makinhg an average of N last readings, to make the temperature values more stable
const int numReadings = 10; // number of readings to average                                                                20************************************************
float readings[numReadings]; // array to store the readings
int index = 0; // index of the current reading
float total = 0; // sum of the readings
float average = 0;


float readings_2[numReadings]; // array to store the readings
int index_2 = 0; // index of the current reading
float total_2 = 0; // sum of the readings
float average_2 = 0;

unsigned long t_sensor_1;
unsigned long tempo_sensor_1 =500;
unsigned long t_sensor_2;
unsigned long tempo_sensor_2 =500;

// ligar e desligar sistemas de aquecimento
#define heater_1 9
#define heater_2 7

unsigned long t_ligar_1;
unsigned long tempo_ligar_1 = 2000;
int off_1 = 0;

unsigned long t_ligar_2;
unsigned long tempo_ligar_2 = 2000;
int off_2 = 0;

int power_1 = 0;
int power_2 = 0;
unsigned long teste;
unsigned long tempo_teste = 1000;

void setup() {
  quickCharge.set12V();
  Wire.begin();
  Serial.begin(9600);
  pinMode(buttonStartStop, INPUT_PULLUP);
  pinMode(buttonReset, INPUT_PULLUP);
  pinMode(buttonMinutesUp, INPUT_PULLUP);
  pinMode(buttonMinutesDown, INPUT_PULLUP);
  pinMode(chamber_1, INPUT_PULLUP);
  pinMode(chamber_2, INPUT_PULLUP);
  pinMode(battery,INPUT_PULLUP);


  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
  pinMode(led6Pin, OUTPUT);
  pinMode(buzzerPin1, OUTPUT);
  pinMode(buzzerPin2, OUTPUT);


    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  options = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30,10);
  display.print("TIMER");
  display.display();
  

  // Set encoder pins as inputs with pull-up resistors
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

}

void loop() {
  quickCharge.setVoltage(12);
  timer();
  if (flag_1 == 0 && flag_2 == 0 && flag_both == 0) {
    encoder();
  }
  termistor_1();
  termistor_2(); // uncomment when we have two sensors! a also uncomment the line display.print(average_2); and comment the one bellow it
  heating();

  if (millis() - teste >= tempo_teste){
    Serial.print("heat 1 ---->");
    Serial.println(power_1);
    Serial.print("heat 2 ---->");
    Serial.println(power_2);
    Serial.println(ligar);
    Serial.println(average);
    Serial.println(off_1);
    Serial.println(source);
    
    teste = millis();
  }
}



// FALTA VER SE ELE FAZ A CENA DE DESLIGAR DURANTE 4 SEGUNDOS QUANDO A TEMPERATURA É SUPERIOR A 65.5!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void heating() {
  source = digitalRead(battery);
  if (source == 1) { // quando ligado a corrente, vai funcionar as duas camaras
    if (ligar == 1){
      analogWrite(heater_2, 0);
      if (average < 73 && average >= 50 && off_1 == 0) {
        power_1 = 200;
        analogWrite(heater_1, power_1);
        t_ligar_1 = millis(); 
      }
      if (average < 50 && off_1 == 0) {
        power_1 = 255;
        analogWrite(heater_1, power_1);
        t_ligar_1 = millis();
      } else if (average >= 73) {
        off_1 = 1;
      }
      if (off_1 == 1 && (millis() - t_ligar_1 <= tempo_ligar_1)) {
        power_1 = 0;
        analogWrite(heater_1, power_1);
      } else if (off_1 == 1 && (millis() - t_ligar_1 >= tempo_ligar_1)){
        off_1 = 0;
      }
    }

    if (ligar == 2){
      analogWrite(heater_1, 0);
      if (average_2 < 73 && average_2 >= 50 && off_2 == 0) {
        power_2 = 200;
        analogWrite(heater_2, power_2);
        t_ligar_2 = millis(); 
      }
      if (average_2 < 50 && off_2 == 0) { //(average_2 < 65.5 && off_1 == 0)
      power_2 = 255;
      analogWrite(heater_2, power_2);
      t_ligar_2 = millis();
      } else if (average_2 >= 73) {
        off_2 = 1;
      }
      if (off_2 == 1 && (millis() - t_ligar_2 <= tempo_ligar_2)) {
        power_2 = 0;
        analogWrite(heater_2, power_2);
      } else if (off_2 == 1 && (millis() - t_ligar_2 >= tempo_ligar_2)){
        off_2 = 0;
      }
    }

    if (ligar == 3){
      if (average < 73 && average >= 50 && off_1 == 0) {
        power_1 = 200;
        analogWrite(heater_1, power_1);
        t_ligar_1 = millis(); 
      }
      if (average < 50 && off_1 == 0) {
        power_1 = 255;
      analogWrite(heater_1, power_1);
      t_ligar_1 = millis();
      } else if (average >= 73){
        off_1 = 1;
      }
      if (off_1 == 1 && (millis() - t_ligar_1 <= tempo_ligar_1)) {
        power_1 = 0;
        analogWrite(heater_1, power_1);
      } else if (off_1 == 1 && (millis() - t_ligar_1 >= tempo_ligar_1)){
        off_1 = 0;
      }


      if (average_2 < 73 && average_2 >= 50 && off_2 == 0) {
        power_2 = 200;
        analogWrite(heater_2, power_2);
        t_ligar_2 = millis(); 
      }
      if (average_2 < 50 && off_2 == 0) { //(average_2 < 65.5 && off_1 == 0)
      power_2 = 255;
      analogWrite(heater_2, power_2);
      t_ligar_2 = millis();
      } else if (average_2 >= 73) {
        off_2 = 1;
      }
      if (off_2 == 1 && (millis() - t_ligar_2 <= tempo_ligar_2)) {
        power_2 = 0;
        analogWrite(heater_2, power_2);
      } else if (off_2 == 1 && (millis() - t_ligar_2 >= tempo_ligar_2)){
        off_2 = 0;
      }
    }

    if (ligar == 0) {
      power_1 = 0;
      power_2 = 0;
      analogWrite(heater_1, power_1);
      analogWrite(heater_2, power_2);
    } 
  } else if (source == 0) { //se ligado a bateria, só funcionará uma das câmaras
    if (ligar == 1){
      analogWrite(heater_2, 0);
      if (average < 73 && off_1 == 0) {
        power_1 = 200;
        analogWrite(heater_1, power_1);
        t_ligar_1 = millis(); 
      } else if (average >= 73) {
        off_1 = 1;
      }
      if (off_1 == 1 && (millis() - t_ligar_1 <= tempo_ligar_1)) {
        power_1 = 0;
        analogWrite(heater_1, power_1);
      } else if (off_1 == 1 && (millis() - t_ligar_1 >= tempo_ligar_1)){
        off_1 = 0;
      }
    }

    if (ligar == 2){
      analogWrite(heater_1, 0);
      if (average_2 < 73 && off_2 == 0) {
        power_2 = 200;
        analogWrite(heater_2, power_2);
        t_ligar_2 = millis(); 
      } else if (average_2 >= 73) {
        off_2 = 1;
      }
      if (off_2 == 1 && (millis() - t_ligar_2 <= tempo_ligar_2)) {
        power_2 = 0;
        analogWrite(heater_2, power_2);
      } else if (off_2 == 1 && (millis() - t_ligar_2 >= tempo_ligar_2)){
        off_2 = 0;
      }
    }

    if (ligar == 3){
      if (average < 73 && off_1 == 0) {
        power_1 = 200;
        analogWrite(heater_1, power_1);
        t_ligar_1 = millis(); 
      } else if (average >= 73){
        off_1 = 1;
      }
      if (off_1 == 1 && (millis() - t_ligar_1 <= tempo_ligar_1)) {
        power_1 = 0;
        analogWrite(heater_1, power_1);
      } else if (off_1 == 1 && (millis() - t_ligar_1 >= tempo_ligar_1)){
        off_1 = 0;
      }


      if (average_2 < 73 && off_2 == 0) {
        power_2 = 200;
        analogWrite(heater_2, power_2);
        t_ligar_2 = millis(); 
      } else if (average_2 >= 73) {
        off_2 = 1;
      }
      if (off_2 == 1 && (millis() - t_ligar_2 <= tempo_ligar_2)) {
        power_2 = 0;
        analogWrite(heater_2, power_2);
      } else if (off_2 == 1 && (millis() - t_ligar_2 >= tempo_ligar_2)){
        off_2 = 0;
      }
    }

    if (ligar == 0) {
      power_1 = 0;
      power_2 = 0;
      analogWrite(heater_1, power_1);
      analogWrite(heater_2, power_2);
    }
  }
}


void timer() {
  int c_1 = digitalRead(chamber_1);
  int c_2 = digitalRead(chamber_2);
  int up = digitalRead(buttonMinutesUp);
  int down = digitalRead(buttonMinutesDown);
  int start_stop = digitalRead(buttonStartStop);
  int reset = digitalRead(buttonReset);
  if (millis()-t_check >= tempo_check){ //check if buttons are pressed
    //Serial.println(options);
    
    // it will make sure to display the right information when switching between chambers
    if (c_1 == HIGH){
      chamber = 1;
      menus_1();
    }
    if (c_2 == HIGH){
      chamber = 2; 
      menus_2();
    }

    if (up == HIGH && chamber == 1) {
    // delay(200);
      if (time_running == 0 && minutes < 99){ // if the timer did not start running yet
        minutes++;
        options = 1;
      }
      if (time_running == 2 && tempo_target + minutes*60 < 99*60){ // if it started and was stoped, and if the time does not exceed 99 minutes
        minutes++;
      }
    }
    if (up == HIGH && chamber ==2) {
    // delay(200);
      if (time_running_2 == 0 && minutes_2 < 99){ // if the timer did not start running yet
        minutes_2++;
        options = 1;
      }
      if (time_running_2 == 2 && tempo_target_2 + minutes_2*60 < 99*60){ // if it started and was stoped, and if the time does not exceed 99 minutes
        minutes_2++;
      }
    }

    
    if (down == HIGH && chamber == 1){
    // delay(200);
      if (time_running == 0 && minutes > 0){ // if the timer did not start running yet
        minutes--;
        options = 1;
      }
      if (time_running == 2 && tempo_target + minutes*60 > 60){ // if it started and was stoped, and if the time does not go bellow 0 minutes
        minutes--;
      }
    }
    if (down == HIGH && chamber ==2){
    // delay(200);
      if (time_running_2 == 0 && minutes_2 > 0){ // if the timer did not start running yet
        minutes_2--;
        options = 1;
      }
      if (time_running_2 == 2 && tempo_target_2 + minutes_2*60 > 60){ // if it started and was stoped, and if the time does not go bellow 0 minutes
        minutes_2--;
      }
    }
  

    if (reset == HIGH && chamber == 1 && flag_1 == 0 && flag_2 == 0 && flag_both == 0){ //when pressed, it will reset all the variables and display the inicial screen
      power_1 = 0;
      analogWrite(heater_1, power_1);
      options = 4; // colocar print paused
      tempo_target = 0;
      time_running = 0;
      minutes = 0;
      t_reset = millis();
      //desligar o aquecimento no chamber 1, ou caso nos dois caso o 2 não esteja a ser utilizado
      if (ligar == 3){
        ligar = 2;
      } else if (ligar == 1){
        ligar = 0;
      }
    }
    if (reset == HIGH && chamber == 2 && flag_1 == 0 && flag_2 == 0 && flag_both == 0){ //when pressed, it will reset all the variables and display the inicial screen
      power_2 = 0;
      analogWrite(heater_2, power_2);
      options = 4; // colocar print paused
      tempo_target_2 = 0;
      time_running_2 = 0;
      minutes_2 = 0;
      t_reset_2 = millis();
      //desligar o aquecimento no chamber 2, ou nos dois caso o 1 não esteja a ser usado
      if (ligar == 3){
        ligar = 1;
      } else if (ligar == 2) {
        ligar = 0;
      }
    }
    t_check = millis();
  
  }

  
  if (((millis() - t_target)/1000 >= tempo_target) && time_running == 1 && ((millis() - t_target_2)/1000 >= tempo_target_2) && time_running_2 == 1){ //check if any chamber finished
      if (flag_both == 0){
        t_buzzer_both = millis();
        flag_both = 1;
      }
      digitalWrite(led1Pin, HIGH);
      digitalWrite(led2Pin, HIGH);
      // desligar os dois aquecimentos
      ligar = 0;
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("END in both Chambers");
      display.display();
      power_2 = 0;
      analogWrite(heater_2, power_2);
      power_1 = 0;
      analogWrite(heater_1, power_1);
      if (millis() - t_buzzer_both <= 500){
          digitalWrite(buzzerPin1, HIGH);
          digitalWrite(buzzerPin2, HIGH);
        } else if ((millis() - t_buzzer_both) > 500 && (millis() - t_buzzer_both) <= 1000) {
          digitalWrite(buzzerPin1, LOW);
          digitalWrite(buzzerPin2, LOW);
        } else if ((millis() - t_buzzer_both) > 1000 && (millis() - t_buzzer_both) <= 1500) {
          digitalWrite(buzzerPin1, HIGH);
          digitalWrite(buzzerPin2, HIGH);
        } else if ((millis() - t_buzzer_both) > 1500 && (millis() - t_buzzer_both) <= 2000) {
          digitalWrite(buzzerPin1, LOW);
          digitalWrite(buzzerPin2, LOW);
        } else if ((millis() - t_buzzer_both) > 2000 && (millis() - t_buzzer_both) <= 2500) {
          digitalWrite(buzzerPin1, HIGH);
          digitalWrite(buzzerPin2, HIGH);
        } else if ((millis() - t_buzzer_both) > 2500 ) {
          digitalWrite(buzzerPin1, LOW);
          digitalWrite(buzzerPin2, LOW);
          //flag_both = 0; //uncomment if you want to repeatedly sound the allarm until press button
      }
      
      
      if (start_stop == HIGH){
        tempo_target = 0;
        time_running = 0;
        minutes = 0;
        flag_1 = 0;
        flag_2 = 0;
        flag_both = 0; //comment if you want to repeatedly sound the allarm until press button
        digitalWrite(led1Pin, LOW);
        digitalWrite(buzzerPin1, LOW);
        tempo_target_2 = 0;
        time_running_2 = 0;
        minutes_2 = 0;
        digitalWrite(led2Pin, LOW);
        digitalWrite(buzzerPin2, LOW);
        options = 1;
    }
    } else if (((millis() - t_target_2)/1000 >= tempo_target_2) && time_running_2 == 1){
      if (flag_2 == 0){
        t_buzzer_2 = millis();
        flag_2 = 1;
      }
      digitalWrite(led2Pin, HIGH);
      // se estiverem os dois ligar,s passa so a estar o 1, caso tambem nao esteja, nenhun fica ligado
      if (ligar == 3){
        ligar = 1;
      } else if (ligar == 2) {
        ligar = 0;
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("END in C.2");
      display.display();
      power_2 = 0;
      analogWrite(heater_2, power_2);
      if (millis() - t_buzzer_2 <= 500){
          digitalWrite(buzzerPin2, HIGH);
        } else if ((millis() - t_buzzer_2) > 500 && (millis() - t_buzzer_2) <= 1000) {
          digitalWrite(buzzerPin2, LOW);
        } else if ((millis() - t_buzzer_2) > 1000 && (millis() - t_buzzer_2) <= 1500) {
          digitalWrite(buzzerPin2, HIGH);
        } else if ((millis() - t_buzzer_2) > 1500 && (millis() - t_buzzer_2) <= 2000) {
          digitalWrite(buzzerPin2, LOW);
        } else if ((millis() - t_buzzer_2) > 2000 && (millis() - t_buzzer_2) <= 2500) {
          digitalWrite(buzzerPin2, HIGH);
        } else if ((millis() - t_buzzer_2) > 2500 ) {
          digitalWrite(buzzerPin2, LOW);
          //flag_2 = 0; //uncomment if you want to repeatedly sound the allarm until press button
      }
    if (start_stop == HIGH){
      tempo_target_2 = 0;
      time_running_2 = 0;
      minutes_2 = 0;
      options = 1;
      digitalWrite(led2Pin, LOW);
      digitalWrite(buzzerPin2, LOW);
      menus_1();
      flag_2 = 0; //comment if you want to repeatedly sound the allarm until press button
    }
    } else if ((((millis() - t_target)/1000 >= tempo_target) && time_running == 1)){
      if (flag_1 == 0){
        t_buzzer_1 = millis();
        flag_1 = 1;
      }
      digitalWrite(led1Pin, HIGH);
      // se estiverem os dois ligar, passa so a estar o 2, caso tambem nao esteja, nenhun fica ligado
      if (ligar == 3){
        ligar = 2;
      } else if (ligar == 1) {
        ligar = 0;
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("END in C.1");
      display.display();
      power_1 = 0;
      analogWrite(heater_1, power_1);
      if (millis() - t_buzzer_1 <= 500){
          digitalWrite(buzzerPin1, HIGH);
        } else if ((millis() - t_buzzer_1) > 500 && (millis() - t_buzzer_1) <= 1000) {
          digitalWrite(buzzerPin1, LOW);
        } else if ((millis() - t_buzzer_1) > 1000 && (millis() - t_buzzer_1) <= 1500) {
          digitalWrite(buzzerPin1, HIGH);
        } else if ((millis() - t_buzzer_1) > 1500 && (millis() - t_buzzer_1) <= 2000) {
          digitalWrite(buzzerPin1, LOW);
        } else if ((millis() - t_buzzer_1) > 2000 && (millis() - t_buzzer_1) <= 2500) {
          digitalWrite(buzzerPin1, HIGH);
        } else if ((millis() - t_buzzer_1) > 2500 ) {
          digitalWrite(buzzerPin1, LOW);
          //flag_1 = 0; //uncomment if you want to repeatedly sound the allarm until press button 
      }
      if (start_stop == HIGH){
        tempo_target = 0;
        time_running = 0;
        minutes = 0;
        options = 1;
        flag_1 = 0; //comment if you want to repeatedly sound the allarm until press button
        digitalWrite(led1Pin, LOW);
        digitalWrite(buzzerPin1, LOW);
        menus_2();
      }
  } else{ //display what needs to be displayed

      if (options == 4 && chamber == 1) { // option of time reset and inicial screen
        tempo_target = 0;
        minutes = 0;
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Timer Reseted in C.1");
        display.display();
        if (millis() - t_reset >= tempo_reset){
          options = 1;
        }
      }
      if (options == 4 && chamber ==2) { // option of time reset and inicial screen
        tempo_target_2 = 0;
        minutes_2 = 0;
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Timer Reseted in C.2");
        display.display();
        if (millis() - t_reset_2 >= tempo_reset_2){
          options = 1;
        }
      }



      if (options == 3 && chamber == 1){ // display when the time is paused
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Timer Paused in C.1:");
        display.setCursor(0,10);
        //display.print("Time left: ");
        display.setCursor(0,10);
        display.print((tempo_target + minutes*60)/3600);
        display.print(":");
        display.print(((tempo_target + minutes*60)/60) % 60);
        display.print(":");
        display.print((tempo_target + minutes*60) % 60);
        display.print(" left");
        display.setCursor(0,20);
        //display.print("HH:MM:SS");
        //display.print(" seconds.");
        display.display();
      }
      if (options == 3 && chamber ==2){ // display when the time is paused
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Timer Paused in C.2:");
        display.setCursor(0,10);
        //display.print("Time left: ");
        display.setCursor(0,10);
        display.print((tempo_target_2 + minutes_2*60)/3600);
        display.print(":");
        display.print(((tempo_target_2 + minutes_2*60)/60) % 60);
        display.print(":");
        display.print((tempo_target_2 + minutes_2*60) % 60);
        display.print(" left");
        display.setCursor(0,20);
        //display.print("HH:MM:SS");
        //display.print(" seconds.");
        display.display();
      }

      if (options == 1 && chamber == 1){ // inicial display
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Timer time in C.1: ");
        display.setCursor(0,10);
        display.print(minutes);
        display.print(" Minutes");
        display.setCursor(0,20);
        display.print("T. in C.1 = ");
        display.print(average);
        display.print(" C");
        display.display();
      }
      if (options == 1 && chamber ==2){ // inicial display
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print("Timer time in C.2: ");
        display.setCursor(0,10);
        display.print(minutes_2);
        display.print(" Minutes");
        display.setCursor(0,20);
        display.print("T. in C.2 = ");
        display.print(average_2);
        //display.print(average);
        display.print(" C");
        display.display();
      }


      if (options == 2 && chamber == 1){ //display when the time is running and when it ends
      
      if ((millis() - t_target)/1000 <= tempo_target){
        if (millis() - t_refresh >= tempo_refresh){
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0,0);
          display.print("Time Left in C.1: ");
          display.setCursor(0, 10);
          int t_remaining = tempo_target - int((millis()-t_target)/1000);
          //display.print(t_remaining);
          display.print(t_remaining/3600);
          display.print(":");
          display.print((t_remaining/60) % 60);
          display.print(":");
          display.println(t_remaining % 60);
          display.setCursor(0, 20);
          display.print("Temperature = ");
          display.print(average);
          //display.print(int((millis()-t_target)/1000));
          display.display();
          t_refresh = millis();
          t_reset = millis();
        }//if (tempo_target - (millis()-t_target)/1000 <= 0)
      } else {
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0, 0);
          display.print("END in C.1");
          display.display();
          tempo_target = 0;
          time_running = 0;
          minutes = 0;
          if (millis() - t_reset >= tempo_reset){
            options = 1;
        }
      }
      }
      if (options == 2 && chamber ==2){ //display when the time is running and when it ends
      
      if ((millis() - t_target_2)/1000 <= tempo_target_2){
        if (millis() - t_refresh_2 >= tempo_refresh_2){
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0,0);
          display.print("Time Left in C.2: ");
          display.setCursor(0, 10);
          int t_remaining_2 = tempo_target_2 - int((millis()-t_target_2)/1000);
          //display.print(t_remaining);
          display.print(t_remaining_2/3600);
          display.print(":");
          display.print((t_remaining_2/60) % 60);
          display.print(":");
          display.println(t_remaining_2 % 60);
          display.setCursor(0, 20);
          display.print("Temperature = ");
          display.print(average_2);
          //display.print(int((millis()-t_target)/1000));
          display.display();
          t_refresh_2 = millis();
          t_reset_2 = millis();
        }//if (tempo_target - (millis()-t_target)/1000 <= 0)
      } else {
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0, 0);
          display.print("END in C.2");
          display.display();
          tempo_target_2 = 0;
          time_running_2 = 0;
          minutes_2 = 0;
          if (millis() - t_reset_2 >= tempo_reset_2){
            options = 1;
          }
        }
      }
  }
}


void menus_1() {
    if (time_running == 0){
    options = 1; 
  } else if (time_running == 1){
    options = 2;
  } else {
    options =3;
  }
}

void menus_2(){
    if (time_running_2 == 0){
    options = 1; 
  } else if (time_running_2 == 1){
    options = 2;
  } else {
    options = 3;
  }
}

void encoder() {
  if (millis - t_encoder >= tempo_encoder){
  // Read the current state of the encoder pins
  current_clk_state = digitalRead(ENCODER_CLK);
  current_dt_state = digitalRead(ENCODER_DT);
  button_state = digitalRead(ENCODER_SW);

  //all the possible outcomes after trying to start timer and the temperature is not ideal
  if ((millis() - t_back >= tempo_back) && (millis() - t_back < 2200) && (time_running == 0 || time_running_2 == 0)) {
    options = 1;
  } else if ((millis() - t_back >= tempo_back) && (millis() - t_back < 2200) && (time_running == 1 || time_running_2 == 1)) {
    options = 2;
  } else if ((millis() - t_back >= tempo_back) && (millis() - t_back < 2200) && time_running == 1 && time_running_2 == 0){
    chamber = 1;
    options = 1;
  } else if ((millis() - t_back >= tempo_back) && (millis() - t_back < 2200) && time_running == 0 && time_running_2 == 1) {
    chamber = 2;
    options = 1;
  } else if ((millis() - t_back >= tempo_back) && (millis() - t_back < 2200) && time_running == 2 && time_running_2 == 0) {
    chamber = 1;
    options = 3;
  } else if ((millis() - t_back >= tempo_back) && (millis() - t_back < 2200) && time_running == 0 && time_running_2 == 2) {
    chamber = 2;
    options = 3;
  }
  

  // If the encoder button is pressed
  if (button_state == LOW && last_button_state == HIGH && chamber == 1) {

    if (ligar == 2) {
      ligar = 3;
    } else if (ligar == 1 || ligar == 0) {
      ligar = 1;
    }

    if (time_running == 1 && (millis() - t_stop >= tempo_stop)){ //if the timer was running, the time will stop, enabling the possibility to add or reduce time
      options = 3; // colocar print paused
      tempo_target = tempo_target - int((millis()-t_target)/1000); // updates the time it needs to run 
      time_running = 2;
      minutes = 0;  
      t_restart = millis();
    }


    if (time_running == 0 && average >= 71){ // after chosing the time, the first press will just start the timer
      time_running = 1;
      options = 2;
      t_target = millis();
      tempo_target = minutes*60; // will convert the time chosen to seconds
      extra_1 = true;
      t_extra_1 = millis() - 180000;
    } else if (time_running == 0 && average < 71) {
      extra_1 = false;
      options = 10;
      t_back = millis();
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("Heater in C.1 on.");
      display.setCursor(0, 10);
      display.println("Wait for temperature to rise !");
      display.display();
    }

    if (time_running == 2 && (millis() - t_restart >= tempo_restart)){ //makes possible to restart the timer after pausing it
      time_running = 1;
      tempo_target = tempo_target + minutes*60; // will update the time to run, if the user wants to reduce the time, he can simply press the buttons up and down 
      options = 2;                              // in order to change the targetted time
      t_target = millis();
      t_stop = millis();
    }
  }

  if (button_state == LOW && last_button_state == HIGH && chamber == 2) {

    if (ligar == 1) {
      ligar = 3;
    } else if (ligar == 2 || ligar == 0) {
      ligar = 2;
    }

    if (time_running_2 == 1 && (millis() - t_stop_2 >= tempo_stop_2)){ //if the timer was running, the time will stop, enabling the possibility to add or reduce time
      options = 3; // colocar print paused
      tempo_target_2 = tempo_target_2 - int((millis()-t_target_2)/1000); // updates the time it needs to run 
      time_running_2 = 2;
      minutes_2 = 0;  
      t_restart_2 = millis();
    }

    if (time_running_2 == 0 && average_2 >= 71){ // after chosing the time, the first press will just start the timer
      time_running_2 = 1;
      options = 2;
      t_target_2 = millis();
      tempo_target_2 = minutes_2*60; // will convert the time chosen to seconds
      extra_2 = true;
      t_extra_2 = millis() - 180000;
    } else if (time_running_2 == 0 && average_2 < 71){
      extra_2 = false;
      options = 10;
      t_back = millis();
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("Heater in C.2 on.");
      display.setCursor(0, 10);
      display.println("Wait for temperature to rise !");
      display.display();
    }

    if (time_running_2 == 2 && (millis() - t_restart_2 >= tempo_restart_2)){ //makes possible to restart the timer after pausing it
      time_running_2 = 1;
      tempo_target_2 = tempo_target_2 + minutes_2*60; // will update the time to run, if the user wants to reduce the time, he can simply press the buttons up and down 
      options = 2;                              // in order to change the targetted time
      t_target_2 = millis();
      t_stop_2 = millis();
    }
  }

  // If the encoder knob is turned, adjust the duty cycle value
  if (current_clk_state != last_clk_state && chamber ==1) {
    if (current_dt_state != current_clk_state) {
      if (time_running == 0 && minutes < 99){ // if the timer did not start running yet
        minutes++;
        options = 1;
      }
      if (time_running == 2 && tempo_target + minutes*60 < 99*60){ // if it started and was stoped, and if the time does not exceed 99 minutes
        minutes++;
      }
    } else {
      if (time_running == 0 && minutes > 0){ // if the timer did not start running yet
        minutes--;
        options = 1;
      }
      if (time_running == 2 && tempo_target + minutes*60 > 60){ // if it started and was stoped, and if the time does not go bellow 0 minutes
        minutes--;
      }
    }
  }

  if (current_clk_state != last_clk_state && chamber ==2) {
    if (current_dt_state != current_clk_state) {
      if (time_running_2 == 0 && minutes_2 < 99){ // if the timer did not start running yet
        minutes_2++;
        options = 1;
      }
      if (time_running_2 == 2 && tempo_target_2 + minutes_2*60 < 99*60){ // if it started and was stoped, and if the time does not exceed 99 minutes
        minutes_2++;
      }
    } else {
      if (time_running_2 == 0 && minutes_2 > 0){ // if the timer did not start running yet
        minutes_2--;
        options = 1;
      }
      if (time_running_2 == 2 && tempo_target_2 + minutes_2*60 > 60){ // if it started and was stoped, and if the time does not go bellow 0 minutes
        minutes_2--;
      }
    }
  }



  // Save the current encoder state for the next loop
  last_clk_state = current_clk_state;
  last_button_state = button_state;
  
  }
}


void termistor_1() {
    if (millis() - t_sensor_1 >= tempo_sensor_1){
        Vo = analogRead(ThermistorPin);
        R2 = R1 * (1023.0 / (float)Vo - 1.0);
        logR2 = log(R2);
        T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
        Tc = T - 273.15;
        //Tf = (Tc * 9.0)/ 5.0 + 32.0; 
        //Serial.print("Temperature: "); 
        //Serial.print(Tf);
        //Serial.print(" V1 ---> ");
        //Serial.println(Vo);
        //Serial.println(" C");   
        t_sensor_1 = millis();
        // subtract the oldest reading from the total
        total = total - readings[index];
        // add the new reading to the total
        readings[index] = Tc;
        total = total + readings[index];
        // move to the next index
        index = (index + 1) % numReadings;
        // calculate the average
        average = total / numReadings;


        if (average >= 72 && extra_1 == false){
          t_extra_1 = millis();
          extra_1 = true;
          
        }

        // print the average~
        if (ligar == 1 || ligar == 3) {
          if ((millis() - t_extra_1 >= tempo_extra_1) && extra_1 == true){   //option of only putting the time, since the temperature readings can be oscilating because of the socket power
            digitalWrite(led3Pin, HIGH);
            digitalWrite(led4Pin, LOW);
          } else {    //option of only putting the time, since the temperature readings can be oscilating because of the socket power
            digitalWrite(led3Pin, LOW);
            digitalWrite(led4Pin, HIGH);
          }
        } else if (ligar == 2 || ligar == 0){
          digitalWrite(led3Pin, LOW);
          digitalWrite(led4Pin, LOW);
        }

    }
}

void termistor_2() {
    if (millis() - t_sensor_2 >= tempo_sensor_2){
        Vo_2 = analogRead(ThermistorPin_2); //analogRead(ThermistorPin_2);*********************************************************************
        R2_2 = R1 * (1023.0 / (float)Vo_2 - 1.0);
        logR2_2 = log(R2_2);
        T_2 = (1.0 / (c1 + c2*logR2_2 + c3*logR2_2*logR2_2*logR2_2));
        Tc_2 = T_2 - 273.15;
        //Tf = (Tc * 9.0)/ 5.0 + 32.0; 
        //Serial.print("Temperature: "); 
        //Serial.print(Tf);
        //Serial.print(" V2 ---> ");
        //Serial.println(Vo_2);
        //Serial.println(" C");   
        t_sensor_2 = millis();
        // subtract the oldest reading from the total
        total_2 = total_2 - readings_2[index_2];
        // add the new reading to the total
        readings_2[index_2] = Tc_2;
        total_2 = total_2 + readings_2[index_2];
        // move to the next index
        index_2 = (index_2+ 1) % numReadings;
        // calculate the average
        average_2 = total_2 / numReadings;
        // print the average
        //if (average_2> 30){
          average_2 = average;//***************************************

        if (average_2 >= 72 && extra_2 == false){
          t_extra_2 = millis();
          extra_2 = true;
          
        }

          if (ligar == 2 || ligar == 3) {
            if ((millis() - t_extra_2 >= tempo_extra_2) && extra_2 == true){  //option of only putting the time, since the temperature readings can be oscilating because of the socket power
            digitalWrite(led5Pin, HIGH);
            digitalWrite(led6Pin, LOW);
          } else {      //option of only putting the time, since the temperature readings can be oscilating because of the socket power
            digitalWrite(led5Pin, LOW);
            digitalWrite(led6Pin, HIGH);
          }
          } else if (ligar == 1 || ligar == 0){
            digitalWrite(led6Pin, LOW);
            digitalWrite(led5Pin, LOW);
          }

    }
}
