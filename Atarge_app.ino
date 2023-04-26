//#include <Arduino_JSON.h>
//#include "ESP8266WiFi.h"
//#include <Firebase.h> 
//#include <Firebase.h>  
//#include <ESP8266.h> 
#include <dht11.h>
//#include <DHTesp.h>
#include<Wire.h>
#include <SoftwareSerial.h>

#define calisma_siniri 50
#define DHT 1
String agAdi = "iphone";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "07zlfkrata"; 
int rxPin=10;
int txPin=11;

SoftwareSerial esp(rxPin,txPin);

#define FIREBASE_HOST "https://atarge-app-default-rtdb.firebaseio.com/"                    // Enter the Firebase Database URL Without Https and backslash
#define FIREBASE_AUTH "ND8LvGaC9WJ0CQhluu7E1t4UI2S9glm1RfYysK04"
//#define WIFI_SSID ="SUPERONLINE_WiFi_1088"                     // Change the name of your WIFI
//#define WIFI_PASSWORD ="beypazari.123,4q5" // dht11 kütüphanesini kodlarımıza dahil ediyoruz.
DHT sensorumuz(DHTPIN, DHTTYPE);
int DhtPin = 1;     // DhtPin olarak Dijital 2'yi belirliyoruz.
 dht11 dht_sensor;   // dht_sensor adında bir DHT11 nesnesi oluşturduk.
int toprakpin = A0;
int role = 13;
int cikis_degeri;
int sula= 5000; // Sulama Süresi
int bekle= 3000;



void setup() {
  pinMode(role, INPUT);
  pinMode(13,OUTPUT);
  pinMode(toprakpin, INPUT);
  sensorumuz.begin();
  Serial.begin(9600);  // Seri iletişimi başlatıyoruz.
  esp.begin(115200);
  esp.println("AT");
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);
  

}
void loop() {
 if(toprakpin>calisma_siniri)
 {
   digitalWrite(role,HIGH);  // Röleyi açık konuma getir
 delay(sula);  // 3 saniye bekle
 
 }
 else{
  digitalWrite(role,LOW);  // Röleyi kapalı konuma getir
  delay(bekle);  // 3 saniye bekle
 }





  cikis_degeri = analogRead(toprakpin);

  cikis_degeri = map(cikis_degeri, 1023, 0, 0, 100);

  Serial.print("Toprak Nemi (%): ");
  

 
  

  Serial.print(cikis_degeri);

  float sicaklik = sensorumuz.readTemperature();
  float nem = sensorumuz.readHumidity();
  String veri = "GET https://api.thingspeak.com/update?api_key=N8UK73GHKR47MOXF";                                 //Göndereceğimiz sıcaklık değişkeni
  veri += "&field1=";
  veri += String(sicaklik);
  veri += "&field2=";
  veri += String(nem);     //Göndereceğimiz nem değişkeni
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(60000);

}
