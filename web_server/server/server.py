# System imports
import sys, requests, subprocess
from os import path
from flas                        k.ext.cors import CORS
from flask import *
from flask.json import jsonify
from werkzeug import secure_filename


# Local predicition modules
# sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'prediction')) #find modules in parent_folder/predictions

static_assets_path = path.join(path.dirname(__file__), "dist")
app = Flask(__name__, static_folder= static_assets_path)
CORS(app)


# ----- Routes ----------

@app.route("/")
def index():
    return app.send_static_file("index.html")

@app.route("/<path:path>")
def send_static(path):
    print "Asset requested: ", path
    return send_from_directory(static_assets_path, path)


@app.route("/api/upload", methods=["POST"])
def uploadVideo():

    def isAllowed(filename):
        return len(filter(lambda ext: ext in filename, ["avi", "mpg", "mpeg", "mkv"])) > 0

    file = request.files.getlist("video")[0]

    print file.filename
    print isAllowed(file.filename)

    if file and isAllowed(file.filename):
        filename = secure_filename(file.filename)
        file.save(filename)
        response = jsonify({"success" : filename})

    else:
        response = jsonify({"error" : "Invalid file"})
        response.status_code = 400

    return response


@app.route("/api/features/<int:question_id>")
def json_features(question_id):
    return jsonify(get_features(question_id))


@app.route("/api/predictions/<int:question_id>")
def json_prediction(question_id):
    prediction = get_prediction(features)
    return jsonify(prediction)


# -------- Prediction & Features --------
def get_prediction(features):

    return {
        "activity" : "Archery",
    }


if __name__ == "__main__":
    # Start the server
    app.config.update(
        DEBUG = True,
        SECRET_KEY = "asassdfs",
        CORS_HEADERS = "Content-Type"
    )

    # Make sure all frontend assets are compiled
    subprocess.Popen("webpack")

    # Start the Flask app
    app.run(port=9000)
