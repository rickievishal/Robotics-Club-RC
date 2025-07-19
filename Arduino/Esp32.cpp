#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

// === LED Matrix Setup ===
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define CS_PIN 5

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX *matrix = display.getGraphicObject();

// === WiFi Setup ===
const char* ssid = "ESP32-LED";
const char* password = "12345678";
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// === Animation Data ===
#define MAX_FRAMES 20
uint8_t customFrames[MAX_FRAMES][8];
int totalCustomFrames = 0;
int currentFrame = 0;
unsigned long lastUpdate = 0;
const int frameDelay = 250; // ms

// === CORS Support ===
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204); // No Content
}

// === POST Frame Handler ===
void handleFramesPost() {
  server.sendHeader("Access-Control-Allow-Origin", "*");

  if (server.method() == HTTP_OPTIONS) {
    handleOptions();
    return;
  }

  if (server.hasArg("plain") == false) {
    server.send(400, "application/json", "{\"error\":\"Missing body\"}");
    return;
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  JsonArray frames = doc.as<JsonArray>();
  if (frames.isNull()) {
    server.send(400, "application/json", "{\"error\":\"Expected an array of frames\"}");
    return;
  }

  int count = 0;
  for (JsonArray frame : frames) {
    if (count >= MAX_FRAMES || frame.size() != 8) break;
    for (int i = 0; i < 8; i++) {
      uint8_t rowByte = 0;
      JsonArray row = frame[i];
      for (int j = 0; j < 8; j++) {
        rowByte |= (row[j].as<int>() ? 1 : 0) << (7 - j); // MSB first
      }
      customFrames[count][i] = rowByte;
    }
    count++;
  }

  totalCustomFrames = count;
  currentFrame = 0;

  server.send(200, "application/json", "{\"success\":true}");
}

void setup() {
  Serial.begin(115200);

  // WiFi Access Point Setup
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  Serial.println("Access Point started");
  Serial.println(WiFi.softAPIP());

  // LED Matrix Init
  display.begin();
  display.setIntensity(5);
  display.displayClear();

  // Web Server Routes
  server.on("/frames", HTTP_OPTIONS, handleOptions);
  server.on("/frames", HTTP_POST, handleFramesPost);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  unsigned long now = millis();
  if (now - lastUpdate >= frameDelay && totalCustomFrames > 0) {
    lastUpdate = now;

    for (uint8_t row = 0; row < 8; row++) {
      matrix->setRow(0, row, customFrames[currentFrame][row]);
    }

    currentFrame = (currentFrame + 1) % totalCustomFrames;
  }

  display.displayAnimate();  // Keeps the matrix driver updated
}
