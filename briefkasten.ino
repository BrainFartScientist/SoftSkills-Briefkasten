//#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>


char auth[] = ""; //!!!Token von Blynk!!!
char ssid[] = ""; //!!!Hier Namen vom Netzwerk eintippen!!!
char pass[] = ""; //!!!Passwort vom Netzwerk!!!

int count = 0;  //alter Zähler für Briefe
int extern letterCount; //aktueller Zähler

const long utcOffsetInSeconds = 7200; //Sommer = 7200 und Winter = 3600
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
Servo Servo_SG90;

void letterCountEvent() //aktualisiert Anzahl der Briefe
{
  if(letterCount > count){ //überprüft ob sich der Zähler erhöht hat
    Blynk.virtualWrite(V5, letterCount);
    count = letterCount;
    Blynk.virtualWrite(V3, "Etwas wurde in den Briefkasten geworfen.");
    timeClient.update();
    Blynk.virtualWrite(V2, timeClient.getFormattedTime());
  }
}
void resetAll(){ //alle Werte werden resettet
  count = 0;
  letterCount = 0;
  Blynk.virtualWrite(V5, 0);
  Blynk.virtualWrite(V3,"Briefkasten ist leer.");
  Blynk.virtualWrite(V2," ");
  timeClient.update();
  Blynk.virtualWrite(V6, timeClient.getFormattedTime());
}
BLYNK_WRITE(V4) //Reset-Button von Blynk
{
  letterCount = 0;
  count = 0;
  Blynk.virtualWrite(V5, 0);
  Blynk.virtualWrite(V3,"Briefkasten ist leer.");
  Blynk.virtualWrite(V2," ");
  delay(100);
}

bool opened = false;
int open =100;
int close = 170;
BLYNK_WRITE(V1) //öffnen und schließen
{
  if(param.asInt()==1) //! 
  {
    Servo_SG90.write(open);
    delay(100);
    Blynk.virtualWrite(V8, "Das Schloss ist offen.");
    opened = true;
  }
  else
  {
    Servo_SG90.write(close);
    resetAll();
    Blynk.virtualWrite(V8, "Das Schloss wurde verriegelt.");
    opened = false;
  }
  
}
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
BLYNK_WRITE(V11) //um zu schauen ob der Mikrocontroller wirklich online ist
{ //kann igonoriert werden
  Blynk.virtualWrite(V12, "Signal erhalten");
  delay(500);
  Blynk.virtualWrite(V12, " ");
}
void setup() 
{
  Blynk.begin(auth, ssid, pass, "iot.informatik.uni-oldenburg.de", 8080);
  magnetInit();
  fingerWriteInit();
  fingerReadInit();
  timeClient.begin();
  Blynk.virtualWrite(V5, 0);
  Blynk.virtualWrite(V3,"Briefkasten ist leer.");
  Blynk.virtualWrite(V8, "Das Schloss wurde verriegelt.");
  Servo_SG90.attach(2); // 2 = D4
  Servo_SG90.write(close);
}

void loop() 
{
  runMagnet();
  letterCountEvent();
  getFingerprintID();
  Blynk.run(); //ruft BLYNK_WRITE auf
}
