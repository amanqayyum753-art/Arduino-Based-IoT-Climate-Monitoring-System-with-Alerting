#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 8
#define DHTTYPE DHT22

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
DHT dht(DHTPIN, DHTTYPE);

// ---------------- PINS ----------------
const int greenLED = 9;
const int yellowLED = 10;
const int redLED = 13;
const int buzzer = 7;

// ---------------- DATA ----------------
float h = 0;
float t = 0;
float maxTemp = -100;
float minTemp = 100;

// ---------------- TIMING ----------------
unsigned long lastScreenUpdate = 0;
unsigned long lastBlinkUpdate = 0;

const long screenInterval = 2000;
const long blinkInterval = 400;

int screenIndex = 0;
bool blinkState = false;

// ---------------- SETUP ----------------
void setup() {
  dht.begin();
  lcd.begin(16, 2);

  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

// ---------------- SENSOR ----------------
void readSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (!isnan(t) && !isnan(h)) {
    if (t > maxTemp) maxTemp = t;
    if (t < minTemp) minTemp = t;
  }
}

// ---------------- LCD ----------------
void updateLCD() {
  lcd.clear();

  switch (screenIndex) {

    case 0:
      lcd.setCursor(0, 0);
      lcd.print("H: ");
      lcd.print(h);
      lcd.print("%");

      lcd.setCursor(0, 1);
      lcd.print("T: ");
      lcd.print(t);
      lcd.print("C");
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Temperature:");

      lcd.setCursor(0, 1);
      if (t <= 20) lcd.print("Cold");
      else if (t <= 30) lcd.print("Comfortable");
      else if (t <= 35) lcd.print("Warm");
      else lcd.print("Hot");
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Max: ");
      lcd.print(maxTemp);

      lcd.setCursor(0, 1);
      lcd.print("Min: ");
      lcd.print(minTemp);
      break;

    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Humidity");

      lcd.setCursor(0, 1);
      if (h <= 30) lcd.print("Dry");
      else if (h <= 60) lcd.print("Normal");
      else if (h <= 80) lcd.print("Humid");
      else lcd.print("Very Humid");
      break;
  }

  screenIndex = (screenIndex + 1) % 4;
}

// ---------------- LEDS ----------------
void updateLEDs() {

  if (h <= 30) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  }

  else if (h <= 60) {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);

    if (millis() - lastBlinkUpdate >= blinkInterval) {
      lastBlinkUpdate = millis();
      blinkState = !blinkState;
      digitalWrite(yellowLED, blinkState);
    }
  }

  else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);

    if (millis() - lastBlinkUpdate >= blinkInterval) {
      lastBlinkUpdate = millis();
      blinkState = !blinkState;
      digitalWrite(redLED, blinkState);
    }
  }
}

// ---------------- BUZZER ----------------
void updateBuzzer() {
  if (h > 50 || t > 35) {
    tone(buzzer, 2000);
  } else {
    noTone(buzzer);
  }
}

// ---------------- LOOP ----------------
void loop() {

  readSensor();

  updateLEDs();
  updateBuzzer();

  if (millis() - lastScreenUpdate >= screenInterval) {
    lastScreenUpdate = millis();
    updateLCD();
  }
}
