#! /usr/bin/python

from mqtt_to_db import FridgeDB, FridgeState

import flask
from flask import Flask, Response
from flask_restful import Api, Resource, reqparse, request


app = Flask(__name__)
api = Api(app)
fdb = FridgeDB()

@app.route('/door_open', methods=['GET'])
def plot():
    response = flask.jsonify({'plot': fdb.get_open_states()})
    response.headers.add('Access-Control-Allow-Origin', '*')
    return response

print fdb.get_open_states()

app.run(debug=True)