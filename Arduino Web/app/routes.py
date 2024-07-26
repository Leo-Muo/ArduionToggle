from flask import Blueprint
from flask import Flask, request, render_template, jsonify
from .shared_state import light_status
from .events import socketio

main_blueprint = Blueprint('main', __name__)


@main_blueprint.route('/')
@main_blueprint.route('/index')
def index():
    return render_template("index.html", light_status=light_status)

@main_blueprint.route('/status', methods=['GET'])
def get_light_status():
    global light_status
    return jsonify({"light_status": light_status})


@main_blueprint.route('/set_light', methods=['POST'])
def handle_set_light():
    global light_status
    data = request.get_json()
    if data and 'status' in data:
        
        status_str = data['status'].lower()  # Convert to lowercase to handle case insensitivity

        if status_str == 'true':
            light_status = True
        elif status_str == 'false':
            light_status = False
        response = {'light_status': light_status}
        print(response)
        socketio.emit('light_status_changed', response)
        return jsonify({'light_status': light_status})
    return jsonify({'error': 'Invalid data'}), 400

