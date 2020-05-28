#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <DHTesp.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h> 
DHTesp dht;
SimpleTimer timer;
char auth[] = "xxx";
char ssid[] = "xxx";
char pass[] = "xxx";
float t;
float h;
int l;


void setup()
{
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    dht.setup(D5, DHTesp::DHT11);
    timer.setInterval(2000, sendUptime);
}

void loop()
{
  Blynk.run();
  timer.run();
}

void sendUptime()
{
  h = dht.getHumidity();
  t = dht.getTemperature(); 
  l = analogRead(A0);
  
  Serial.print("H = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("T = ");
  Serial.print(t); 
  Serial.print("C  ");
  Serial.print("Light = ");
  Serial.print(l, DEC);
  Serial.print(" \n");
  
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h); 
  Blynk.virtualWrite(V2, l); 
}
