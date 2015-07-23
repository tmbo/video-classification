# System imports
import subprocess
import time
from os import path
import os           
import shutil

import numpy as np
from flask.ext.cors import CORS
from flask import *
from werkzeug import secure_filename
from flask_extensions import *

# Local predicition modules
# sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'prediction')) #find modules in parent_folder/predictions

static_assets_path = path.join(path.dirname(__file__), "dist")
app = Flask(__name__, static_folder=static_assets_path)
CORS(app)
LABELS = ["MilitaryParade", "PlayingSitar", "TennisSwing", "VolleyballSpiking", "HandstandPushups", "Typing", "TableTennisShot", "BrushingTeeth", "StillRings", "HulaHoop", "PlayingDaf", "Drumming", "CricketShot", "ApplyLipstick", "WritingOnBoard", "TrampolineJumping", "YoYo", "BoxingPunchingBag", "Knitting", "Basketball", "PlayingFlute", "JavelinThrow", "ApplyEyeMakeup", "HeadMassage", "RopeClimbing", "WalkingWithDog", "Skijet", "PlayingTabla", "HighJump", "CleanAndJerk", "Fencing", "Archery", "UnevenBars", "HammerThrow", "BaseballPitch", "Haircut", "SalsaSpin", "FrontCrawl", "BabyCrawling", "CuttingInKitchen", "SoccerJuggling", "Billiards", "SkyDiving", "HandstandWalking", "PlayingCello", "Surfing", "SkateBoarding", "Mixing", "FloorGymnastics", "SoccerPenalty", "Swing", "CliffDiving", "Skiing", "Lunges", "ShavingBeard", "BasketballDunk", "HorseRace", "BodyWeightSquats", "Rowing", "RockClimbingIndoor", "HorseRiding", "BlowDryHair", "Diving", "FieldHockeyPenalty", "JumpingJack", "PlayingPiano", "BreastStroke", "JugglingBalls", "FrisbeeCatch", "Rafting", "Biking", "BoxingSpeedBag", "MoppingFloor", "CricketBowling", "ThrowDiscus", "LongJump", "TaiChi", "PushUps", "BandMarching", "JumpRope", "PullUps", "SumoWrestling", "PlayingViolin", "Nunchucks", "Hammering", "GolfSwing", "PommelHorse", "PoleVault", "PizzaTossing", "PlayingDhol", "Punch", "ParallelBars", "PlayingGuitar", "Kayaking", "BalanceBeam", "WallPushups", "IceDancing", "BenchPress", "BlowingCandles", "Bowling", "Shotput"]

# ----- Video -----------
import caffe
def get_frames(video_file_name, frame_rate, output_folder):
    cmd = "ffmpeg -n -nostdin -i \"%s\" -r \"%d\" -qscale:v 2 \"%s/%%3d.jpg\"" % (video_file_name, frame_rate, output_folder)
    subprocess.call(cmd, shell=True)
    return output_folder

def load_frame_data(folder):
    frame_files = os.listdir(folder)
    print frame_files
    frame_data = []
    for idx, frame_file in enumerate(frame_files):
        img = caffe.io.load_image(os.path.join(folder, frame_file))
        frame_data.append(img)
    return frame_data

def predict_caffe(frames):
    net = caffe.Net(app.config["CAFFE_SPATIAL_PROTO"], app.config["CAFFE_SPATIAL_MODEL"], caffe.TEST)

    transformer = caffe.io.Transformer({'data': net.blobs['data'].data.shape})
    transformer.set_transpose('data', (2,0,1))
    transformer.set_raw_scale('data', 255)  # the reference model operates on images in [0,255] range instead of [0,1]
    transformer.set_channel_swap('data', (2,1,0))  # the reference model has channels in BGR order instead of RGB
    transformer.set_mean('data', np.load(app.config["CAFFE_SPATIAL_MEAN"]).mean(1).mean(1)) # mean pixel
    
    data = np.zeros((len(frames), 3, net.blobs['data'].data.shape[2], net.blobs['data'].data.shape[3]))
    
    for idx, frame in enumerate(frames):
        data[idx, :, :, :] = transformer.preprocess('data', frame)
        
    caffe.set_mode_cpu()
    net.blobs['data'].reshape(*data.shape)

    out = net.forward_all(data=data)
    return out['prob']

# ----- Routes ----------

@app.route("/", defaults={"fall_through": ""})
@app.route("/<path:fall_through>")
def index(fall_through):
    if fall_through:
        return redirect(url_for("index"))
    else:
        return app.send_static_file("index.html")


@app.route("/dist/<path:asset_path>")
def send_static(asset_path):
    return send_from_directory(static_assets_path, asset_path)


@app.route("/videos/<path:video_path>")
def send_video(video_path):
    return send_file_partial(path.join(app.config["UPLOAD_FOLDER"], video_path))


@app.route("/api/upload", methods=["POST"])
def uploadVideo():
    def is_allowed(filename):
        return len(filter(lambda ext: ext in filename, ["avi", "mpg", "mpeg", "mkv", "webm", "mp4", "mov"])) > 0

    file = request.files.getlist("video")[0]

    if file and is_allowed(file.filename):
        filename = secure_filename(file.filename)
        file_path = path.join(app.config["UPLOAD_FOLDER"], filename)
        file.save(file_path)

        response = jsonify(get_prediction(file_path))
    else:
        response = bad_request("Invalid file")

    return response


@app.route("/api/example/<int:example_id>")
def use_example(example_id):
    if example_id <= 3:
        filename = "video%s.webm" % example_id
        file_path = path.join(app.config["UPLOAD_FOLDER"], "examples", filename)
        response = jsonify(get_prediction(file_path))
    else:
        response = bad_request("Invalid Example")

    return response


def bad_request(reason):
    response = jsonify({"error": reason})
    response.status_code = 400
    return response


# -------- Prediction & Features --------
def get_prediction(file_path):
    temp_dir = path.join(app.config["TEMP_FOLDER"], "", file_path)
    shutil.rmtree(temp_dir, ignore_errors=True)
    os.makedirs(temp_dir)
    get_frames(file_path, 2, temp_dir)
    frames = load_frame_data(temp_dir)

    
    #  predictions = external_script.predict(file_path)
    predictions = predict_caffe(frames).astype(np.float32, copy=False)
    print "Shape of predicitons", predictions.shape, "Type", type(predictions)

    file_path = file_path + "?cachebuster=%s" % time.time()
    result = {
        "video": {
            "url": "%s" % file_path,
            "framerate": 25
        },
        "frames": []
    }

    for index, row in enumerate(predictions):

        pred_per_label = []

        five_best = np.argpartition(row, -5)[-5:]
        for i in five_best:
            pred_per_label.append({"label": LABELS[i], "prob": row[i].item()})

        new_frame = {
            "frameNumber": index,
            "predictions": pred_per_label
        }

        result["frames"].append(new_frame)


    print result


    return result


if __name__ == "__main__":
    # Start the server
    app.config.update(
        DEBUG=True,
        SECRET_KEY="asassdfs",
        CORS_HEADERS="Content-Type",
        UPLOAD_FOLDER="videos",
        TEMP_FOLDER="temp",
        CAFFE_SPATIAL_PROTO="/Users/tombocklisch/Documents/Studium/Master Project/models/deploy.prototxt",
        CAFFE_SPATIAL_MODEL="/Users/tombocklisch/Documents/Studium/Master Project/models/_iter_70000.caffemodel",
        CAFFE_SPATIAL_MEAN="/Users/tombocklisch/Documents/Studium/Master Project/models/ilsvrc_2012_mean.npy" #"/home/mpss2015/caffe/python/caffe/imagenet/ilsvrc_2012_mean.npy"
    )

    # Make sure all frontend assets are compiled
    subprocess.Popen("webpack")

    # Start the Flask app
    app.run(port=9000, threaded=True)
