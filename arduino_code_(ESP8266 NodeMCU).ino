#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>

#define DHTPIN D4
#define DHTTYPE DHT11
#define WATER_SENSOR A0

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram configuration
const char* telegramToken = "YOUR_TELEGRAM_BOT_TOKEN";
const char* chatId = "YOUR_CHAT_ID";
const char* telegramServer = "api.telegram.org";

// Thresholds
const int FLOOD_THRESHOLD = 500;  // Adjust based on sensor calibration
const int HUMIDITY_THRESHOLD = 90;

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  
  connectToWiFi();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int waterLevel = analogRead(WATER_SENSOR);

  // Display readings on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Flood Detection Sys");
  display.printf("Temp: %.1fC\n", temperature);
  display.printf("Hum: %.1f%%\n", humidity);
  display.printf("Water: %d\n", waterLevel);
  display.display();

  // Check flood conditions
  if (waterLevel > FLOOD_THRESHOLD || humidity > HUMIDITY_THRESHOLD) {
    String message = "🚨 Flood Alert! 🚨\n";
    message += "Water Level: " + String(waterLevel) + "\n";
    message += "Humidity: " + String(humidity) + "%\n";
    message += "Temperature: " + String(temperature) + "°C";
    
    sendTelegramAlert(message);
  }

  delay(5000);  // Check every 5 seconds
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void sendTelegramAlert(String message) {
  WiFiClientSecure client;
  client.setInsecure();  // For SSL connection
  
  if (client.connect(telegramServer, 443)) {
    String url = "/bot" + String(telegramToken) + "/sendMessage";
    String payload = "chat_id=" + String(chatId) + "&text=" + message;
    
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + String(telegramServer));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();
    client.println(payload);
    
    delay(10);
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    client.stop();
  }
}
