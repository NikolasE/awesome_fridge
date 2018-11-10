#! /usr/bin/python

import paho.mqtt.client as mqtt
from random import randint
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("broker.mqttdashboard.com", 1883)


def data_to_str(temp, is_open, dists):

    assert isinstance(is_open, bool)

    temp_str = "%02i" % temp

    msg = "AF_" + temp_str + '_' + str(int(is_open)) + '_' + str(int(len(dists)))

    dist_strings = ""
    for d in dists:
        dist_strings += ("_%04i" % d)

    msg += dist_strings
    return msg

# print data_to_str(7, False, [123, 0010, 20])

temp = 10
dists = [100, 200, 300]
is_open = False


while True:
    if randint(0, 100) == 42:
        is_open = not is_open

    if randint(0, 100) == 42:
        dtemp = randint(-3, 3)
        temp += dtemp

    if temp <= 0:
        temp = 1
    if temp >= 30:
        temp = 30

    for ndx, value in enumerate(dists):
        if randint(0, 100) == 42:
            dt = randint(-5, 5)
            dists[ndx] += dt

    time.sleep(1)

    msg = data_to_str(temp, is_open, dists)
    print msg
    client.publish("fridge_state", msg)
