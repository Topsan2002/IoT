/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/


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

//set wifi
char ssid[] = "Topsan iPhone";
char pass[] = "00000000";

//det thinkspeak
unsigned long myChannelNumber = 2308003;
const char* myWriteAPIKey = "WBRTPQJSYRCKZSNU"; 


//set token google sheet                                          
String GAS_ID = "AKfycbzN2Jeif_lDAY2ujRzDxnE5_cC6lCNHQW-QGvhoSJEvfNobK7n0Wt0OOXXPuN9hKGLw";  //--> spreadsheet script ID
const char* host = "script.google.com";
const int httpsPort = 443;

//set line token
#define LINE_TOKEN "3TduxHSZtpKT4MWoXDq6vUXi9Xqp6m6Og0Fu0DK8kvf"

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
WiFiClientSecure client_send;

//set led
const int LED_1 = 25;
const int LED_2 = 18;
const int LED_3 = 23;

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
  pinMode(LED_3, OUTPUT);
}

void loop() {

  //รับค่าจาก dht
  delay(10000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // เช็คอุณหภูมิแสดงไฟ
  checkTemp(t,28);
  // ส่งค่าไป thinkspeak
  sendThinkSpeak(t,h);
  // ส่งไป google sheet
  sendSheet(t, h);
  // ส่งไป line
  sendLine(t,h);
  Serial.println(t);
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
  client_send.setInsecure();
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  //----------------------------------------Connect to Google host
  if (!client_send.connect(host, httpsPort)) {

    Serial.println("connection failed");
    return;
  }
  float string_temp = value;
  float string_humi = value2;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi;
  Serial.print("requesting URL: ");
  Serial.println(url);
  client_send.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP32\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
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
}

void sendThinkSpeak(float t, float h){
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void sendBlynk(float t, float h){

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void sendLine(float t, float h){

  LINE.notify("อุณหภูมิห้อง" + String(t) + "°C ");
  LINE.notify("ความชื้น" + String(h) + "% ");

}
void checkTemp(float t, int check){
    if(t > check){
      digitalWrite(LED_1,1);
    }else{
      digitalWrite(LED_1,0);
    }
}
