#define HalogenPin 14        // Send the value from the potentiometer (P1.6)
#define FanPowerPin 12       // Turn on and off the fan (P2.4)
#define LEDPin 10            // To control the LED(P2.2)


//#define OnePin 2
//#define TwoPin 3
//#define ThreePin 4
//#define FourPin 5
//#define FivePin 8
//#define SixPin 9

void setup()  {
  Serial.begin(9600);                              //Start data transmission
  pinMode(HalogenPin, OUTPUT);                 //Set HalogenPin as output
  pinMode(FanPowerPin, OUTPUT);                    //Set FanPowerPin as output
  pinMode(LEDPin, OUTPUT);                         //Set LEDPin as output
  
  UnpowerFan();
  UnpowerLed();
  UnpowerHalogen();
}

void loop()  {
  int num;
  if (Serial.available() > 0) {
    num = Serial.parseInt();
    if (num==1 ) {
      PowerFan(); 
    } else if (num==2) {
      UnpowerFan(); 
    } else if (num==3) {
      PowerLed(); 
    } else if (num==4) {
      UnpowerLed(); 
    } else if (num==5) {
      PowerHalogen(); 
    } else if (num==6) {
      UnpowerHalogen(); 
    }
  }
}

void PowerFan() {
  digitalWrite(FanPowerPin, HIGH); 
}

void UnpowerFan() {
 digitalWrite(FanPowerPin, LOW); 
}

void PowerLed() {
  digitalWrite(LEDPin, HIGH); 
}

void UnpowerLed() {
  digitalWrite(LEDPin, LOW); 
}

void PowerHalogen() {
  analogWrite(HalogenPin, 0); 
}

void UnpowerHalogen() {
  analogWrite(HalogenPin, 255); 
}
