/*
  Created by TEOS
  Domotic with Arduino https://goo.gl/btZpjs
  YouTube https://goo.gl/k7TUSZ
  Instagram https://goo.gl/zEIjia
  Facebook https://goo.gl/eivraR
  Twitter https://goo.gl/du5Wgn
  Github https://goo.gl/Xl5IiS
  WEB https://www.proyecto-teos.com
*/

/*
  todo el credito es para la persona de arriba 
  lo unico que yo le anexe a este codigo seria la parte del ethernet shield
*/

#include <Password.h>
#include <Keypad.h>     
#include<Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal_I2C.h>

// replace the MAC address below by the MAC address printed on a sticker on the Arduino Shield 2
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

int    HTTP_PORT   = 8000;
String HTTP_METHOD = "PUT";
char   HOST_NAME[] = "ip de tu compu";
String PATH_NAME   = "/alarma/alarma/1/";


LiquidCrystal_I2C lcd(0x27,16,2);

const uint16_t delayFunctionT = 1000, intervalT = 400, delayLedT = 50;
unsigned long previousMillisT;


uint8_t passPositionT = 0, speakerStatusT = LOW, ledStatusT = LOW;
bool alarmStatusT = false, alarmActiveT = false, motionDetectionT = false;


const uint8_t FILAS = 4;     // define numero de filas
const uint8_t COLUMNAS = 3;    // define numero de columnas
const char keys[FILAS][COLUMNAS] = {    // define la distribucion de teclas
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

uint8_t pinesFilas[FILAS] = {9,8,6};   // pines correspondientes a las filas
uint8_t pinesColumnas[COLUMNAS] = {5}; // pines correspondientes a las columnas
//char CLAVE[5] ="1111";
Keypad keypad = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);  // crea objeto

Password passwordT = Password("11111");

//---------------------------
uint8_t ledRed= 2;
uint8_t ledGreen = 7;
uint8_t pirPin= 3;
uint8_t buzzerPin=4;



  
void setup()
{
  
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  pinMode(pirPin, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledRed,HIGH);
 
  lcd.setCursor(0, 0);
  lcd.print("Sistema de");
  lcd.setCursor(0, 1);
  lcd.print("seguridad");
  delay(3000);
  previousMillisT = millis();
  keypad.addEventListener(keypadEventT);
  blockingMessageT(); 

  // initialize the Ethernet shield using DHCP:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtaining an IP address using DHCP");
    while(true);
  }
}

void loop(){
  
 //If the alarmActiveT variable is in a high state and the pirPinT detects movement,
  //it will call the motionDetectedT function. The motionDetectionT function triggers
  //the buzzer's sonar.
  
  keypad.getKey();
  if (alarmActiveT == true && digitalRead(pirPin) == HIGH){
      motionDetectedT();
  }
  if (motionDetectionT){
     digitalWrite(buzzerPin,speakerAlarmT()); 
  }
}

void keypadEventT(KeypadEvent keyT) {
  switch (keypad.getState()) {
    //When typing "#" or entering more than 4 characters the function will be
    //called checkPasswordT.
    case PRESSED:
      if (passPositionT >= 5)
        checkPasswordT();
      lcd.setCursor((passPositionT++), 2);
      switch (keyT) {
        case '#':
          checkPasswordT();
          break;
        default:
          passwordT.append(keyT);
          lcd.print("*");
      }
  }
}


//If the password is correct and the alarms are deactivated, then it will call the
//blockedSystemT function, if the password is correct and the alarms are activated,
//then it will call the unlockSystemT function, contrary to all this it will be called
//the incorrectPasswordT function, which is when introduced an incorrect password.
void checkPasswordT() {
   
  if (passwordT.evaluate()) {
    if (alarmActiveT == false && alarmStatusT == false)
      blockedSystemT();
    else if (alarmActiveT == true || alarmStatusT == true)
      unlockSystemT();
  }
  else
    incorrectPasswordT();
}

//Function that indicates that an incorrect password has been entered.
void incorrectPasswordT() {
  passwordT.reset();
  passPositionT = 0;
  digitalWrite(ledRed, LOW);
  //An incorrect password message is sent.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Clave incorrecta");
  //Using the ternary operator will turn on or off a led.
  for (uint8_t iT = 1; iT <= 8; iT++) {
    ledStatusT = (ledStatusT) ? LOW : HIGH;
    digitalWrite(ledGreen, ledStatusT);
    delay(delayLedT);
  }
  //The following conditions will serve to return the state of the LED.
  if (alarmActiveT == false && alarmStatusT == false) {
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
    delay(delayFunctionT);
    blockingMessageT();
  }
  else if (alarmActiveT == true || alarmStatusT == true) {
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
    delay(delayFunctionT);
    unlockMessageT();
  }
}

//Function that shows motion detection.
void motionDetectedT() {
  //Activate the alarms and restart the password.
  motionDetectionT = true;
  alarmStatusT = true;
  
  passwordT.reset();
  passPositionT = 0;
  //Send a message that movement has been detected, also indicated by LEDs.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Movimiento");
  lcd.setCursor(0,1);
  lcd.print("detectado");
  for (uint8_t iT = 1; iT <= 8; iT++) {
    ledStatusT = (ledStatusT) ? LOW : HIGH;
    digitalWrite(ledGreen, ledStatusT);
    delay(delayLedT);
  }
  for (uint8_t iT = 1; iT <= 8; iT++) {
    ledStatusT = (ledStatusT) ? LOW : HIGH;
    digitalWrite(ledRed, ledStatusT);
    delay(delayLedT);
  }
  digitalWrite(ledGreen, HIGH);
  digitalWrite(ledRed, LOW);
  delay(delayFunctionT);
  unlockMessageT();
}

//Function that blocks the system.
void blockedSystemT() {
  passwordT.reset();
  passPositionT = 0;
  digitalWrite(ledGreen, HIGH);
  digitalWrite(ledRed, LOW);
  //10 seconds of tolerance are given for the system to activate.
  for (uint8_t iT = 10; iT > 0; iT--) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("activando ");
    lcd.print(iT);
    lcd.setCursor(0, 1);
    lcd.print("segundos");
    delay(delayFunctionT);
  }

  alarmActiveT = true;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Activo");
  delay(delayFunctionT);
  unlockMessageT();
}

//Function that unlocks the system.
void unlockSystemT() {
  //The alarm states change to a low state.
  alarmStatusT = false;
  alarmActiveT = false;
   
  motionDetectionT = false;
  passwordT.reset();
  passPositionT = 0;
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, HIGH);
  digitalWrite(buzzerPin,LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema");
  lcd.setCursor(0,1);
  lcd.print("desactivado");
  delay(delayFunctionT);
  blockingMessageT();
}

//The next two functions show messages on the screen.
void blockingMessageT() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("clave para");
  lcd.setCursor(0, 1);
  lcd.print("activar");
  httpPut(alarmActiveT);
}

void unlockMessageT() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("clave para");
  lcd.setCursor(0, 1);
  lcd.print("desactivar");
  httpPut(alarmActiveT);
}

//This function returns a high or low state that is assigned to the buzzer.
uint8_t speakerAlarmT() {
  //Using the millis function we can execute functions in "background".
  unsigned long currentMillisT = millis();
  if ((unsigned long)(currentMillisT - previousMillisT) >= intervalT) {
    previousMillisT = millis();
    return speakerStatusT = (speakerStatusT) ? LOW : HIGH;
  }
} 

void httpPut(bool estado){
String JSON;

  if(estado){
    JSON="{\"idAlarma\":\"1\",\"estadoAlarma\":\"true\"}";
  }
  else
    JSON="{\"idAlarma\":\"1\",\"estadoAlarma\":\"false\"}";
  // connect to web server on port 80:
  if(client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(JSON.length());
    client.println();
    client.println(JSON);
    client.println(); // end HTTP header

    // send HTTP body
   // client.println(queryString);

    while(client.connected()) {
      if(client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }
}
