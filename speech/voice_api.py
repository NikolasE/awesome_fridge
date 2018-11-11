import subprocess
import sys
import os
import signal
import pygame

processes = []
pygame.init()

# mp3cut -o hot_short.mp3 -t 00:02:05+000-00:02:13+000 HotInHerre.mp3

def text_to_speech(text, speed=50, gap=5):
	p = subprocess.Popen(['espeak', '-s{}'.format(speed), '-g{}'.format(gap), text])

def stop_sound():
	#subprocess.Popen(['pkill', '-f', 'vlc'])
    pygame.mixer.music.stop()    

def hot_in_here():
    play_file('/home/magazino/Desktop/hot_short.mp3')

def get_low():
	play_file('/home/magazino/Desktop/GetLow.mp3')

def play_file(path):
	pygame.mixer.music.load(path)
	pygame.mixer.music.play()
		
#if sys.platform == 'linux2':
#		subprocess.Popen(["xdg-open", path])
	#elif sys.platform == 'darwin':
	#	subprocess.Popen(["afplay", path])

