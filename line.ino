#include <WiFi.h>
#include <TridentTD_LineNotify.h>
#include "DHT.h"


#define DHTPIN 17 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

#define SSID        "SetzeroDev-2.4G"
#define PASSWORD    "51552105315"
#define LINE_TOKEN  "55kssrCNJodKVwbS2WMoFGsgn0EtJn69A8Qqx0u9RES  "

void setup() {
  Serial.println(F("DHTxx test!"));
  dht.begin();
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);




  // ตัวอย่างส่งข้อความ
  LINE.notify("อุณหภูมิ เกินกำหนด");

  // ตัวอย่างส่งข้อมูล ตัวเลข
//   LINE.notify(2342);          // จำนวนเต็ม
//   LINE.notify(212.43434,5);   // จำนวนจริง แสดง 5 หลัก

//   // เลือก Line Sticker ได้จาก https://devdocs.line.me/files/sticker_list.pdf
//   LINE.notifySticker(3,240);        // ส่ง Line Sticker ด้วย PackageID 3 , StickerID 240
//   LINE.notifySticker("Hello",1,2);  // ส่ง Line Sticker ด้วย PackageID 1 , StickerID 2  พร้อมข้อความ

//   // ตัวอย่างส่ง รูปภาพ ด้วย url
//   LINE.notifyPicture("https://preview.ibb.co/j6G51n/capture25610417181915334.png");
//   LINE.notifyPicture("จตุธาตุ","https://www.fotoaparat.cz/storage/pm/09/10/23/670915_a5351.jpg");
}

void loop() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));


  LINE.notify("อุณหภูมิห้อง"+String(t)+"°C ");

  // delay();
}