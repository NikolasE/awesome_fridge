import subprocess
import sys
import os
import signal

processes = []
def text_to_speech(text, speed=260, gap=5):
	p = subprocess.Popen(['espeak', text])

def stop_sound():
	subprocess.Popen(['pkill', '-f', 'vlc'])

def hot_in_here():
	play_file('/home/magazino/Desktop/HotInHerre.mp3')

def get_low():
	play_file('/home/magazino/Desktop/GetLow.mp3')

def play_file(path):
	if sys.platform == 'linux2':
		subprocess.Popen(["xdg-open", path])
	elif sys.platform == 'darwin':
		subprocess.Popen(["afplay", path])

