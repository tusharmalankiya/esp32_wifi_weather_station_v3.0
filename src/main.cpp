#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <time.h>
#include "espwebserver.h"
#include "secrets.h"
#include "icons.h"
#include "logToGoogleSheets.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1

#define CLEAR_SKY_ICON "https://cdn-icons-png.flaticon.com/512/3222/3222800.png"
#define RAIN_ICON "https://cdn-icons-png.flaticon.com/512/1312/1312359.png"
#define CLOUD_ICON "https://cdn-icons-png.flaticon.com/512/4834/4834559.png"
#define UNKNOWN_ICON "https://cdn-icons-png.flaticon.com/512/5829/5829172.png"

// global variables
float temperature = 0.0;
int humidity = 0;
String weather = "", updated = "", png_icon = "";

// helper function declaration
String getFormattedTime(String format_type = "long");
void updateWeather();

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(115200);
  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // connect to wifi
  display.print("Connecting to WiFi...");
  display.display();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.println(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    display.print(".");
    display.display();
  }

  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Wifi Connected!");
  display.display();
  delay(1000);

  // Set timezone to your local time, e.g., for India (IST)
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov"); // 19800 = 5.5 hrs offset in seconds

  display.clearDisplay();
  display.setCursor(0, 0);
  Serial.print("Waiting for NTP time sync...");
  display.print("Waiting for NTP time sync...");
  display.display();

  while (time(nullptr) < 100000)
  {
    delay(100);
    display.print(".");
    Serial.print(".");
    display.display();
  }

  Serial.println("\nTime is set!");
  display.println("\n\nTime is set!");
  display.display();
  delay(1500);

  server.on("/", [](AsyncWebServerRequest *request){
    handleRoot(request, temperature, humidity, weather, updated, png_icon);
  });

  server.addHandler(&events); // Register SSE handler
  server.begin();
  Serial.println("Connect Web Server at http://localhost:80");

  // updateWeather();
  logToGoogleSheetSetUp();
}

unsigned long previousWeatherMillis = 0;
const unsigned long weatherUpdateInterval = 3000; // 30 seconds

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {    
    unsigned long currentMillis = millis();
    
    // Check if 30 seconds have passed
    if (currentMillis - previousWeatherMillis >= weatherUpdateInterval || previousWeatherMillis == 0)
    {
      previousWeatherMillis = currentMillis;

      // Calling weather update function
      updateWeather();
      logToGoogleSheet(updated, temperature, humidity, weather);
      sendWeatherUpdate(png_icon, temperature, humidity, weather, updated);
    }
  }
  // delay(30000);
}

String getFormattedTime(String format_type)
{
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  char buf[30];

  if (String(format_type) == "short")
  {
    strftime(buf, sizeof(buf), "%d-%b-%Y %H:%M:%S", timeinfo);
  }
  else
  {
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", timeinfo);
  }

  return String(buf);
}

void updateWeather()
{
  HTTPClient http;

  // Construct URL
  String url = "http://api.openweathermap.org/data/2.5/weather?lat=";
  url += LATITUDE;
  url += "&lon=";
  url += LONGITUDE;
  url += "&appid=";
  url += API_KEY;
  url += "&units=metric"; // Get temperature in Celsius

  Serial.println(url);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("sending request to get the latest data...");
  display.display();

  http.begin(url);
  int httpCode = http.GET(); // send the request

  if (httpCode == 200)
  {
    display.println("\nrequest succeed");
    display.display();
    delay(1000);

    String payload = http.getString();
    Serial.println(payload); // Debug: print raw JSON

    // Parse JSON
    JsonDocument doc;                                           // This creates a dynamic memory container (doc) that will hold the parsed JSON data in RAM.
    DeserializationError error = deserializeJson(doc, payload); // This parses (deserializes) the JSON string stored in payload and loads it into doc.

    if (!error)
    {
      temperature = doc["main"]["temp"];
      humidity = doc["main"]["humidity"];
      weather = doc["weather"][0]["main"].as<String>();

      // Display on OLED
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.print("Weather in ");
      display.println(String(CITY));
      display.println("---------------------");
      display.setTextSize(1);
      display.print("Temp: ");
      display.print(temperature);
      display.println(" C");
      display.print("Humidity: ");
      display.print(humidity);
      display.println(" %");
      // --------code for bitmap icon - start---------------------------------
      // getting current coordinates for the next line
      uint16_t y;
      y = display.getCursorY();
      String label = "Condition: ";
      label += String(weather);
      display.print(label);

      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(label, 0, y, &x1, &y1, &w, &h);

      if (String(weather) == "Clear")
      {
        png_icon = CLEAR_SKY_ICON;
        display.drawBitmap(x1 + w + 3, y - int(h / 2), sun_bitmap_icon, 16, 16, WHITE);
      }
      else if (String(weather) == "Clouds")
      {
        png_icon = CLOUD_ICON;
        display.drawBitmap(x1 + w + 3, y - int(h / 2), cloud_bitmap_icon, 16, 16, WHITE);
      }
      else if (String(weather) == "Rain")
      {
        png_icon = RAIN_ICON;
        display.drawBitmap(x1 + w + 3, y - int(h / 2), rain_bitmap_icon, 16, 16, WHITE);
      }
      else
      {
        // display.setCursor(100, 0);
        display.print("?");
        png_icon = UNKNOWN_ICON;
      }
      // --------code for bitmap icon - end---------------------------------
      display.print("\n\nLast updated: ");
      updated = getFormattedTime("short");
      display.print(getFormattedTime());
      display.display();
    }
    else
    {
      Serial.println("Failed to parse JSON");
    }

    doc.clear(); // clears memory used by doc
  }
  else
  {
    display.print("error in HTTP request: ");
    display.println(httpCode);
    display.display();
  }

  http.end(); // Free resources
}