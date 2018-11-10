#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <FastLED.h>
#define LED_PIN     2
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];

int pattern_gap = 5;
String _pattern = "";

int count = 0;
int last = 0;
int last_alarm = 0;


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
  Serial.println(topic);

  set_pattern(String((char*)payload));

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
      // client.publish("hello_world_arduino", "hello world");
      // ... and resubscribe
      client.subscribe("led");
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
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  FastLED.show();
  delay(1);
}

void set_pattern(String pattern) {
  Serial.println(pattern);
    _pattern = pattern;
    if(pattern.startsWith("cola")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            if(i % (pattern_gap*2) < pattern_gap) {
                leds[i] = CRGB(255, 0, 0);
            } else {
                leds[i] = CRGB(0, 0, 255);
            }
        }
    } else if(pattern.startsWith("diet")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            if(i % (pattern_gap*2) < pattern_gap) {
                leds[i] = CRGB(174, 226, 255);
            } else {
                leds[i] = CRGB(237, 237, 237);
            }
        }
    } else if(pattern.startsWith("classic")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            if(i % (pattern_gap*2) < pattern_gap) {
                leds[i] = CRGB(0, 0, 255);
            } else {
                leds[i] = CRGB(237, 237, 237);
            }
        }
    } else if(pattern.startsWith("Nikolas")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(32, 255, 0);
        }
    } else if(pattern.startsWith("Michael")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(244, 65, 235);
        }
    } else if(pattern.startsWith("George")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(255, 170, 0);
        }
    } else if(pattern.startsWith("alarm")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(255, 0, 0);
        }
    } else if(pattern.startsWith("dark")) {
        for(int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(100, 100, 100);
        }
    }
}
