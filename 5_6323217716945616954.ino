#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <IRsend.h>
#include <IRremoteESP8266.h>
#include <ir_Daikin.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   

#ifndef UNIT_TEST
#endif

#define SUHURUANG A0


#define IR_LED 4 //esp recomended D2
IRDaikinESP daikinir(IR_LED);


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "iot_uh"
#define AIO_KEY         "50bccd4956004d448c9a2c50654af887"
/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish suhuruang = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/suhuruang");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

Adafruit_MQTT_Subscribe suhu = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/suhu");

Adafruit_MQTT_Subscribe modeac = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/modeac");
/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {

  Serial.begin(115200);
  delay(10);
  daikinir.begin();
  Serial.println(F("Connenct to"));
  // Connect to WiFi access point.
  //Serial.println(); Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(WLAN_SSID);
  WiFiManager wifiManager;
  //WiFi.begin(WLAN_SSID, WLAN_PASS);
  //while (WiFi.status() != WL_CONNECTED) {
   // delay(500);
   // Serial.print(".");
  //}
  Serial.println();
  wifiManager.autoConnect("ConnectAP");
 Serial.println("WiFi connected");
 Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&suhu);
  mqtt.subscribe(&modeac);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("AC "));
      Serial.println((char *)onoffbutton.lastread);
      
      if (strcmp((char *)onoffbutton.lastread, "ON") ==0) {
          daikinir.on();
          daikinir.setFan(1);
          daikinir.setMode(DAIKIN_COOL);
          daikinir.setTemp(25);
          daikinir.setSwingVertical(false);
          daikinir.setSwingHorizontal(false);
          Serial.println("AC ON");
           }
      if (strcmp((char *)onoffbutton.lastread, "OFF") ==0) {
          daikinir.off();

          Serial.println("AC OFF");
        }

    }
    if (subscription == &suhu) {
      String value = (char *) suhu.lastread;
      Serial.print(F("SUHU: "));
      Serial.println((char *)suhu.lastread);
      daikinir.setTemp(value.toFloat());
      
  
      }
    if (subscription == &modeac) {
      Serial.print(F("MODE: "));
      Serial.println((char *)modeac.lastread);
      if (strcmp((char *)modeac.lastread, "0") ==0) {
          daikinir.setMode(DAIKIN_FAN);
           }
      
      if (strcmp((char *)modeac.lastread, "1") ==0) {
          daikinir.setMode(DAIKIN_COOL);
           }
      if (strcmp((char *)modeac.lastread, "2") ==0) {
          daikinir.setMode(DAIKIN_HEAT);
           }
      if (strcmp((char *)modeac.lastread, "3") ==0) {
          daikinir.setMode(DAIKIN_HEAT);
           }
    }
         #if SEND_DAIKIN
   daikinir.send();
   #endif  // SEND_DAIKIN
  }
  

  // Now we can publish stuff!
  int LDR = analogRead(SUHURUANG);
  
  Serial.print(F("\nSending suhuruang val "));
  Serial.print(LDR);
  Serial.print("...");
  if (! suhuruang.publish(LDR)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  
  
  Serial.println("MQTT Connected!");
}
