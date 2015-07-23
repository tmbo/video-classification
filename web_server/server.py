# System imports
import subprocess
import time
from os import path
import shutil

import numpy as np
from flask.ext.cors import CORS
from flask import *
from werkzeug import secure_filename
from flask_extensions import *
import math

# Local predicition modules
# find modules in parent_folder/predictions
# sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'prediction')) 

static_assets_path = path.join(path.dirname(__file__), "dist")
app = Flask(__name__, static_folder=static_assets_path)
CORS(app)


LABEL_MAPPING = {}


def load_label_mapping():
    global LABEL_MAPPING
    with open(app.config["LABEL_MAPPING"], "r") as f:
        for line in f:
            splitted = line.split(" ")
            LABEL_MAPPING[int(splitted[1])] = splitted[0]
        
        
def slice_in_chunks(els, n):
    for i in range(0, len(els), n):
        yield els[i:i+n]


def clear_folder(folder):
    for the_file in os.listdir(folder):
        file_path = os.path.join(folder, the_file)
        try:
            if os.path.isfile(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path): 
                shutil.rmtree(file_path)
        except Exception, e:
            print e

# ----- Video -----------
import caffe


def create_frames(video_file_name, frame_rate, output_folder):
    cmd = "ffmpeg -n -nostdin -i \"%s\" -r \"%d\" -qscale:v 2 \"%s/%%4d.jpg\"" % (video_file_name, frame_rate, output_folder)
    subprocess.call(cmd, shell=True)
    return output_folder

def create_flows(frame_folder, output_folder):
    cmd = "%s %s %s 0" % (app.config["FLOW_CMD"], frame_folder, output_folder)
    subprocess.call(cmd, shell=True)
    return output_folder


def load_frame_data(frame_file):
    return caffe.io.load_image(frame_file)


def load_frames(frame_list, w, h, transformer):
    data = np.zeros((len(frame_list), 3, w, h))

    for idx, frame in enumerate(frame_list):
        data[idx, :, :, :] = transformer.preprocess('data', load_frame_data(frame))
    
    return data


def frames_in_folder(folder):
    return map(lambda p: os.path.join(folder, p), os.listdir(folder))


def predict_caffe(frame_files):
    net = caffe.Net(app.config["CAFFE_SPATIAL_PROTO"], app.config["CAFFE_SPATIAL_MODEL"], caffe.TEST)
    batch_size = app.config["CAFFE_BATCH_LIMIT"]

    transformer = caffe.io.Transformer({'data': net.blobs['data'].data.shape})
    transformer.set_transpose('data', (2, 0, 1))
    transformer.set_raw_scale('data', 255)  # the reference model operates on images in [0,255] range instead of [0,1]
    transformer.set_channel_swap('data', (2, 1, 0))  # the reference model has channels in BGR order instead of RGB
    transformer.set_mean('data', np.load(app.config["CAFFE_SPATIAL_MEAN"]).mean(1).mean(1))  # mean pixel

    caffe.set_mode_cpu()
    
    results = np.zeros((len(frame_files), app.config["CAFFE_NUM_LABELS"]))
    
    for cidx, chunk in enumerate(slice_in_chunks(frame_files, batch_size)):
        
        data = load_frames(chunk, net.blobs['data'].data.shape[2], net.blobs['data'].data.shape[3], transformer)
        
        net.blobs['data'].reshape(*data.shape)
    
        out = net.forward_all(data=data)
        print "Finished chunk %d of %d" % (cidx, math.ceil(len(frame_files) * 1.0 / batch_size))
        results[cidx*batch_size: cidx*batch_size+len(chunk), :] = out['prob']
    return results


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
def upload_video():
    def is_allowed(file_name):
        return len(filter(lambda ext: ext in file_name, ["avi", "mpg", "mpeg", "mkv", "webm", "mp4", "mov"])) > 0

    video_file = request.files.getlist("video")[0]

    if file and is_allowed(video_file.filename):
        file_name = secure_filename(video_file.filename)
        file_path = path.join(app.config["UPLOAD_FOLDER"], file_name)
        video_file.save(file_path)

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
    temp_dir = path.join(app.config["TEMP_FOLDER"], "frames", file_path)
    shutil.rmtree(temp_dir, ignore_errors=True)
    os.makedirs(temp_dir)
    create_frames(file_path, 25, temp_dir)
    create_flows(temp_dir)

    # predictions = external_script.predict(file_path)
    predictions = predict_caffe(frames_in_folder(temp_dir))
    print "Shape of predicitons", predictions.shape, "Type", type(predictions)
    
    print "Max ", np.argmax(predictions, axis=1)

    file_path += "?cachebuster=%s" % time.time()
    result = {
        "video": {
            "url": "%s" % file_path,
            "framerate": 25
        },
        "frames": []
    }

    for idx, row in enumerate(predictions):

        predictions_per_label = []

        five_best = np.argpartition(row, -5)[-5:]
        for i in five_best:
            predictions_per_label.append({"label": LABEL_MAPPING[i], "prob": row[i].item()})

        new_frame = {
            "frameNumber": idx,
            "predictions": predictions_per_label
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
        LABEL_MAPPING="/Users/tombocklisch/Documents/Studium/Master Project/models/label_mapping.txt",
        FLOW_CMD="/Users/tombocklisch/Documents/Studium/Master Project/somefile",
        CAFFE_BATCH_LIMIT=50,
        CAFFE_NUM_LABELS=101,
        CAFFE_SPATIAL_PROTO="/Users/tombocklisch/Documents/Studium/Master Project/models/deploy.prototxt",
        CAFFE_SPATIAL_MODEL="/Users/tombocklisch/Documents/Studium/Master Project/models/_iter_70000.caffemodel",
        CAFFE_SPATIAL_MEAN="/Users/tombocklisch/Documents/Studium/Master Project/models/ilsvrc_2012_mean.npy"  #"/home/mpss2015/caffe/python/caffe/imagenet/ilsvrc_2012_mean.npy"
    )
    
    clear_folder(path.join(app.config["TEMP_FOLDER"], "frames"))

    load_label_mapping()

    # Make sure all frontend assets are compiled
    subprocess.Popen("webpack")

    # Start the Flask app
    app.run(port=9000, threaded=True)
