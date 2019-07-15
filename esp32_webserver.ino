#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "vivo 1612";
const char* password = "uh010697";

WebServer server(80);

uint8_t led1 = 2; 
bool led1status = LOW;

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(led1status));
}

void handle_led1on() {
  led1status = HIGH;
  server.send(200, "text/html", SendHTML(led1status));
  Serial.println("LED dinyalakan.");
}

void handle_led1off() {
  led1status = LOW;
  server.send(200, "text/html", SendHTML(led1status));
  Serial.println("LED dimatikan.");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(led1, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  if (led1status) {
    digitalWrite(led1, HIGH);}   // LED1 seharusnya menyala
  else {
    digitalWrite(led1, LOW);}   // LED0 seharusnya menyala
}

String SendHTML(uint8_t led1stat){
  String teks = "<!DOCTYPE html> <html>\n";

  if (led1status) {
    teks += "<h2>Status LED: MENYALA</h2><br>";}
  else {
    teks += "<h2>Status LED: MATI</h2><br>";}

  teks += "<a href=\"/led1on\"\"><button>Nyalakan LED </button></a>";
  teks += "<a href=\"/led1off\"\"><button>Matikan LED </button></a>";

  teks += "</html>";
  
return teks;
}
