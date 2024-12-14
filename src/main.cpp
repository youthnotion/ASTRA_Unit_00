#include <Arduino.h>
#include <Creds.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>

#define BLYNK_PRINT Serial

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define analogPin A0
#define dhtPin D5
#define trigPin D6
#define echoPin D7
#define buzzerPin d8

#define DHTTYPE DHT22

DHT_Unified dht(dhtPin, DHTTYPE);

int analogSensorValue = analogRead(analogPin);
float temperature = 0.0;
float humidity = 0.0;
long duration;
float distanceCm;
float distanceInch;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

BlynkTimer timer;

void transmission() {
  Blynk.virtualWrite(V0, analogSensorValue);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, distanceCm);
  Blynk.virtualWrite(V5, millis() / 1000);
}

void setup() {

  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, transmission);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));

  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("Setup Completed");
  display.display();
}


void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Blynk.run();
  timer.run();

  Serial.print("Analog Sensor Value: ");
  Serial.println(analogSensorValue);

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    temperature = event.temperature;
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
  } else {
    Serial.println(F("Error reading temperature!"));
  }

  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    humidity = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.println(F("%"));
  } else {
    Serial.println(F("Error reading humidity!"));
  }


  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY / 2;
  distanceInch = distanceCm * CM_TO_INCH;
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 7);
  display.println((String) "Uptime:" + millis() / 1000 + (String) "ms");
  display.setCursor(0, 15);
  display.println(WiFi.localIP());
  display.setCursor(0, 23);
  display.println((String) "Network Strength:" + WiFi.RSSI());
  display.display();
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Network Strength: ");
  Serial.println(WiFi.RSSI());
}