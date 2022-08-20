
#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
//SoftwareSerial mySerial(0, 4); //2, 3 original

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

uint8_t fingerPrintNumber;
BLYNK_WRITE(V9)

{
  fingerPrintNumber = param.asInt();
  Blynk.virtualWrite(V0, "Speicherort gewählt.");
}

void fingerWriteInit() //void setup
{
  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Blynk.virtualWrite(V0, "Found fingerprint sensor!");
  } else {
    Blynk.virtualWrite(V0, "Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  finger.getParameters();
}

BLYNK_WRITE(V7)  // Neuen Fingerabdruck                // run over and over again
{
  id = fingerPrintNumber;
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Blynk.virtualWrite(V0, "Folgender Slot ausgewählt:");
  delay(1000);
  Blynk.virtualWrite(V0, id);
  delay(1000);

  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Blynk.virtualWrite(V0, "Wait for finger.");
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Blynk.virtualWrite(V0, "Finger accepted.");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Blynk.virtualWrite(V0, "Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Blynk.virtualWrite(V0, "Image error");
      break;
    default:
      Blynk.virtualWrite(V0, "Unknown error");
      break;
    }
    delay(2000);
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Blynk.virtualWrite(V0, "Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Blynk.virtualWrite(V0, "Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Blynk.virtualWrite(V0, "Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Blynk.virtualWrite(V0, "Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Blynk.virtualWrite(V0, "Could not find fingerprint features");
      return p;
    default:
      Blynk.virtualWrite(V0, "Unknown error");
      return p;
  }
  delay(1000);

  Blynk.virtualWrite(V0, "Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  Blynk.virtualWrite(V0, "Place same finger again");
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Blynk.virtualWrite(V0, "Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Blynk.virtualWrite(V0, "Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Blynk.virtualWrite(V0, "Imaging error");
      break;
    default:
      Blynk.virtualWrite(V0, "Unknown error");
      break;
    }
  }
  delay(2000);
  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Blynk.virtualWrite(V0, "Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Blynk.virtualWrite(V0, "Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Blynk.virtualWrite(V0, "Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Blynk.virtualWrite(V0, "Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Blynk.virtualWrite(V0, "Could not find fingerprint features");
      return p;
    default:
      Blynk.virtualWrite(V0, "Unknown error");
      return p;
  }
  delay(2000);

  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Blynk.virtualWrite(V0, "Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Blynk.virtualWrite(V0, "Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Blynk.virtualWrite(V0, "Fingerprints did not match");
    return p;
  } else {
    Blynk.virtualWrite(V0, "Unknown error");
    return p;
  }
  delay(2000);

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Blynk.virtualWrite(V0, "Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Blynk.virtualWrite(V0, "Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Blynk.virtualWrite(V0,"Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Blynk.virtualWrite(V0, "Error writing to flash");
    return p;
  } else {
    Blynk.virtualWrite(V0, "Unknown error");
    return p;
  }
  return true;
}
