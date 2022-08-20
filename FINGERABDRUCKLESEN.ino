#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(4, 0);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint fingerRead = Adafruit_Fingerprint(&mySerial);

void fingerReadInit() //void setup
{
  // set the data rate for the sensor serial port
  fingerRead.begin(57600);
  delay(5);
  if (fingerRead.verifyPassword()) {
    Blynk.virtualWrite(V0, "Found fingerprint sensor!");
  } else {
    Blynk.virtualWrite(V0, "Did not find fingerprint sensor :(");
    while (1) { delay(1); }
    return;
  }
  fingerRead.getParameters();
  fingerRead.getTemplateCount();

  if (fingerRead.templateCount == 0) {
    Blynk.virtualWrite(V0, "Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Blynk.virtualWrite(V10, "Wartet auf Finger...");
  }
}


uint8_t getFingerprintID() {
  uint8_t p = fingerRead.getImage();
  // OK converted!
  p = fingerRead.image2Tz();
  p = fingerRead.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Blynk.virtualWrite(V10, "Finger wurde identifiziert.");
    if(opened == false) //Schloss öffnet oder schließt sich
    {
      Servo_SG90.write(open);
      Blynk.virtualWrite(V8, "Das Schloss ist offen.");
      opened = true;
      return p;
    }
    else
    {
      Servo_SG90.write(close);
      resetAll();
      Blynk.virtualWrite(V8, "Das Schloss wurde verriegelt.");
      opened = false;
      return p;
    }
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Blynk.virtualWrite(V0,"Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Blynk.virtualWrite(V10,"Unbekannter Finger entdeckt!");
    return p;
  } else {
    //Blynk.virtualWrite(V0,"Unknown error");
    return p;
  }

  // found a match!
  return fingerRead.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = fingerRead.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = fingerRead.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = fingerRead.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  return fingerRead.fingerID;
}
