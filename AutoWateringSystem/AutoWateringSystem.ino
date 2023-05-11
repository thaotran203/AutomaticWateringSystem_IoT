//Include the library files
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6ycVivoRI"
#define BLYNK_TEMPLATE_NAME "Moisture Sensor"
#define BLYNK_AUTH_TOKEN "Ss4tXo10Z8_FmriyG2Q1UfItF04wAurM"
#define FIREBASE_HOST "https://fir-smarthome-4d96e-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyCcTrOFOgzZGBaoQfp8jbjmW3b5VO__NgA"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP8266.h>

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "Ss4tXo10Z8_FmriyG2Q1UfItF04wAurM";//Enter your Auth token
char ssid[] = "Greenwich-Student"; //Enter your WIFI name
char pass[] = "12345678"; //Enter your WIFI password


// Pin definitions
const int moisturePin = A0;
const int relayPin = D3;
WidgetLCD LCD(V1);
FirebaseData firebaseData;

// Set the threshold for watering (adjust as needed)
const int moistureThreshold = 45;

// Set the watering duration (in milliseconds)
const int wateringDuration = 5000;
String MoistureStatus = "";

void setup() {
  Serial.begin(9600);
  Wire.begin(D2, D1); //SDA và SCL
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(moisturePin, INPUT);
  pinMode(relayPin, OUTPUT);
  Blynk.begin(auth, ssid, pass);

  // initialize the LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
}

//Get the button value
BLYNK_WRITE(V3) {
  int Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(relayPin, HIGH);
    Blynk.virtualWrite(V3, HIGH);
    lcd.setCursor(0, 1);

  } else {
    digitalWrite(relayPin, LOW);
    Blynk.virtualWrite(V3, LOW);

  }
}

void loop() {
  int moistureValue = analogRead(moisturePin);
  moistureValue = map(moistureValue, 0, 1024, 0, 100);
  moistureValue = (moistureValue - 100) * -1;

  if (moistureValue < moistureThreshold) {
    MoistureStatus = "Trees need watering";
    Blynk.virtualWrite(V0, moistureValue);
    LCD.print(0, 0, "Trees need watering");
    Firebase.RTDB.setString(&firebaseData, "Moisture Sensor/Status", MoistureStatus);

    lcd.setCursor(0, 0);
    lcd.print("Moisture: ");
    lcd.print(moistureValue);

    digitalWrite(relayPin, HIGH); // Turn on water pump
    lcd.setCursor(0, 1);
    lcd.print("Motor is ON ");
  }
  else {
    MoistureStatus = "No need watering";
    Blynk.virtualWrite(V0, moistureValue);
    LCD.print(0, 0,"No need watering");
    Firebase.RTDB.setString(&firebaseData, "Moisture Sensor/Status", MoistureStatus);

    lcd.setCursor(0, 0);
    lcd.print("Moisture: ");
    lcd.print(moistureValue);
    
    digitalWrite(relayPin, LOW); // Turn off water pump
    lcd.setCursor(0, 1);
    lcd.print("Motor is OFF");

  }
  
  Blynk.run();//Run the Blynk library
  Firebase.RTDB.setFloat(&firebaseData, "Moisture Sensor/Value", moistureValue); 
}
