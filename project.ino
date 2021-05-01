#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

SoftwareSerial gsm(7, 6); //SIM900 Tx & Rx is connected to Arduino #7 & #6

LiquidCrystal lcd(13, 12, 8, 9, 10, 11);
#define good 550
#define maxm 750
#define MQ3pin A0
#define relay 4//in1
#define buzzer 3
#define gled 2

String incomingData;   
String message = "";
float sensorValue;
float newSenVal;
int SMSSent=0;

void receive_message()
{
  if (gsm.available() > 0)
  {
    incomingData = gsm.readString(); // Get the data from the serial port.
    Serial.print(incomingData); 
    delay(10); 
  }
}

void send_message(String message)
{
  gsm.println("AT+CMGF=1");    //Set the GSM Module in Text Mode
  delay(100);  
  gsm.println("AT+CMGS=\"+919866654507\""); // Replace it with your mobile number
  delay(100);
  gsm.println(message);   // The SMS text you want to send
  delay(100);
  gsm.println((char)26);  // ASCII code of CTRL+Z
  delay(100);
  gsm.println();
  delay(1000);  
}



boolean chgakg(){
  newSenVal=analogRead(MQ3pin);
  if(newSenVal != sensorValue){
    return true;
    sensorValue=newSenVal;
    }
  return false;
}

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(relay, OUTPUT);//in1
  digitalWrite(relay, HIGH);
  gsm.print("AT+CMGF=1\r");  
  delay(100);
  gsm.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
  sensorValue = analogRead(MQ3pin);

  
  if (sensorValue < good) {
    message = "Status: Not at all drunk. Highly recommend to start. Alcohol level:"+ String(sensorValue) +"ppm";
    send_message(message);
    lcd.setCursor(0, 0);
    lcd.print("SOBER. Good to start");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    lcd.print("Alcohol level: "); // Prints Sensor Val: to LCD
    lcd.print(analogRead(MQ3pin));
  } else if (sensorValue >= good && sensorValue < maxm) {
    message = "Status: Drunk within legal limits. Medium recommendation to start. Alcohol level:"+ String(sensorValue) +"ppm";
    send_message(message);
    lcd.setCursor(0, 0);
    lcd.print("Limitedly drunk.");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    lcd.print("Alcohol level: "); // Prints Sensor Val: to LCD
    lcd.print(analogRead(MQ3pin));
  } else {
    message = "Status:Drunk. No recommendation to start. Alcohol level:"+ String(sensorValue) +"ppm";
    send_message(message);
    lcd.setCursor(0, 0);
    lcd.print("Highly drunk. Leave vehicle");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    lcd.print("Alcohol level: "); // Prints Sensor Val: to LCD
    lcd.print(analogRead(MQ3pin));
  }
}

void loop() {
  delay(250);
  lcd.scrollDisplayLeft();

  receive_message();
  if(incomingData.indexOf("Start")>=0)
  {
    digitalWrite(relay, LOW);
    digitalWrite(gled, HIGH);
    digitalWrite(buzzer, LOW);
    message = "Supply is ON";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Supply is ON");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    lcd.print("Alcohol level: "); // Prints Sensor Val: to LCD
    lcd.print(analogRead(MQ3pin));
    send_message(message);
    SMSSent = 0;
  }
  if(incomingData.indexOf("Stop")>=0)
  {
    digitalWrite(relay, HIGH);
    digitalWrite(buzzer, HIGH);
    digitalWrite(gled, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Owner Stopped supply");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    lcd.print("Contact owner"); // Prints Sensor Val: to LCD
    message = "Supply is OFF";
    send_message(message);
  }        
 
  if (chgakg()){
    if (newSenVal >= maxm){
     message = "Status: Alcohol beyond limit detected. Supply is cut off. Alcohol level:"+ String(newSenVal) +"ppm";
     if(SMSSent == 0){
       send_message(message);
       SMSSent = 1;
     }
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Alcohol limt exceeded. Supply lost.");
     lcd.setCursor(0, 1); // set the cursor to column 0, line 2
     lcd.print("Alcohol level: "); // Prints Sensor Val: to LCD
     lcd.print(analogRead(MQ3pin));
     digitalWrite(buzzer, HIGH);
     digitalWrite(gled, LOW);
     digitalWrite(relay,HIGH);
    
   }
  } 
 }
