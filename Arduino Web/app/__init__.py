from flask import Flask
from flask_socketio import SocketIO

socketio = SocketIO(cors_allowed_origins="*") 

def create_app():
    app = Flask(__name__)
    app.config["DEBUG"] = True
    app.config["SECRET_KEY"] = "secret"

    from .routes import main_blueprint
    app.register_blueprint(main_blueprint)

    from . import events  # Ensure socket events are registered

    socketio.init_app(app)

    return app
