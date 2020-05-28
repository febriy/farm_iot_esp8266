//-----------------------------------------------
//This sketch is combined from Adafruit DHT sensor and tdicola
//for dht.h library
//Along with some  esp8266 NodeMCU specifics from teos009
// https://learn.adafruit.com/dht/overview
// https://gist.github.com/teos0009/acad7d1e54b97f4b2a88
//other Authors Arduino and associated Google Script:
//Aditya Riska Putra
//Ahmed Reza Rafsanzani
//Ryan Eko Saputro
//See Also
//http://jarkomdityaz.appspot.com/
//
//ELINS UGM
//
//Modified for Hackster.io project to the ESP8266 from
//a Ethernet Sheild project
//by Stephen Borsay
//Since Arduino can't handle https, we need to use Pushingbox API
//(uses http)to run
//the Google Script (uses https).


#include <ESP8266WiFi.h>
#include <DHTesp.h>
DHTesp dht;

const char host[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "xxx"; //device ID from Pushingbox

const char MY_SSID[] = "xxx";
const char MY_PWD[] =  "xxx";


void setup()
{
  Serial.begin(115200);
  dht.setup(D5, DHTesp::DHT11);
  Serial.print("Connecting to " + *MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  Serial.println("going into wl connect");

  while (WiFi.status() != WL_CONNECTED) //not connected,..waiting to connect
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("wl connected");
  Serial.println("");
  Serial.println("Credentials accepted! Connected to wifi\n ");
  Serial.println("");
}


void loop()
{

  //Wait between measurements longer then normal to slow donwn
  //google sheet populate, as We dont want to exceed free service quota
  delay(120000); //2 minutes, (sampling rate vs. service call quota)

  float humidityData = dht.getHumidity();
  // Read temperature as Celsius (the default)
  float celData = dht.getTemperature();
  // Convert to Fahrenheit
  float fehrData = (celData*9/5) + 32;

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidityData) || isnan(celData) || isnan(fehrData))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hicData = dht.computeHeatIndex(celData, humidityData, false);
  // Compute heat index in Fahrenheit (the default)
  float hifData = dht.computeHeatIndex(fehrData, humidityData);

  //Print to Serial monitor or Terminal of your chocice at 115200 Baud
  Serial.print("Humidity: ");
  Serial.print(humidityData);
  Serial.print(" %\t");
  Serial.print("Temperature in Cel: ");
  Serial.print(celData);
  Serial.print(" *C ");
  Serial.print("Temperature in Fehr: ");
  Serial.print(fehrData);
  Serial.print(" *F\t");
  Serial.print("Heat index in Cel: ");
  Serial.print(hicData);
  Serial.print(" *C ");
  Serial.print("Heat index in Fehr: ");
  Serial.print(hifData);
  Serial.print(" *F\n");

  WiFiClient client;  //Instantiate WiFi object

  //Start or API service using our WiFi Client through PushingBox
  if (client.connect(host, 80))

  {
    String url = "/pushingbox?devid=" + devid
                 + "&humidityData=" + (String) humidityData
                 + "&celData="      + (String) celData
                 + "&fehrData="     + (String) fehrData
                 + "&hicData="      + (String) hicData
                 + "&hifData="      + (String) hifData;

    // Serial.print("Requesting URL: " + url);

    client.print(String("GET ")
                 + url
                 + " HTTP/1.1\r\n"
                 + "Host: " + host + "\r\n"
                 + "Connection: close\r\n\r\n"
                );

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      Serial.print("Data Sent!");
    }

    Serial.println();
    Serial.println("closing connection");
  }
}

// https://www.hackster.io/detox/transmit-esp8266-data-to-google-sheets-8fc617#code
// https://gist.github.com/C4psL0cks/b196b8176a6b119d169985798f2c9bca
// https://esp8266-shop.com/blog/home-security-motion-detection-and-notification-on-smartphone/
