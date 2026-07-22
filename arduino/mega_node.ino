#include <DHT.h>

#define DHTPIN 48
#define DHTTYPE DHT11
#define RED_PIN 9
#define GREEN_PIN 10 
#define BLUE_PIN 11
#define BUZZER_PIN 7

// -----------------------------
// SYSTEM CONSTANTS
// -----------------------------
const float WARNING_TEMP = 25.3;
const float CRITICAL_TEMP = 28.0;
const float HIGH_CRITICAL_TEMP = 30;
const int YELLOW_GREEN_VALUE = 50;
const int BEEP_DURATION = 400;
const unsigned long BEEP_INTERVAL = 10000;
const unsigned long SENSOR_INTERVAL = 1000;
unsigned long previousBeepTime = 0;
unsigned long previousSensorReadTime = 0;
unsigned long currentBeepInterval;
DHT dht(DHTPIN, DHTTYPE);

// -----------------------------
// RGB FUNCTIONS
// -----------------------------

void setGreen() {

  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 255);
  analogWrite(BLUE_PIN, 0);
}

void setYellow() {

  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, YELLOW_GREEN_VALUE);
  analogWrite(BLUE_PIN, 0);
}

void setRed() {

  analogWrite(RED_PIN, 255);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}
void setOff() {

  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}
//--------------------------------
void setup() {

  Serial.begin(9600);              // Start Serial Communication
  dht.begin();                     // Start DHT Sensor
  Serial.println("DHT11 Monitoring System Started");
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

// -----------------------------
// Main Loop
// -----------------------------

void loop() {
  
  float TEMP;
  float HUM;
  
  if (millis() - previousSensorReadTime >= SENSOR_INTERVAL){
    previousSensorReadTime = millis();
    HUM = dht.readHumidity();         // Read Humidity
    TEMP= dht.readTemperature();      // Read Temperature
    if (isnan(HUM) || isnan(TEMP)) {        // Check if sensor reading failed
      Serial.println("ERROR: Sensor read failed!");
      delay(2000);
      return;
    }
                                            // Send Structured Data
    Serial.print("TEMP:");
    Serial.print(TEMP);
    Serial.print(",HUM:");
    Serial.println(HUM);

    if (TEMP < WARNING_TEMP ) {                //GREEN
      setGreen();
    } 
    else if (TEMP >= WARNING_TEMP && TEMP < CRITICAL_TEMP ){       //Yellow
      setYellow();
    }
    else if (TEMP >= CRITICAL_TEMP && TEMP < HIGH_CRITICAL_TEMP) {

      setRed();

      if (millis() - previousBeepTime >= BEEP_INTERVAL) {

        digitalWrite(BUZZER_PIN, HIGH);
        delay(BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
        previousBeepTime = millis();
      }
    }                              
    else {

      setRed();
      if (millis() - previousBeepTime >= BEEP_INTERVAL / 4) {
        digitalWrite(BUZZER_PIN, HIGH);
        setOff();
        delay(BEEP_DURATION);
        digitalWrite(BUZZER_PIN, LOW);
        setRed();
        previousBeepTime = millis();
      }
    }
  }
}

