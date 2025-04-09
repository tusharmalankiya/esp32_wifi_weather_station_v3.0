#ifndef ESPWEBSERVER_H
#define ESPWEBSERVER_H

// #include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// extern WebServer server;
extern AsyncWebServer server;
extern AsyncEventSource events;

void handleRoot(AsyncWebServerRequest *request, float temperature, int humidity, String weather, String updated, String png_icon);
// void handleCheckUpdate(String updated_time);
void sendWeatherUpdate(String png, float temp, int humidity, String condition, String updated_time);

const String htmlTemplate = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>ESP32 Weather Station</title>
  <style>
    :root {
      --bg-color: #1e1e2f;
      --card-color: #292940;
      --text-color: #ffffff;
      --accent: #00d4ff;
    }

    body {
      margin: 0;
      padding: 0;
      font-family: "Segoe UI", sans-serif;
      background-color: var(--bg-color);
      color: var(--text-color);
      display: flex;
      align-items: center;
      justify-content: center;
      height: 100vh;
    }

    .weather-card {
      background: var(--card-color);
      padding: 2rem;
      border-radius: 1rem;
      box-shadow: 0 0 10px rgba(0, 212, 255, 0.4);
      text-align: center;
      width: 300px;
    }

    .weather-card h1 {
      font-size: 1.8rem;
      margin-bottom: 0.5rem;
    }

    .icon {
      width: 100px;
      height: 100px;
      margin: 1rem auto;
    }

    .temp {
      font-size: 2.5rem;
      font-weight: bold;
      color: var(--accent);
    }

    .humidity {
      margin-top: 1rem;
      font-size: 1.2rem;
    }

    .condition {
      margin-top: 0.5rem;
      font-size: 1.2rem;
      font-style: italic;
    }

    .last-updated {
      margin-top: 1.5rem;
      font-size: 0.9rem;
      color: #ccc;
    }
  </style>
</head>
<body>
  <div class="weather-card">
    <h1>Current Weather</h1>
    <img class="icon" src="{PNG_ICON}" alt="Weather Icon" />
    <div class="temp">{TEMP}°C</div>
    <div class="humidity">Humidity: {HUMIDITY}%</div>
    <div class="condition">{CONDITION}</div>
    <div class="last-updated">Last Updated: {UPDATED}</div>
  </div>
</body>
</html>

<script>
  const evtSource = new EventSource("/events");

  // const scriptURL = "https://script.google.com/macros/s/AKfycbzAODnYYjxuA5RcThsR96mha9Sn0DtNMZgjf42avk_12WklhtpphMPJjUzvZagWoQ1aOg/exec";

  evtSource.addEventListener("weatherUpdate", function (e) {
    const data = JSON.parse(e.data);
    document.getElementsByClassName("icon")[0].src = data.png;
    document.getElementsByClassName("temp")[0].innerText = data.temp + "°C";
    document.getElementsByClassName("humidity")[0].innerText = "Humidity: " + data.humidity + "%";
    document.getElementsByClassName("condition")[0].innerText = data.condition;
    document.getElementsByClassName("last-updated")[0].innerText = "Last Updated: " + data.updated;

    // fetch(`${scriptURL}?updated=${data.updated}&temp=${data.temp}&humidity=${data.humidity}&condition=${data.condition}`)
    // .then(response => response.text())
    // .then(data => {
    //     console.log("Server Response: ", data);
    // })
    // .catch(error =>{
    //     console.log("Error: ", error);
    // });

});

</script>
)";

#endif