#! /usr/bin/python
# coding: utf8


import requests

url = 'http://127.0.0.1:5000/door_open'
data_dict = {"user":"asddd"}

response = requests.get(url)  # , data_dict)
print response.content, response.status_code
