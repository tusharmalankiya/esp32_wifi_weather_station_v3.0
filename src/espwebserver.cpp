#include "espwebserver.h"

// WebServer server(80);
AsyncWebServer server(80);
AsyncEventSource events("/events"); // SSE endpoint

void handleRoot(AsyncWebServerRequest *request, float temperature, int humidity, String weather, String updated, String png_icon)
{
  Serial.println("home page sent");
  String html = htmlTemplate;
  html.replace("{PNG_ICON}", png_icon);
  html.replace("{TEMP}", String(temperature));
  html.replace("{HUMIDITY}", String(humidity));
  html.replace("{CONDITION}", weather);
  html.replace("{UPDATED}", updated);
  // request->send_P(200, "text/html", html);
  request->send(200, "text/html", html);
}

void sendWeatherUpdate(String png, float temp, int humidity, String condition, String updated_time) {
  String payload = "{\"png\":\"" + png + "\", \"humidity\": \"" + String(humidity) + "\", \"temp\":\"" + String(temp) + "\",\"condition\":\"" + condition + "\",\"updated\":\"" + updated_time + "\"}";
  events.send(payload.c_str(), "weatherUpdate", millis());
}

// void handleCheckUpdate(String updated_time){
//   Serial.println("update checked");
//   server.send(200, "text/plain", updated_time);
// }
