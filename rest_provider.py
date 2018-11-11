#! /usr/bin/python

from mqtt_to_db import FridgeDB, FridgeState

import flask
from flask import Flask, Response
from flask_restful import Api, Resource, reqparse, request


app = Flask(__name__)
api = Api(app)
fdb = FridgeDB()

@app.route('/door_open', methods=['GET'])
def door_open():
    response = flask.jsonify({'plot': fdb.get_open_states()})
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response

@app.route('/distance', methods=['GET'])
def distance():
    dists, dist_cnt = fdb.get_distances()
    response = flask.jsonify({'data': dists, 'counter': dist_cnt})
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response

@app.route('/temperature', methods=['GET'])
def temperature():
    response = flask.jsonify({'temps': fdb.get_temperature()})
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response

@app.route('/wrongdoers', methods=['GET'])
def wrongdoers():
    response = flask.jsonify({'wrongdoers': fdb.get_wrongdoers()})
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response



# print fdb.get_open_states()

app.run(debug=True)