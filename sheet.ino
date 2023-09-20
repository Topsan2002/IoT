//Include required libraries
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
#include "DHT.h"


#define DHTPIN 17
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "SetzeroDev-2.4G";  // change SSID
const char* password = "51552105315";  // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbwcmfdPqDQLGj-AJX3XhJpaJ5FGO9HkQaCP_D7qyLvOSkNi987PUTiQK8s0rSDXM3xf";  // change Gscript ID
int count = 0;
void setup() {

  Serial.println(F("DHTxx test!"));
  dht.begin();


  delay(1000);
  Serial.begin(115200);
  delay(1000);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }


    delay(2000);
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }



    char timeStringBuff[50];  //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "date=" + asString + "&temp=" + String(t) + "&hum=" + String(h);
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload: " + payload);
    }
    //---------------------------------------------------------------------
    http.end();
  }
  count++;
  delay(1000);
}
