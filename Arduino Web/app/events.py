from flask import request
from flask_socketio import emit
from .shared_state import light_status

from . import socketio

users = {}

@socketio.on("connect")
def handle_connect():
    print("Client connected!")
    
    
@socketio.on('toggle_light')
def handle_toggle_light():
    global light_status
    light_status = not light_status
    response = {'light_status': light_status}
    print(response)
    socketio.emit('light_status_changed', response)
