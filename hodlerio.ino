#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "SH1106.h"      //https://github.com/ThingPulse/esp8266-oled-ssd1306 use version 4.1.0, 4.2.0 doesn't work for me
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include "images.h"

//config
char ssid[] = "Your_SSID";          // your network SSID (name)
char password[] = "password";  // your network key
String secret_key = "Your-secret-key";
String api_key = "Your-api-key";
String currency = "usd";
const char fingerprint[] PROGMEM = "2D 9D FB DD EA CC C8 0E 0C 5B 11 A0 EA EC 6F 89 E9 14 DC 08"; //sha1 https fingerprint
// Pins based on your wiring
#define SCL_PIN D5
#define SDA_PIN D3

SH1106 display(0x3c, SDA_PIN, SCL_PIN);

String ipAddressString;
String host = "https://api.hodlerio.cz/API/get?api_key=" + api_key + "&mena=" + currency;
const int httpsPort = 443;

void setup() {
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 0, F("hodlerio.cz"));
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 18, F("By Adam Malý"));
  display.drawXbm(32, 30, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  ipAddressString = ip.toString();
}

void loop() {
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  client.setTimeout(15000); // 15 Seconds
  Serial.print("HTTPS Connecting");
  int r = 0; //retry counter
  while ((client.connect(host, httpsPort)) && (r < 5)) {
    delay(100);
    Serial.print(".");
    r++;
  }
  if (r == 5) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
    HTTPClient http;
    http.begin(client, host);
    // Send HTTP GET request
    http.addHeader("Authorization", secret_key);
    int httpResponseCode = http.GET();
    Serial.println("Request sent to: " + host);

    if (httpResponseCode == 200) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      }
      JsonObject root = doc.as<JsonObject>();
      int i = 0;
      float value;
      float percent_change;
      String symbol;
      if (currency == "czk") {
        symbol = "CZK";
      }
      else if (currency == "eur") {
        symbol = "EUR";
      }
      else {
        symbol = "$";
      }
      for (JsonPair kv : root) {
        if (i > 0) {
          String key = kv.key().c_str();
          value = root[key]["hodnota"];
          percent_change = root[key]["24h"];
          display.clear();
          display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.setFont(ArialMT_Plain_16);
          display.drawString(64, 0, key);
          display.setFont(ArialMT_Plain_16);
          if (symbol == "CZK" || symbol == "EUR") {
            display.drawString(64, 18, String(value) + symbol);
          }
          else {
            display.drawString(64, 20, symbol + String(value));
          }
          display.setFont(ArialMT_Plain_10);
          display.drawString(64, 37, String(root[key]["zustatek"]) + key);
          display.setFont(ArialMT_Plain_10);
          display.drawString(34, 50, String(percent_change, 2) + '%');
          if (symbol == "CZK" || symbol == "EUR") {
            display.drawString(90, 50, String(root[key]["kurz"]) + symbol);
          }
          else {
            display.drawString(90, 50, symbol + String(root[key]["kurz"]));
          }
          display.display();
          delay (5000);
        }
        i++;


      }
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(ArialMT_Plain_16);
      display.drawString(64, 0, F("ERROR"));
      display.setFont(ArialMT_Plain_10);
      display.drawString(64, 18, payload);
      display.display();
      delay(5000);
      for (int i = 20; i > 0; i--) {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(ArialMT_Plain_16);
        display.drawString(64, 0, F("Retry after"));
        display.setFont(ArialMT_Plain_10);
        display.drawString(64, 18, (String(i) + " seconds"));
        display.display();
        delay(1000);
      }

    }
  }
}
