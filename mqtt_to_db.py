#! /usr/bin/python

import paho.mqtt.client as mqtt
import time
import os
import yaml


class FridgeDB:
    def __init__(self):
        self.dir = "/home/engelhard/Documents/awesome_fridge"
        self.state_file = self.dir + '/states.txt'
        # self.face_file = self.dir + '/faces.txt'
        self.door_file = self.dir + '/door.txt'

        self._ensure_file(self.state_file)
        self._ensure_file(self.door_file)

        # only stored in memory
        self._last_face_name = "NO_LAST_FACE"
        self._last_face_time = 0

        self._last_door_open_time = 0

        self._fridge_is_open = None
        self.client = None
        self.alarm_active = False

    def _ensure_file(self, filename):
        if not os.path.exists(filename):
            print "Creating new file at ", filename
            f = open(self.state_file, 'w')
            f.close()

    def store_face_state(self, face_name):
        self._last_face_name = face_name
        self._last_face_time = time.time()
        if not self.alarm_active:

            if face_name == "Unknown":
                self.set_led('alarm')  # red
            else:
                self.set_led(face_name)

    def start_alarm(self):
        if self.alarm_active:
            return

        self.alarm_active = True
        self.set_led('alarm')  # 100, 100, 100

        # self.client.publish("text_to_speech", "SHAME " + self._last_face_name + " SHAME");
        self.client.publish("text_to_speech", "a")
        self.client.publish("music", str("hot"))

        f = open(self.door_file, 'a')
        f.write(str(time.time()) + ' ' + self._last_face_name + '\n')
        f.close()

    def stop_alarm(self):
        if not self.alarm_active:
            return

        self.alarm_active = False
        self.set_led('dark')  # 100, 100, 100
        self.client.publish('music', "stop")

    def store_fridge_state(self, fs):
        door_open = fs.data[fs.open]

        if not door_open:
            self.stop_alarm()

        if self._fridge_is_open != door_open:
            print ("DOOR OPEN CHANGED to " + str(door_open) + " by " + self._last_face_name)
            self._fridge_is_open = door_open

            if self._fridge_is_open:
                t = time.time()
                # print "door was opened at ", t
                self._last_door_open_time = t

        # print self._fridge_is_open, self._last_door_open_time
        # check how long fridge was opened:
        if self._fridge_is_open and self._last_door_open_time > 0:
            dt = (time.time() - self._last_door_open_time)
            print "fridge open for %f seconds" % dt

            if dt > 5:
                print self._last_face_name, "forgot to close the door"
                self.start_alarm()

        assert isinstance(fs, FridgeState)
        f = open(self.state_file, 'a')
        f.write(str(fs) + '\n')
        # print "writing", str(fs)
        f.close()

    def set_led(self, command):
        if not self.client:
            print "NO MQTT CLIENT"
            return

        self.client.publish('led', command)

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
            # print e
            if e.data[FridgeState.open]:
                print "skipping", e
                continue
            d = e.data[FridgeState.distance]
            if max(d) > 30:
                print "skipping (large value)", e
                continue
            dist_cnt = len(d)
            line = [e.data[FridgeState.timestamp]]
            line += d
            l.append(line)
        return l, dist_cnt

    def get_wrongdoers(self):
        f = open(self.door_file, 'r')
        lines = f.readlines()
        f.close()

        users = ["Nikolas", "George", "Michael"]

        d = dict()
        for u in users:
            d[u] = 0

        for l in lines:
            spl = l.split()
            name = spl[1]
            if not name in d:
                d[name] = 1
            else:
                d[name] += 1
        return d


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
        # board_id in first split
        self.data[self.temp] = int(spl[2])
        self.data[self.open] = bool(int(spl[3]))
        # dist_cnt = int(spl[4])
        self.data[self.distance] = map(int, spl[5:])
        # print self.data
        return True


if __name__ == "__main__":
    f_db = FridgeDB()

    face_topic_name = "faces"
    state_topic_name = "fridge_state"


    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(client, userdata, flags, rc):
        # print("Connected with result code "+str(rc))
        assert isinstance(client, mqtt.Client)
        client.subscribe(state_topic_name)
        client.subscribe(face_topic_name)

    # The callback for when a PUBLISH message is received from the server.
    def on_message(client, userdata, msg):
        # print(msg.topic+" "+str(msg.payload))
        if msg.topic == state_topic_name:
            fs = FridgeState(msg=msg.payload)
            f_db.store_fridge_state(fs)
        if msg.topic == face_topic_name:
            f_db.store_face_state(face_name=str(msg.payload))


    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("broker.mqttdashboard.com", 1883)

    f_db.client = client

    client.loop_forever()