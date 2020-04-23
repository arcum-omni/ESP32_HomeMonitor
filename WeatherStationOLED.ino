/*
 * Simple Weather Station w/ Display
 * Coding from Scratch for a OLED & BME280
 * No Network Connectivity
 * 
 * GPIO (Sensor/Board)
 * VIN: 3V3 - Power
 * GND: GND - Ground
 * SCL: 22 - Clock
 * SDA: 21 - Data
 * 
 * Brd:  Espressif ESP32-WROOM-32D DevKitC V4
 * OLED: Kailedi 0.96in Yellow/Blue SSD1306, I2C
 * Snsr: Ohyehn BME280, I2C
 * Libs: Adafruit BME280 2.0.1
 *       Adafruit SSD1306 2.2.1
 *       Adafruit GFX 1.7.5
 * 
 * Author Travis Eiler
 * Date:  20200422
 */

#include <Wire.h>

// Display Driver Chip Library
#include <Adafruit_SSD1306.h>

// Graphics Library
#include <Adafruit_GFX.h>

// BME280 Libraries
#include <Adafruit_BME280.h>

// 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA/SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BME280 bme;

float tempC, tempF;
float humidity;
float presPa, presInHg;
// Barometric Pressure, Mean Sea Level Pressure (MSLP)
// 1atm = 760mmHg = 29.9212inHg = 14.696psi = 1013.250hPa
#define SEALEVELPRESSURE_HPA (1013.250)
float altM, altFt;

void setup() {
  Wire.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  bme.begin(0x76);
}

void displayMeteorologicalData(){
  // Allow Sensor To Stabilize
  delay(2500);

  tempC = bme.readTemperature();
  tempF = tempC * 1.8 + 32.0;
  humidity = bme.readHumidity();
  presPa = bme.readPressure();
  presInHg = presPa / 3386.3887;
  altM = bme.readAltitude(SEALEVELPRESSURE_HPA); // Altitude in meters
  altFt = altM * 3.28084; // 3.28084 ft/m

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Hut8 Weather Station");
  display.setCursor(21,9);
  display.print("ESP32 & BME280");
  display.setCursor(0,20);
  display.print("TEMP: ");
  display.print(tempF);
  display.print(" F");
  display.setCursor(0,32);
  display.print("RH:   ");
  display.print(humidity);
  display.print(" %");
  display.setCursor(0,44);
  display.print("ATM:  ");
  display.print(presInHg);
  display.print(" inHg");
  display.setCursor(0,56);
  display.print("ALT:  ");
  display.print(altFt);
  display.print(" ft");
}

void loop() {
  displayMeteorologicalData();
  display.display();
}
