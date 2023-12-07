// TODO: Add one more ultrasonic sensor

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "DFRobotDFPlayerMini.h"

TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(D1, D2); // The serial connection to the GPS device
//SoftwareSerial mySoftwareSerial(D1, D2); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701
#define DFPLAY        1

const char* ssid = "TPLink";
const char* password = "Excitel@103";

//const char* ssid = "project";
//const char* password = "project007";

const int echoPin1 = D3;
const int trigPin1 = D4;

const int echoPin2 = D5;
const int trigPin2 = D6;

const int echoPin3 = D7;
const int trigPin3 = D8;

const int gpsSwitch = D0;

//#define BOTtoken "519073934:AAHD2MNwbqRg5m-pjDl6COl07zRL6n2Vy9E"  // your Bot Token (Get from Botfather)
#define BOTtoken "6112344947:AAH-7y3PnjKPHiiWWVW1mWRFt9WdKV44_mM"  // your Bot Token (Get from Botfather)
//6079607178:AAFlVFErf59AYGDrshzdBbIPlsgiFOFHEzo

#define CHAT_ID "748459272"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
int pm;
unsigned int distance1,distance2,distance3;
unsigned long myTime=0, myTime1=0; 
long duration;
float distanceCm1,distanceCm2,distanceCm3,distanceCm4;
float distanceInch;
String coord="";
String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
int incomingdata=0;
int rcvdata=0;


WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
//  mySoftwareSerial.begin(9600);  
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org  
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
//  pinMode(gpsSwitch, INPUT); 
  pinMode(gpsSwitch, INPUT_PULLUP);  
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin3, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin3, INPUT); // Sets the echoPin as an Input
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "Application Started", "");  
  delay(4000);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  Serial.println(F("DFPlayer Mini online."));
//   if(DFPLAY)
//     dfplay(2);

}
void loop()
{
  if (stringComplete) {
    Serial.println(inputString);
    incomingdata = inputString.toInt();
    Serial.print("incomingdata: ");
    Serial.println(incomingdata);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        coord = String(lat_str) + String(",") + String(lng_str);
      }
      else
      {
        coord = String("No Valid GPS Data");
        Serial.println(coord);
      }
      if (gps.date.isValid())
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date);
        date_str += " / ";
        if (month < 10)
          date_str += '0';
        date_str += String(month);
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }
      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;
        if (hour >= 12)
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour);
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute);
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second);
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }
    }
    
//  Serial.println("<< Ultrasonic ");

#if 1
  if(millis() - myTime > 1000)
  {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);  
  duration = pulseIn(echoPin1, HIGH);  
  distanceCm1 = duration * SOUND_VELOCITY/2;

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);  
  duration = pulseIn(echoPin2, HIGH);  
  distanceCm2 = duration * SOUND_VELOCITY/2;
      
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);  
  duration = pulseIn(echoPin3, HIGH);  
  distanceCm3 = duration * SOUND_VELOCITY/2;
  
//  Serial.print("D1: ");
//  Serial.print(distanceCm1);
  Serial.print("  D2: ");
  Serial.print(distanceCm2);
  Serial.print("  D3: ");
  Serial.print(distanceCm3);
  Serial.print("  ");

  Serial.println(time_str);

  Serial.print("digitalRead(gpsSwitch): ");
  Serial.println(digitalRead(gpsSwitch));

//  analogRead(A0);
Serial.println(analogRead(A0));
  if(analogRead(A0) > 1000)
//  if(digitalRead(gpsSwitch)==0)
  {    
    String dat="";
    dat = coord + ", SOS Pressed";
    bot.sendMessage(CHAT_ID,dat , ""); 
  }
  
  if(distanceCm2<10 || distanceCm3<10 || incomingdata > 0)
//  if(distanceCm1<10 || distanceCm2<10 || distanceCm3<10 || incomingdata > 0)
  {
    if(distanceCm1 < 10)
    {
//      String dc1="";
      if(DFPLAY)
        dfplay(55);
//      dc1 = coord + ", Obstacle in the front";
//      bot.sendMessage(CHAT_ID,dc1 , ""); 
//      dc1 = "";     
    }
    if(distanceCm2 < 10)
    {
//      String dc2="";
      if(DFPLAY)
        dfplay(5);
//      dc2 = coord + ", Obstacle in the left";
//      bot.sendMessage(CHAT_ID,dc2 , ""); 
//      dc2 = "";     
    }
    if(distanceCm3 < 10)
    {
//      String dc3="";
      if(DFPLAY)      
        dfplay(15);
//      dc3 = coord + ", Obstacle in the right";
//      bot.sendMessage(CHAT_ID,dc3 , ""); 
//      dc3 = "";     
    }
    if(incomingdata > 0)
    {
      String inc="";
      switch(incomingdata)
      {
        case 112:
          dfplay(45);
//          inc = coord + ", Bus is there";
//          bot.sendMessage(CHAT_ID,inc , ""); 
//          inc = "";     
        break;
        case 224:
          dfplay(35);
//          inc = coord + ", Car";
//          bot.sendMessage(CHAT_ID,inc , ""); 
//          inc = "";     
        break;
        case 336:
          dfplay(25);
//          inc = coord + ", Traffic Light";
//          bot.sendMessage(CHAT_ID,inc , ""); 
//          inc = "";     
        break;
        case 448:
          dfplay(7);
//          inc = coord + ", Stop Sign";
//          bot.sendMessage(CHAT_ID,inc , ""); 
//          inc = "";     
        break;
        case 555:
          dfplay(65);
//          inc = coord + ", Red Light";
//          bot.sendMessage(CHAT_ID,inc , ""); 
//          inc = "";     
        break;
        case 666:
          dfplay(75);
//          inc = coord + ", Green Light";
//          bot.sendMessage(CHAT_ID,inc , ""); 
//          inc = "";     
        break;
      }
      incomingdata=0;
    }

//    if(distanceCm4 < 10)
//    {
//      String dc4="";
//      if(DFPLAY)      
//        dfplay(4);
//      dc4 = coord + ", Obstacle in the right";
//      bot.sendMessage(CHAT_ID,dc4 , ""); 
//      dc4 = "";     
//    }
//    coord = "";
  }
  myTime = millis();
  }
#endif
  
  delay(100);
}

void dfplay(int index)
{
  ss.write(0x7E);
  ss.write(0xFF);
  ss.write(0x06);
  ss.write(0x03);
  ss.write(0x00);
  ss.write(0x00);
  ss.write(index);
  ss.write(0xEF);
  delay(600);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
