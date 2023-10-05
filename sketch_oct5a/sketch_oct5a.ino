/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6IADa1to7"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "ptXTLNz5XUWH0UKN66Vhi5jfVblkokcR"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "DHT.h"
#define DHTPIN 17
#define DHTTYPE DHT22


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Topsan iPhone";
char pass[] = "00000000";

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);

const int LED_1 = 25;
const int LED_2 = 26;
const int LED_3 = 27;
// This function is called every time the Virtual Pin 0 state changes
//ดึงค่าลงมา
BLYNK_WRITE(V5) {
  int value = param.asInt();
  // Update state // อัพเดทค่า
  Blynk.virtualWrite(V2, value);
  digitalWrite(LED_1,value);
}
BLYNK_WRITE(V6) {
  int value = param.asInt();
  // Update state // อัพเดทค่า
  Blynk.virtualWrite(V3, value);
  digitalWrite(LED_2,value);
}

BLYNK_WRITE(V7) {
  int value = param.asInt();
  // Update state // อัพเดทค่า
  Blynk.virtualWrite(V4, value);
  digitalWrite(LED_3,value);
}

// This function is called every time the device is connected to the Blynk.Cloud
// ทำงานอัตโนมัติ
BLYNK_CONNECTED() {
  // Change Web Link Button message to "Congratulations!"
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

    //อัพค่าขึ้น byling
  Blynk.virtualWrite(V0,t);
  Blynk.virtualWrite(V1,h);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));

}

void setup() {
  // Debug console
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  // ช่วงเวลาทำงาน
  timer.setInterval(3000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
  // อย่าใส่ delay
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
