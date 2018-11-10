

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <iomanip>
#include <string>
#include <sstream>
#include <map>

#define PAYLOAD_LEN 50

#define DOOR_SWITCH_PIN 2  // D4

// Maps sensor number --> Arduino-GPIO trigger pin
// IMPORTANT: use Dx as the port (comment)
std::map<int, int> trigPinMap = {
  {0, 16},  // D0
  {1, 5},   // D1
  {2, 4},   // D2
  {3, 0},   // D3
};

// Maps sensor number --> Arduino-GPIO echo pin
// IMPORTANT: use Dx as the port (comment)
std::map<int, int> echoPinMap = {
  {0, 14},  // D5
  {1, 12},  // D6
  {2, 13},  // D7
  {3, 15}   // D8
};

// defines variables
long duration;
int distance;

// Update these with values suitable for your network.

const char *ssid = "AndroidAP4656";
const char *password = "Moments2018";
const char *mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[PAYLOAD_LEN];
int value = 0;

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("hello_world_arduino", "hello world");
      // ... and resubscribe
      // client.subscribe("hello_world_arduino");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output

  pinMode(DOOR_SWITCH_PIN, INPUT);

  for (const auto& pair : trigPinMap) {
    pinMode(pair.second, OUTPUT);
  }

  for (const auto& pair : echoPinMap) {
    pinMode(pair.second, INPUT);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

long getDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the TRIG_PIN on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;
  return distance;
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 500)
  {
    lastMsg = now;
    ++value;
    //Serial.print("Publish message: ");
    String payload = "AF_00";
    payload += "_" + String(digitalRead(DOOR_SWITCH_PIN));
    payload += "_" + String(trigPinMap.size());
    for (int i = 0; i < trigPinMap.size(); ++i) {
      long dist = getDistance(trigPinMap[i], echoPinMap[i]);
      char paddedValue[5];
      sprintf(paddedValue, "%04d", dist);
      payload += "_" + String(paddedValue);
    }
   Serial.println(payload);
   payload.toCharArray(msg, PAYLOAD_LEN);
   client.publish("ultrasonic", msg);
  }
}
