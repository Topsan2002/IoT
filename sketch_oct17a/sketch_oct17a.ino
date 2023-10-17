/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6IADa1to7"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "p1uhWzZrLNmZQzK6EufEAEuv-Vto7W73"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <TridentTD_LineNotify.h>
#include <BlynkSimpleEsp32.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include "time.h"
#include <HTTPClient.h>




#define DHTPIN 17
#define DHTTYPE DHT22
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SetzeroDev-2.4G";
char pass[] = "51552105315";
unsigned long myChannelNumber = 2308003;
const char* myWriteAPIKey = "WBRTPQJSYRCKZSNU";                                              //API KEY
String GAS_ID = "AKfycbzN2Jeif_lDAY2ujRzDxnE5_cC6lCNHQW-QGvhoSJEvfNobK7n0Wt0OOXXPuN9hKGLw";  //--> spreadsheet script ID
const char* host = "script.google.com";
const int httpsPort = 443;

#define LINE_TOKEN "3TduxHSZtpKT4MWoXDq6vUXi9Xqp6m6Og0Fu0DK8kvf"

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
WiFiClientSecure client_send;


const int LED_1 = 25;
const int LED_2 = 27;





BLYNK_WRITE(V5)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V2, value);
  digitalWrite(LED_1,value);

}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED() {


  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

void myTimerEvent() {


  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  client_send.setInsecure();
  // sendData(t, h);

  if(t<25){
  digitalWrite(LED_2,1);

  }else{
  digitalWrite(LED_2,0);

  }
  sendSheet(t, h);
  LINE.notify("อุณหภูมิห้อง" + String(t) + "°C ");
  Serial.println(t);
}


void setup() {

  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(12000L, myTimerEvent);
  dht.begin();

  WiFi.mode(WIFI_STA);
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  ThingSpeak.begin(client);
  client_send.setInsecure();
  LINE.setToken(LINE_TOKEN);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSheet(float t, float h) {
  String urlFinal = "https://script.google.com/macros/s/" + GAS_ID + "/exec?" + "&temp=" + String(t) + "&humi=" + String(h);
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

void sendData(float value, float value2) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  //----------------------------------------Connect to Google host
  if (!client_send.connect(host, httpsPort)) {

    Serial.println("connection failed");
    return;
  }
  //----------------------------------------
  //----------------------------------------Proses dan kirim data
  float string_temp = value;
  float string_humi = value2;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi;
  // 2 variables
  Serial.print("requesting URL: ");
  Serial.println(url);
  client_send.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP32\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
  //----------------------------------------
  //---------------------------------------
  while (client_send.connected()) {
    String line = client_send.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client_send.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp32/Arduino CI successfull!");
  } else {
    Serial.println("esp32/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}