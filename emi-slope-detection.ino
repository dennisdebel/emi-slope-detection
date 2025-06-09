// connect a buzzer to pin 13
// connect a coil to pin 2 and gnd (you can experiment with different coils, and resistors, caps paralell to the coil, you can even add multiple coils in parallel and series)

#define RGB_BRIGHTNESS 64 // Change white brightness (max 255)
#define RGB_BUILTIN 48

int adcPin = 2;
int buzzerPin = 13;
int previousValue = 0;
int previousSlope = 0;
int currentValue = 0;
int slope = 0;
const int ledPin = RGB_BUILTIN;


// Activity rate tracking
unsigned long lastRateUpdate = 0;
unsigned long rateInterval = 1000; // 1 second
int activityCount = 0;
float activityRate = 0;

// LED smoothing
float currentR = 0, currentG = 0, currentB = 0;
const float smoothing = 0.05;


void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  currentValue = analogRead(adcPin);
  slope = currentValue - previousValue;

// Zero-crossing of slope
  if ((previousSlope < 0 && slope > 0) || (previousSlope > 0 && slope < 0)) {
    tone(buzzerPin, 3000, 10); // Beep!
    activityCount++;
  }

  previousValue = currentValue;
  previousSlope = slope;
  
//  // Simple threshold to ignore small changes (noise) -- this worked best...
////  if (abs(slope) > 10) {
////    Serial.print("Slope: "); Serial.println(slope);
////    //tone(8,2000,20);
////    
////  }  --- this worked...
//  //Zero-crossings of slope
//  if ((previousValue < 0 && slope > 0) || (previousValue > 0 && slope < 0)) {
//  // signal reversed direction
//  tone(8,2000,20);
//  }
//
//  noTone(8);
//  previousValue = currentValue;

  // === UPDATE RATE EVERY SECOND ===
  if (millis() - lastRateUpdate >= rateInterval) {
    activityRate = activityCount * (1000.0 / rateInterval); // crossings per sec
    activityCount = 0;
    lastRateUpdate = millis();

    Serial.print("Activity rate: ");
    Serial.println(activityRate);
  }

  // === UPDATE LED BASED ON RATE ===
  updateLedByRate(activityRate);
  delay(10); // adjust sampling rate if needed
}



void updateLedByRate(float rate) {
  // Constrain rate to 0–20 Hz
  rate = constrain(rate, 0, 20);

  // Map rate to brightness (0–64)
  float brightness = map(rate, 0, 20, 0, 64);

  // Color logic: green → yellow → red
  float targetR = 0, targetG = 0, targetB = 0;

  if (rate <= 10) {
    float t = rate / 10.0;
    targetR = 255 * t;
    targetG = 255;
  } else {
    float t = (rate - 10.0) / 10.0;
    targetR = 255;
    targetG = 255 * (1.0 - t);
  }

  // Apply brightness
  targetR = (targetR * brightness) / 255;
  targetG = (targetG * brightness) / 255;
  targetB = (targetB * brightness) / 255;

  // Smooth transition
  currentR += (targetR - currentR) * smoothing;
  currentG += (targetG - currentG) * smoothing;
  currentB += (targetB - currentB) * smoothing;

  // Write to LED
  rgbLedWrite(RGB_BUILTIN, (int)currentR, (int)currentG, (int)currentB);
}
