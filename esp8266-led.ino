/**********************************************
* 
* Program : Web Server Kontrol 1 LED
* https://github.com/arjunapanji21/esp8266-led
* 
* *********************************************/

// import library WiFi Esp
#include <WiFiEspServer.h>
#include "WiFiEsp.h"
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>

// import library komunikasi serial
#include <SoftwareSerial.h>

// import library lcd
#include <LiquidCrystal_I2C.h>

SoftwareSerial Serial1(8, 9); // RX, TX pada ESP8266
LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd yang digunakan yaitu 16x2

char ssid[] = "Ursi Aulia"; // nama wifi
char pass[] = "aulia123"; // password wifi

int status = WL_IDLE_STATUS; // status server

WiFiEspServer server(80); // mengaktifkan port 80
WiFiEspClient client; // inisialisasi ethernet client object
RingBuffer buf(8);

#define LED1 10 // pin LED pada ESP8266
int ledStatus = LOW; // status awal led yaitu LOW alias Off

void setup(){
  lcd.init(); // inisialisasi LCD
  lcd.backlight(); // mengaktifkan lampu lcd
  lcd.setCursor(0,0); // memposisikan cursor lcd
  lcd.print("SSID: "); 
  lcd.print(ssid); // menampilkan nama wifi kedalam lcd

  pinMode(LED1, OUTPUT); // menetapkan pin pada variable LED1 sebagai OUTPUT
  digitalWrite(LED1, LOW); // memastikan lampu led pada kondisi off ketika program baru jalankan

  Serial.begin(9600); // mengaktifkan Serial Monitor
  Serial1.begin(9600); // mengaktifkan komunikasi serial

  WiFi.init(&Serial1); // inisialisasi modul WiFi

  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println("WiFi Shild tidak tersedia"); // jika WiFi Shield tidak ada
    lcd.setCursor(0,1);
    lcd.print("Tidak terhubung");
    while(true); //?
  }

  while (status != WL_CONNECTED) {
    Serial.print("Menghubungkan ke SSID: "); // jika belum terhubung
    Serial.println(ssid);
    lcd.setCursor(0,1);
    lcd.print("Menghubungkan...");
    status = WiFi.begin(ssid, pass);
  }

  if(status == WL_CONNECTED){
    Serial.println("Berhasil terhubung ke jaringan"); // jika sudah terhubung    
  }

  printWifiStatus();

  //lcd.clear();

  server.begin(); // mulai menjalankan server
}

void loop(){
  client = server.available();
  if(client){
    Serial.println("Client Baru Terhubung");
    buf.init();
    while(client.connected()){
      if(client.available()){
        char c = client.read();
        buf.push(c);
        if(buf.endsWith("\r\n")){
          sendHttpResponse(client);
          break;
        }
        lcd.setCursor(0,1);
        if (buf.endsWith("GET /H1")) {
          digitalWrite(LED1, HIGH); 
          lcd.print("LED 1 ON ");
        }
        if (buf.endsWith("GET /L1")) {
          digitalWrite(LED1, LOW);
          lcd.print("LED 1 OFF");
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}

void sendHttpResponse(WiFiEspClient client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("");
  client.println("<hr/><hr>");
  client.println("<h4><center> IoT Starter Kit Arduino : Kontrol</center></h4>");
  client.println("<hr/><hr>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("LED 1");
  client.println("<a href=\"/H1\"><button>Turn On </button></a>");
  client.println("<a href=\"/L1\"><button>Turn Off </button></a><br />");
  client.println("</center>");
  client.println("<br><br>");
  client.println("<center>");
  if (digitalRead(LED1)) {
    client.print("<td>LED 1 = ON</td>");
  }
  else {
    client.print("<td>LED 1 = OFF</td>");
  }
  client.println();
}
void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
  lcd.setCursor(0, 1);
  lcd.print("Terhubung");
  delay(2000);
  lcd.clear();
  lcd.print("Alamat IP");
  lcd.setCursor(0, 1);
  lcd.print(ip);
  delay(2000);
}
