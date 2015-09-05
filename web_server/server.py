# System imports
import subprocess
from flask.ext.cors import CORS
from flask import Flask
from blueprints import video_blueprint, image_blueprint, frontend_blueprint

app = Flask(__name__, static_folder="dist")
CORS(app)

app.register_blueprint(video_blueprint)
app.register_blueprint(image_blueprint)
app.register_blueprint(frontend_blueprint)

if __name__ == "__main__":

    # Start the server
    app.config.update(
        DEBUG=True,
        SECRET_KEY="asassdfs",
        CORS_HEADERS="Content-Type",
    )

    # Make sure all frontend assets are compiled
    subprocess.Popen("webpack")

    # Start the Flask app
    app.run(port=9000, threaded=True, host="0.0.0.0")
