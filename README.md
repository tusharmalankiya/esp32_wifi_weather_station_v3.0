
# 🌤️ ESP32 Wi-Fi Weather Station

This project uses an ESP32 microcontroller to measure tempreture, humidity and weather condition from the DHT22 sensors. It also fetches and compares these data from the OpenWeatherMap API and display real-time weather information  on an OLED display. Additionally, the ESP32 functions as a web server, hosting a responsive web-based weather dashboard accessible over the local network.

---

## 📌 Features

- Connects to Wi-Fi
- Fetches current weather data (temperature, humidity, and condition)
- Displays data on a 128x64 OLED screen
- Updates automatically every 30 seconds
- Simple web dashboard which keeps data updated without refreshing the page throguh Server Sent Events
- Log data directly to Google Sheets for every updates
- Measure data through DHT22 and compare them with API data


## 🧰 Tools Used

- [ESP32](https://www.espressif.com/en/products/socs/esp32)
- [Wokwi Simulator](https://wokwi.com/)
- DHT22 Temperature and Humidity Sensor
- VS Code + PlatformIO
- SSD1306 OLED Display (I2C)
- OpenWeatherMap API
- Google Console

## Data Source
Weather data is obtained from OpenWeatherMap and DHT22 onboard sensors.