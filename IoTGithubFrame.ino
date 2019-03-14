/**
   File:    IoTGitHubFrame.h
   Author:  Paweł Skórka <skorkapawel@gmail.com>
   Licnece: GNU General Public License
   URL:     https://github.com/sqra/IoTGitHubFrame
*/

#include "EEPROM.h"
#include "WiFiManager.h"
#include "HTTPClient.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"
#include "TM1637Display.h"
#include "ESP32httpUpdate.h"
#include "time.h"
#include "cacert.h"
#include "update.h"
#include "texts.h"

#define EEPROM_SIZE 512

bool firstRun = true;
int starsLast = 0;
int stars = 0;
int watchers = 0;
int forks = 0;
int starsToday;
int starsNew = 0;
int NumStep = 0;
String user = "";
String repository = "";
String base_url = "https://api.github.com/repos/";
const int requestInterval = 90; // (in seconds) Carefully! GitHub RateLimit -> 60 requests per hour.
char errorCode;
boolean mustReset = true;
const int CLK_BIG = 17;             // TX2
const int DIO_BIG = 5;              // D5
const int CLK_SMALL = 4;            // D4
const int DIO_SMALL = 16;           // RX2
const int RED_LED = 22;             // D22
const int GREEN_LED = 23;           // D23
const int BLUE_LED = 32;            // D32
const int STARS_LED = 18;           // D18
const int WATCHERS_LED = 19;        // D19
const int FORKS_LED = 21;           // D21
const int BUILD_IN_LED = 2;         // NO PIN
const int BUTTON_IP = 27;           // D27
const int BUTTON_RESET_TODAY = 15;  // D15
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
char arrayToStoreUser[50];
char arrayToStoreRepo[50];
AsyncWebServer server(80);
TM1637Display largeDisplay(CLK_BIG, DIO_BIG);
TM1637Display smallDisplay(CLK_SMALL, DIO_SMALL);
TaskHandle_t Task1;
TaskHandle_t Task2;

void powerLED(String color)
{
  if (color == "RED")
  {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
  }
  else if (color == "GREEN")
  {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
  }
  else if (color == "BLUE")
  {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
  }
}

String getLocalTime() {
  // return time now as String( hh:mm )
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    String respond = "Failed to obtain time";
    return respond;
  }
  char output[80];
  strftime(output, 80, "%H:%M", &timeinfo);
  String time_str = String(output);
  return time_str;
}

void periodReset() {
  // check if it's after midnight
  String time_now = getLocalTime();
  if (time_now == "00:00" && mustReset) {
    resetTodayStars();
    checkForUpdates();
    mustReset = false;
  } else {
    mustReset = true;
  }
}

void resetTodayStars() {
  // reset today's stars and store in EEPROM
  starsToday = 0;
  EEPROM.write(12, 0);
  EEPROM.commit();
  Serial.println("Today's stars have been reset.");
  delay(100);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  // switch to AP mode
  powerLED("BLUE");
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void connectWIFI() {
  // connect to wireless network
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(120);
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("GITHUB-FRAME")) {
    ESP.restart();
    delay(1000);
  }

  Serial.println("Connected to the WiFi network");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime();
  checkForUpdates();
  powerLED("GREEN");
  delay(100);
}

void displayIpAddress() {
  // show IP address on large display
  IPAddress frameIP = WiFi.localIP();
  Serial.print("IP Address on demand: ");
  Serial.println(WiFi.localIP());
  int ip0 = (int)frameIP[0];
  int ip1 = (int)frameIP[1];
  int ip2 = (int)frameIP[2];
  int ip3 = (int)frameIP[3];
  largeDisplay.setSegments(clearMe);
  largeDisplay.setSegments(ipText, 2, 2);
  delay(2000);
  largeDisplay.showNumberDec(ip0);
  delay(2000);
  largeDisplay.showNumberDec(ip1);
  delay(2000);
  largeDisplay.showNumberDec(ip2);
  delay(2000);
  largeDisplay.showNumberDec(ip3);
  delay(2000);
}

void setup() {
  // initialize
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode( RED_LED, OUTPUT );
  pinMode( GREEN_LED, OUTPUT );
  pinMode( BLUE_LED, OUTPUT);
  pinMode( BUILD_IN_LED, OUTPUT);
  pinMode( STARS_LED, OUTPUT);
  pinMode( WATCHERS_LED, OUTPUT);
  pinMode( FORKS_LED, OUTPUT);
  pinMode( BUTTON_IP, INPUT);
  pinMode( BUTTON_RESET_TODAY, INPUT);
  powerLED("RED");
  starsToday = EEPROM.read(12);
  user = EEPROM.get(15, arrayToStoreUser);
  repository = EEPROM.get(50, arrayToStoreRepo);
  Serial.println((String)"[EPR] User: " + user);
  Serial.println((String)"[EPR] Repository: " + repository);
  Serial.println((String)"[EPR] Stars today: " + starsToday);
  connectWIFI();
  largeDisplay.setBrightness(0x0F);
  smallDisplay.setBrightness(0x0F);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Client connected.");
    if (request->hasParam("user") && request->hasParam("repository")) {
      AsyncWebParameter* param1 = request->getParam("user");
      AsyncWebParameter* param2 = request->getParam("repository");
      Serial.println((String)"User: " + param1->value());
      Serial.println((String)"Repository: " + param2->value());
      String val_1 = String(param1->value());
      String val_2 = String(param2->value());
      val_1.toCharArray(arrayToStoreUser, val_1.length() + 1);
      val_2.toCharArray(arrayToStoreRepo, val_2.length() + 1);
      EEPROM.put(15, arrayToStoreUser);
      EEPROM.put(50, arrayToStoreRepo);
      user = EEPROM.get(15, arrayToStoreUser);
      repository = EEPROM.get(50, arrayToStoreRepo);
      resetTodayStars();
      request->send(
        200,
        "text/html",
        "<!DOCTYPE html>\n<html style=\"background-color:#446f94\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "<body style=\"text-align:left;margin:0 auto;color:white;font-family:sans-serif;font-size:14px;padding:20px;\">\n\n"
        "<div style=\"padding:20px\">"
        "<p style=\"text-align:center;margin-top:0\">Configuration has been saved successfully. Restarting device ...</p>\n"
        "</div>\n"
        "</body>\n"
        "</html>\n"
      );
      Serial.println("Restarting ESP after 3 seconds");
      delay(3000);
      request->redirect("/");
      delay(500);
      ESP.restart();
    }
    request->send(
      200,
      "text/html",
      "<!DOCTYPE html>\n<html style=\"background-color:#446f94\">\n"
      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
      "<body style=\"text-align:left;margin:0 auto;color:white;font-family:sans-serif;font-size:14px;padding:20px;\">\n\n"
      "<div style=\"padding:20px\">"
      "<h2 style=\"text-align:center;margin-bottom:0\">GitHub Frame</h2>\n"
      "<p style=\"text-align:center;margin-top:0\">configuration</p>\n"
      "<form action=\"\" method=\"get\" enctype=\"multipart/form-data\">\n"
      "<label style=\"display:block;margin-bottom:10px\">User: </label>\n"
      "<input style=\"display:block;margin-top:6px;margin-bottom:16px;resize:vertical;width:100%;padding:10px;border:1px solid #ccc;border-radius: 4px;box-sizing: border-box;\" placeholder=\"i.e. DivanteLtd\" type=\"text\" id=\"user\" name=\"user\" value=\"" + user + "\" required/>\n<br/>\n"
      "<label style=\"display:block;margin-bottom:10px\">Repository name: </label>\n"
      "<input style=\"display:block;margin-top:6px;margin-bottom:16px;resize:vertical;width:100%;padding:10px;border:1px solid #ccc;border-radius: 4px;box-sizing: border-box;\" placeholder=\"i.e. vue-storefront\" type=\"text\" id=\"repository\" name=\"repository\" value=\"" + repository + "\" required/>\n<br/>\n"
      "<input type=\"submit\" style=\"cursor:pointer;border:none;border-radius:5px;background-color:#375e80;color:white;padding:13px;font-size:18px\" value=\"Save configuration\">\n"
      "</form>\n\n\n\n"
      "</div>\n"
      "</body>\n"
      "</html>\n"
    );
  });

  server.onNotFound(notFound);

  server.begin();
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1,  &Task1, 0);
  xTaskCreatePinnedToCore( Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
}

void notFound(AsyncWebServerRequest *request) {
  // respond when the user gets lost
  request->send(404, "text/plain", "What are you looking for here? It's just a simple frame.");
}

void todayStars() {
  // calculate today's stars
  if (firstRun) {
    starsLast = stars;
    firstRun = false;
  }
  if (stars != starsLast) {
    starsNew = stars - starsLast;
    starsToday += starsNew;
    starsNew = 0;
    starsLast = stars;
  }
  Serial.println((String)"Local time: " + getLocalTime());
  Serial.println((String)"Today stars: " + starsToday);
  delay(100);
}

void getData() {
  // do a GET request to GitHub API and parse json data
  String url = base_url + user + "/" + repository;
  HTTPClient http;
  http.begin(url, root_ca);
  int httpCode = http.GET();
  Serial.println((String)"Response code: " + httpCode);
  if (httpCode > 0) {
    String payload = http.getString();
    DynamicJsonBuffer JSONBuffer(700);
    JsonObject &parsed = JSONBuffer.parseObject(payload);
    if (!parsed.success()) {
      Serial.println("Parsing failed");
      delay(5000);
      return;
    }
    String dataStars = parsed["stargazers_count"];
    String dataWatchers = parsed["subscribers_count"];
    String dataForks = parsed["forks_count"];
    stars = dataStars.toInt();
    watchers = dataWatchers.toInt();
    forks = dataForks.toInt();
    todayStars();
    Serial.println((String)"Stars: " + stars);
    Serial.println((String)"Watchers: " + watchers);
    Serial.println((String)"Forks: " + forks);
    Serial.println((String)" ");
    digitalWrite( BUILD_IN_LED, HIGH);
    delay(500);
    digitalWrite(BUILD_IN_LED, LOW);
  } else {
    Serial.println(httpCode);
    Serial.println("Error on HTTP request");
  }

  http.end();
}

void Task1code( void * pvParameters ) {
  // core loop 1
  for (;;) {
    getLocalTime();
    while (repository == "" || user == "") {
      Serial.print("Please select 'user' and 'repository'.");
      delay(1000);
    }
    getData();
    delay(requestInterval*1000);
  }
}

void Task2code( void * pvParameters ) {
  // core loop 2
  for (;;) {
    if (WiFi.status() != WL_CONNECTED) {
      powerLED("RED");
      Serial.println("WiFi connection lost. Waiting for signal...");
      delay(10000);
      ESP.restart();
    } else {

      powerLED("GREEN");
      periodReset();

      if (digitalRead(BUTTON_IP)) {
        displayIpAddress();
      } else if (digitalRead(BUTTON_RESET_TODAY)) {
        resetTodayStars();
      } else {
        digitalWrite(FORKS_LED, LOW);
        digitalWrite(STARS_LED, LOW);
        digitalWrite(WATCHERS_LED, HIGH);
        largeDisplay.showNumberDec((int)watchers);
        delay(5000);
      }

      if (digitalRead(BUTTON_IP)) {
        displayIpAddress();
      } else if (digitalRead(BUTTON_RESET_TODAY)) {
        resetTodayStars();
      } else {
        digitalWrite(WATCHERS_LED, LOW);
        digitalWrite(STARS_LED, LOW);
        digitalWrite(FORKS_LED, HIGH);
        largeDisplay.showNumberDec((int)forks);
        delay(5000);
      }

      if (digitalRead(BUTTON_IP)) {
        displayIpAddress();
      } else if (digitalRead(BUTTON_RESET_TODAY)) {
        resetTodayStars();
      } else {
        digitalWrite(FORKS_LED, LOW);
        digitalWrite(WATCHERS_LED, LOW);
        digitalWrite(STARS_LED, HIGH);
        largeDisplay.showNumberDec((int)stars);
        delay(5000);
      }

      smallDisplay.showNumberDec(starsToday);
      EEPROM.write(12, starsToday);
      EEPROM.commit();
    }

  }
}

void loop() {
  // not used loop
}
