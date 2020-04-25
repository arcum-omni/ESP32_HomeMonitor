/*
 * Home Monitoring Station
 * This project displays meteorological data such as temperature, humidity, and
 * barometric pressure. 
 * It also monitors luminosity.
 * There is an OLED display that shows the collected data.
 * The unit also acts as a webserver publishing the collected data on a webpage.
 * The unit also has a relay to switch high power devices.
 * 
 * Brd:  Espressif ESP32-WROOM-32D DevKitC V4
 * OLED: Kailedi 0.96in Yellow/Blue SSD1306 i2c
 * Snsr: Ohyehn BME280, i2c; Adafruit TSL2591, i2c
 * Rly:  KeeYee 3v Trigger Relay 250V/10A w/ Optocoupler Optical Isolation
 * IDE:  Arduino IDE
 *       https://www.arduino.cc/reference/en/#structure
 * Libs: Adafruit BME280 2.0.1
 *       Adafruit GFX 1.7.5
 *       Adafruit SSD1306 2.2.1
 *       Adafruit TSL2591 1.2.1
 *       Wifi 1.2.7
 * 
 * GPIO (Sensor : Board)
 * RED : 25,  LED, Temp Indicator
 * YLW : 26,  LED, Storm Indicator
 * GRN : 27,  LED, IdealConditions Indicator
 * BLU : 12,  LED, Humidity Indicator
 * WHT : 13,  LED, Luminosity Indicator
 * RLY : 5,   Relay
 * VIN : 3V3, Power
 * GND : GND, Ground
 * SCL : 22,  Clock
 * SDA : 21,  Data
 * 
 * Author Travis Eiler
 * Date:  20200424
 */

// BME280 Sensor Library
#include <Adafruit_BME280.h>

// Graphics Library for OLED
#include <Adafruit_GFX.h>

// SSD1306 OLED Driver Chip Library
#include <Adafruit_SSD1306.h>

// TSL2591 Light Sensor Library
#include "Adafruit_TSL2591.h"

// WebServer Library
#include <WebServer.h>

// WiFi Connectivity Library
#include <WiFi.h>

// I2C Protocol Library
#include <Wire.h>

// Constants
#define boardNum 1 // IP: 10.0.0.115
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SEALEVELPRESSURE_HPA (1013.250) // 1atm = 760mmHg = 29.9212inHg = 14.696psi = 1013.250hPa
#define ssid "YOUR_SSID" // SSID from "YOUR" local network
#define password "YOUR_PASSWORD" // Password for "YOUR" local network

// Instantiate Objects
Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
WebServer server(80);

// Declare Variables
float tempC, tempF;
float humidity;
float presPa, presInHg;
float altM, altFt;
int redLED = 25;
int ylwLED = 26;
int grnLED = 27;
int bluLED = 12;
int whtLED = 13;
int relay = 5;
int ir, full; // uint16_t
int lum; // uint32_t
int flashPeriod = 250;
int delayPeriod = 1000;

/*
 * Configure TSL2591 Light Sensor
 */
void configLightSensor(){
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
}

/*
 * The setup() function is called when a sketch starts and will only run once,
 * after each powerup or reset of the Arduino board.
 */
void setup() {
  // Set Output pins to power LED
  pinMode(redLED, OUTPUT);
  pinMode(ylwLED, OUTPUT);
  pinMode(grnLED, OUTPUT);
  pinMode(bluLED, OUTPUT);
  pinMode(whtLED, OUTPUT);
  pinMode(relay, OUTPUT);
  
  // Set Serial Commo Baud Rate
  Serial.begin(115200);
  delay(100);
  
  // Establish i2C Commo Protocol
  Wire.begin();
  delay(100);
  
  // Set BME Address
  bme.begin(0x76);
  
  // Configure Light Sensor
  configLightSensor();
  
  Serial.print("ESP32 #");
  Serial.print(boardNum);
  Serial.println(" Startup Initiated");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.print("Establishing connection to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP Server Started");

  Serial.println("LED Test Started");
  testLED();

  Serial.print("ESP32 #");
  Serial.print(boardNum);
  Serial.println(" Startup Complete");
  Serial.println();
}

void testLED(){
  flashLED(whtLED);
  flashLED(bluLED);
  flashLED(grnLED);
  flashLED(ylwLED);
  flashLED(redLED);
}
void turnOnLED(int color){
  digitalWrite(color, HIGH);
}

void flashLED(int color){
  digitalWrite(color, HIGH);
  delay(flashPeriod);
  digitalWrite(color, LOW);
  delay(flashPeriod);
  digitalWrite(color, HIGH);
  delay(flashPeriod);
  digitalWrite(color, LOW);
}

void turnOffLED(int color){
  digitalWrite(color, LOW);
}

/*
 * After creating a setup() function,
 * the loop() function loops consecutively, 
 * allowing your program to change and respond. 
 * Use it to actively control the Arduino board.
 */
void loop() {
  // Allow time for sensors to stabilize
  delay(delayPeriod);
  gatherMeteorologicalData();
  gatherLightData();
  displayData();
  emergencyLights();
  tempWarningLight();
  humidityWarningLight();
  stormWarningLight();
  idealConditionsLight();
  thermostat();
  display.display();
  server.handleClient();
}

void gatherMeteorologicalData(){
  // Read Temp
  tempC = bme.readTemperature();
  tempF = tempC * 1.8 + 32.0;
  
  // Read Humidity
  humidity = bme.readHumidity();
  
  // Read Pressure
  presPa = bme.readPressure();
  presInHg = presPa / 3386.3887;
  
  // Read Altitude
  altM = bme.readAltitude(SEALEVELPRESSURE_HPA); // Altitude in meters
  altFt = altM * 3.28084; // 3.28084 ft/m 
}

void gatherLightData(){
  // Lum
  lum = tsl.getFullLuminosity();

  // IR Spectrum
  ir = lum >> 16;

  // Full Spectrum
  full = lum & 0xFFFF;
}

void displayData(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  if(tempF > 90){
    display.setTextSize(2);
    display.setCursor(10,0);
    display.print("WARNING");
    display.setCursor(10,20);
    display.print("HIGH TEMP");
    display.setCursor(10,40);
    display.print(tempF);
    display.print("F");
  }
  else{
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Hut8 Weather Station");
    display.setCursor(21,9);
    display.print("ESP32 Board #1");
    display.setCursor(0,20);
    display.print(tempF);
    display.print("F");
    display.setCursor(64,20);
    display.print(presInHg);
    display.print("inHg");
    display.setCursor(0,32);
    display.print(humidity);
    display.print("%RH");
    display.setCursor(64,32);
    display.print(altFt);
    display.print("ft");
    display.setCursor(0,44);
    display.print(full);
    display.print("Lux");
    display.setCursor(64,44);
    display.print(ir);
    display.print("LuxIR");
  }
  display.setTextSize(1);
  display.setCursor(18,56);
  display.print("v2.03 24APR2020");
}

// Night Light
void emergencyLights(){
  if(full < 100){
    turnOnLED(whtLED);
  }
  else {
    turnOffLED(whtLED);
  }
}

void tempWarningLight(){
  if(tempF > 80){
    turnOnLED(redLED);
  }
  else if (tempF > 65){
    turnOffLED(redLED);
  }
  else {
    flashLED(redLED);
  }
}

void humidityWarningLight(){
  if(humidity > 50){
    turnOnLED(bluLED);
  }
  else if (humidity < 30){
    flashLED(bluLED);
  }
  else {
    turnOffLED(bluLED);
  }
}

void idealConditionsLight(){
  if(tempF<72 && tempF>68 && humidity<45 && humidity>35){
    flashLED(grnLED);
  }
  else {
    turnOffLED(grnLED);
  }
}

void stormWarningLight(){
  // TODO: We really need to calculate the rate the pressure is dropping
  if(presInHg < 29.5){
    turnOnLED(ylwLED);
  }
  else {
    turnOffLED(ylwLED);
  }
}

void thermostat(){
  int thermostatSetting = 75;
  int hysteresis = 3;
  if(tempF < thermostatSetting){
    digitalWrite(relay, HIGH);
  }
  else if (tempF > thermostatSetting+hysteresis){
    digitalWrite(relay, LOW);
  }
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(tempF,humidity,presInHg,altFt,boardNum)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temp,float humi,float pres,float alti, int brdNum){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>ESP32 #1</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr +="body{margin: 0px;} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr +=".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  ptr +=".temperature .reading{color: #F29C1F;}";
  ptr +=".humidity .reading{color: #3B97D3;}";
  ptr +=".pressure .reading{color: #26B99A;}";
  ptr +=".altitude .reading{color: #955BA5;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:65px}";
  ptr +="</style>";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<h1 style=\"margin-bottom: 0px;\">Hut8 Weather Station</h1>";
  ptr +="<h2 style=\"margin-top: 0px;\">ESP32 Board #1</h2>";
  ptr +="<div class='container'>";
  ptr +="<div class='data temperature' style='padding-top: 0px; padding-bottom: 0px;'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
  ptr +="C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
  ptr +="c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
  ptr +="c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
  ptr +="s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#F29C21 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Temperature</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(float)temp;
  ptr +="<span class='superscript'>&deg;F</span></div>";
  ptr +="</div>";
  ptr +="<div class='data humidity' style='padding-top: 0px; padding-bottom: 0px;'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 29.235 40.64'height=40.64px id=Layer_1 version=1.1 viewBox='0 0 29.235 40.64'width=29.235px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><path d='M14.618,0C14.618,0,0,17.95,0,26.022C0,34.096,6.544,40.64,14.618,40.64s14.617-6.544,14.617-14.617";
  ptr +="C29.235,17.95,14.618,0,14.618,0z M13.667,37.135c-5.604,0-10.162-4.56-10.162-10.162c0-0.787,0.638-1.426,1.426-1.426";
  ptr +="c0.787,0,1.425,0.639,1.425,1.426c0,4.031,3.28,7.312,7.311,7.312c0.787,0,1.425,0.638,1.425,1.425";
  ptr +="C15.093,36.497,14.455,37.135,13.667,37.135z'fill=#3C97D3 /></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Humidity</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(float)humi;
  ptr +="<span class='superscript'>%</span></div>";
  ptr +="</div>";
  ptr +="<div class='data pressure' style='padding-top: 0px; padding-bottom: 0px;'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 40.542 40.541'height=40.541px id=Layer_1 version=1.1 viewBox='0 0 40.542 40.541'width=40.542px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M34.313,20.271c0-0.552,0.447-1,1-1h5.178c-0.236-4.841-2.163-9.228-5.214-12.593l-3.425,3.424";
  ptr +="c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293c-0.391-0.391-0.391-1.023,0-1.414l3.425-3.424";
  ptr +="c-3.375-3.059-7.776-4.987-12.634-5.215c0.015,0.067,0.041,0.13,0.041,0.202v4.687c0,0.552-0.447,1-1,1s-1-0.448-1-1V0.25";
  ptr +="c0-0.071,0.026-0.134,0.041-0.202C14.39,0.279,9.936,2.256,6.544,5.385l3.576,3.577c0.391,0.391,0.391,1.024,0,1.414";
  ptr +="c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293L5.142,6.812c-2.98,3.348-4.858,7.682-5.092,12.459h4.804";
  ptr +="c0.552,0,1,0.448,1,1s-0.448,1-1,1H0.05c0.525,10.728,9.362,19.271,20.22,19.271c10.857,0,19.696-8.543,20.22-19.271h-5.178";
  ptr +="C34.76,21.271,34.313,20.823,34.313,20.271z M23.084,22.037c-0.559,1.561-2.274,2.372-3.833,1.814";
  ptr +="c-1.561-0.557-2.373-2.272-1.815-3.833c0.372-1.041,1.263-1.737,2.277-1.928L25.2,7.202L22.497,19.05";
  ptr +="C23.196,19.843,23.464,20.973,23.084,22.037z'fill=#26B999 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Pressure</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(float)pres;
  ptr +="<span class='superscript'>inHg</span></div>";
  ptr +="</div>";
  ptr +="<div class='data altitude' style='padding-top: 0px; padding-bottom: 0px;'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 58.422 40.639'height=40.639px id=Layer_1 version=1.1 viewBox='0 0 58.422 40.639'width=58.422px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M58.203,37.754l0.007-0.004L42.09,9.935l-0.001,0.001c-0.356-0.543-0.969-0.902-1.667-0.902";
  ptr +="c-0.655,0-1.231,0.32-1.595,0.808l-0.011-0.007l-0.039,0.067c-0.021,0.03-0.035,0.063-0.054,0.094L22.78,37.692l0.008,0.004";
  ptr +="c-0.149,0.28-0.242,0.594-0.242,0.934c0,1.102,0.894,1.995,1.994,1.995v0.015h31.888c1.101,0,1.994-0.893,1.994-1.994";
  ptr +="C58.422,38.323,58.339,38.024,58.203,37.754z'fill=#955BA5 /><path d='M19.704,38.674l-0.013-0.004l13.544-23.522L25.13,1.156l-0.002,0.001C24.671,0.459,23.885,0,22.985,0";
  ptr +="c-0.84,0-1.582,0.41-2.051,1.038l-0.016-0.01L20.87,1.114c-0.025,0.039-0.046,0.082-0.068,0.124L0.299,36.851l0.013,0.004";
  ptr +="C0.117,37.215,0,37.62,0,38.059c0,1.412,1.147,2.565,2.565,2.565v0.015h16.989c-0.091-0.256-0.149-0.526-0.149-0.813";
  ptr +="C19.405,39.407,19.518,39.019,19.704,38.674z'fill=#955BA5 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Altitude</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(float)alti;
  ptr +="<span class='superscript'>ft</span></div>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="<p>Hut8.dev, April 2020</p>";
  ptr +="</body>";
  ptr +="</html>";
  return ptr;
}
