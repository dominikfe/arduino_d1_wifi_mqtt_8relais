#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Variablen
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
const char* mqtt_intopic = "/polling/heizung/eg/ventile/#";
const char* mqtt_outtopic = "/polling/heizung/eg/ventile/state/";

WiFiClient espclient;
PubSubClient client(espclient);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
//  server.begin();
//  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // prepare MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // prepare GPIO2-9
  pinMode(16, OUTPUT); 
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);  
  
  digitalWrite(16, 1);
  digitalWrite(5, 1);
  digitalWrite(4, 1);
  digitalWrite(14, 1);
  digitalWrite(12, 1);
  digitalWrite(13, 1);
  digitalWrite(0, 1);
  digitalWrite(2, 1);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_outtopic, "hello world");
      // ... and resubscribe
      client.subscribe(mqtt_intopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String pl;
  String mytopic = topic;
  Serial.print("Message arrived [");
  Serial.print(mytopic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    pl=pl+(char)payload[i];
  }
  Serial.println();

// Relais 1 aka GPIO 16
if (mytopic.indexOf("1", 0) >= 1  && pl == "relay10") {
  digitalWrite(16, 0);
  if (digitalRead(16) == 0) client.publish(mqtt_outtopic, "relay10");
}
if (mytopic.indexOf("1", 0) >= 1  && pl == "relay11") {
  digitalWrite(16, 1);
  if (digitalRead(16) == 1) client.publish(mqtt_outtopic, "relay11");
}


}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();



  
}
