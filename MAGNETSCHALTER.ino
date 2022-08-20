int letterCount;
const int reedPin = 5; //D1 Pin für Magnetschalter
bool switchState;
bool reedStatusOpen = false;

void magnetInit() { //initialisiert Pin für Magnetschalter
  pinMode(reedPin, INPUT);
}

bool runMagnet() { //main-Funktion
  switchState = digitalRead(reedPin);

  if(switchState == LOW){
    if(reedStatusOpen == true){
      reedStatusOpen = false;
    }
  }
  else{
    if(reedStatusOpen == false){
      reedStatusOpen = true;
      letterCount++;
    }
  }
  delay(100);
  return reedStatusOpen;
}
