

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <iomanip>
#include <map>
#include <sstream>
#include <string>

#define BOARD_ID "01"
#define MAX_PAYLOAD_LEN 50

// IR switch pin
#define DOOR_SWITCH_PIN 2 // D4

// IMPORTANT: use Dx as the port on the ESP (see comments)
std::array<int, 4> trigPins = {
    16, // D0
    5,  // D1
    4,  // D2
    0,  // D3
};

// IMPORTANT: use Dx as the port on the ESP (see comment)
std::array<int, 4> echoPins = {
    14, // D5
    12, // D6
    13, // D7
    15  // D8
};

// Update these with values suitable for your network.

const char *ssid = "AndroidAP4656";
const char *password = "Moments2018";
const char *mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character.
  if ((char)payload[0] == '1') {
    // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01).
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    // Turn the LED off by making the voltage HIGH.
    digitalWrite(BUILTIN_LED, HIGH);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("hello_world_arduino", "hello world");
      // ... and resubscribe
      // client.subscribe("hello_world_arduino");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(BUILTIN_LED, OUTPUT);

  pinMode(DOOR_SWITCH_PIN, INPUT);

  for (const auto &pin : trigPins) {
    pinMode(pin, OUTPUT);
  }

  for (const auto &pin : echoPins) {
    pinMode(pin, INPUT);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a trigger pulse.
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds.
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in mm.
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Add our sophisticated magic number "AF" and the board ID.
  String payload = "AF_" + String(BOARD_ID);

  // Add the temperature value.
  // TODO: add driver for DHT22
  String temperature = "00";
  payload += "_" + temperature;

  // Add boolean if the door is open.
  payload += "_" + String(digitalRead(DOOR_SWITCH_PIN));

  // Add the number of ultrasonic sensors.
  payload += "_" + String(trigPins.size());

  // Add ultrasonic sensor data.
  for (int i = 0; i < trigPins.size(); ++i) {
    long dist = getDistance(trigPins[i], echoPins[i]);
    char paddedValue[5];
    sprintf(paddedValue, "%04d", dist);
    payload += "_" + String(paddedValue);
  }

  // debug
  Serial.println(payload);

  // Publish the payload.
  char msg[MAX_PAYLOAD_LEN];
  payload.toCharArray(msg, MAX_PAYLOAD_LEN);
  client.publish("fridge_state", msg);

  delay(2000);
}
