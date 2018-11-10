

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string>

// defines pins numbers
const int ECHO_PIN = 2; //D4

const int trigPin0 = 16; //D0
const int trigPin1 = 5; //D1
const int trigPin2 = 4; //D2
const int trigPin3 = 0; //D3
const int trigPin5 = 14; //D5
const int trigPin6 = 12; //D6

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
char msg[50];
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
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);

  pinMode(trigPin0, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(trigPin1, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(trigPin2, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(trigPin3, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(trigPin5, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(trigPin6, OUTPUT); // Sets the TRIG_PIN as an Output
  pinMode(ECHO_PIN, INPUT);  // Sets the echoPin as an Input

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

long getDistance(int trigPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the TRIG_PIN on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);

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
  if (now - lastMsg > 1000)
  {
    lastMsg = now;
    ++value;
    //Serial.print("Publish message: ");
    long dist = getDistance(trigPin0);
    snprintf(msg, 50, "0 dist: %ld: %ld", value, dist);
    dist = getDistance(trigPin1);
    snprintf(msg, 50, "1 dist: %ld: %ld", value, dist);
    dist = getDistance(trigPin2);
    snprintf(msg, 50, "2 dist: %ld: %ld", value, dist);
    dist = getDistance(trigPin3);
    snprintf(msg, 50, "3 dist: %ld: %ld", value, dist);
    Serial.println(msg);
    client.publish("hello_world_arduino", msg);
  }
}
