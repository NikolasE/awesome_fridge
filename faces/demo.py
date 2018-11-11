#! /usr/bin/python

import face_recognition
import cv2
import paho.mqtt.client as mqtt

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

with_output = False

video_capture = cv2.VideoCapture(1)
#files = ['George.png', 'Michael.png'] 
files = ['George.png', 'Michael.png', "Nikolas.png"]

# Load a sample picture and learn how to recognize it.
# obama_image = face_recognition.load_image_file("nikolas.png")
# obama_face_encoding = face_recognition.face_encodings(obama_image)[0]
#
# Load a second sample picture and learn how to recognize it.
# biden_image = face_recognition.load_image_file("george.png")
# biden_face_encoding = face_recognition.face_encodings(biden_image)[0]

known_face_encodings = list()
known_face_names = list()

# load and train faces
for f in files:
    known_face_names.append(f.split('.')[0])
    img = face_recognition.load_image_file(f)
    face_enc = face_recognition.face_encodings(img)[0]
    known_face_encodings.append(face_enc)

# Initialize some variables
face_locations = []
face_encodings = []
face_names = []
process_this_frame = True

frame_id = -1

collected_faces = set()

while True:
    frame_id += 1

    # Grab a single frame of video
    ret, frame = video_capture.read()
    frame = cv2.flip(frame, 1)

    # Resize frame of video to 1/4 size for faster face recognition processing
    small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)

    # Convert the image from BGR color (which OpenCV uses) to RGB color (which face_recognition uses)
    rgb_small_frame = small_frame[:, :, ::-1]

    # Only process every other frame of video to save time
    if process_this_frame:
        # Find all the faces and face encodings in the current frame of video
        face_locations = face_recognition.face_locations(rgb_small_frame)
        face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)

        face_names = []
        for face_encoding in face_encodings:
            # See if the face is a match for the known face(s)
            matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
            name = "Unknown"

            # If a match was found in known_face_encodings, just use the first one.
            if True in matches:
                first_match_index = matches.index(True)
                name = known_face_names[first_match_index]

            collected_faces.add(name)
            face_names.append(name)

    # process_this_frame = not process_this_frame

    if frame_id % 30 == 0:
        print face_names
        for f in collected_faces:
          client.publish("faces", f)
        if not collected_faces:
            client.publish("faces", "dark")  # published as led-command

        collected_faces.clear()

    # Display the results
    for (top, right, bottom, left), name in zip(face_locations, face_names):
        # Scale back up face locations since the frame we detected in was scaled to 1/4 size
        top *= 4
        right *= 4
        bottom *= 4
        left *= 4

        color = (0, 0, 255)

        if name == "Nikolas":
            color = (32, 255, 0)
        if name == "George":
            color = (255, 170, 0)
        if name == "Michael":
            color = (244, 65, 235)

        # Draw a box around the face
        cv2.rectangle(frame, (left, top), (right, bottom), color, 2)

        # Draw a label with a name below the face
        cv2.rectangle(frame, (left, bottom - 35), (right, bottom), color, cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    # Display the resulting image
    if with_output:
        cv2.imshow('Video', frame)

    # Hit 'q' on the keyboard to quit!
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release handle to the webcam
video_capture.release()
cv2.destroyAllWindows()
