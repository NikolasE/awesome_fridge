#! /usr/bin/python

import paho.mqtt.client as mqtt
import voice_api

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    # print("Connected with result code "+str(rc))
    assert isinstance(client, mqtt.Client)
    client.subscribe("text_to_speech")
    client.subscribe("music")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    m = msg.payload.decode('ascii')
    
    if msg.topic == "text_to_speech":
        voice_api.text_to_speech(m)
    if msg.topic == "music":
        
        if m == "stop":
            print ("stopping")
            voice_api.stop_sound()
        if m == "hot":
            print ("hot start")
            voice_api.hot_in_here()
            
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("broker.mqttdashboard.com", 1883)
client.loop_forever()
