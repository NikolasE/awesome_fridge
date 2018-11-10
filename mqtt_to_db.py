#! /usr/bin/python

import paho.mqtt.client as mqtt
import time
import os
import yaml


class FridgeDB:
    def __init__(self):
        self.dir = "/home/engelhard/Documents/awesome_fridge"
        self.state_file = self.dir + '/states.txt'
        if not os.path.exists(self.state_file):
            print "Creating new fridge state at ", self.state_file
            f = open(self.state_file, 'w')
            # f.write("Fridge states")
            f.close()

    def store_fridge_state(self, fs):
        assert isinstance(fs, FridgeState)
        f = open(self.state_file, 'a')
        f.write(str(fs) + '\n')
        f.close()

    def read_state_file(self):
        f = open(self.state_file, 'r')
        lines = f.readlines()
        f.close()
        entries = list() # lines[0]
        for l in lines:
            fs = FridgeState()
            fs.from_string(l)
            entries.append(fs)
        return entries

    def get_open_states(self):
        entries = self.read_state_file()
        return [(e.data[FridgeState.timestamp], e.data[FridgeState.open]) for e in entries]

    def get_temperature(self):
        entries = self.read_state_file()
        return [(e.data[FridgeState.timestamp], e.data[FridgeState.temp]) for e in entries]

    def get_distances(self):
        entries = self.read_state_file()
        l = list()
        dist_cnt = 0
        for e in entries:
            d = e.data[FridgeState.distance]
            dist_cnt = len(d)
            line = [e.data[FridgeState.timestamp]]
            line += d
            l.append(line)
        return l, dist_cnt



class FridgeState:
    temp = 'temp'
    open = 'fridge_open'
    distance = 'distances'
    timestamp = 'timestamp'

    def __init__(self, msg=None):
        self.data = dict()
        self.data[self.timestamp] = time.time()
        if msg:
            self.parse_message(msg)

    def from_string(self, db_string):
        self.data = eval(db_string)

    def __str__(self):
        return str(self.data)

    def parse_message(self, msg):
        assert isinstance(msg, str)
        if not msg.startswith('AF_'):
            print ("INVALID MSG: '%s'", msg)
            return False

        spl = msg.split('_')
        self.data[self.temp] = int(spl[1])
        self.data[self.open] = bool(int(spl[2]))
        # dist_cnt = int(spl[3])
        self.data[self.distance] = map(int, spl[4:])
        return True


if __name__ == "__main__":
    f_db = FridgeDB()

    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(client, userdata, flags, rc):
        # print("Connected with result code "+str(rc))
        assert isinstance(client, mqtt.Client)
        client.subscribe("fridge_state")

    # The callback for when a PUBLISH message is received from the server.
    def on_message(client, userdata, msg):
        # print(msg.topic+" "+str(msg.payload))
        if msg.topic == "fridge_state":
            fs = FridgeState(msg=msg.payload)
            f_db.store_fridge_state(fs)


    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("broker.mqttdashboard.com", 1883)
    client.loop_forever()