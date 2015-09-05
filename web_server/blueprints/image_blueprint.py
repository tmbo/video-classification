# System imports
import time
import os
import numpy as np
from flask import Blueprint, jsonify, request
from werkzeug import secure_filename
from video_blueprint import n_largest, bad_request, load_frame_data
import json
import caffe
import linecache

CONFIG = json.load(file("files.json"))

image_blueprint = Blueprint('image_blueprint', __name__)

# -------- Routes -----------

@image_blueprint.route("/api/upload_image", methods=["POST"])
def upload_image():
    def is_allowed(file_name):
        return len(filter(lambda ext: ext in file_name, ["jpg", "jpeg", "bmp", "tiff", "png", "gif"])) > 0

    image_file = request.files.getlist("image")[0]

    if file and is_allowed(image_file.filename):
        file_name = secure_filename(image_file.filename)
        file_path = os.path.join(CONFIG["UPLOAD_FOLDER"], file_name)
        image_file.save(file_path)

        response = jsonify(get_image_prediction(file_path))
    else:
        response = bad_request("Invalid file")

    return response

# ---------- Prediction ------------

# Prediciton for a single image
def get_image_prediction(file_path):
    global NETWORK_BATCH_SIZE
    NETWORK_BATCH_SIZE = 2

    predicitions = predict_caffe(file_path)

    print "Shape of predictions", predicitions.shape, "Type", type(predicitions)
    print "Max ", np.argmax(predicitions, axis=1)
    print "predictions", predicitions

    return bundle_response(file_path, predicitions)


def predict_caffe(file_path):
    net = caffe.Net(str(CONFIG["CAFFE_IMAGE_PROTO"]), str(CONFIG["CAFFE_IMAGE_MODEL"]), caffe.TEST)

    transformer = caffe.io.Transformer({'image_data': net.blobs['image_data'].data.shape})
    # data transformations
    #transformer.set_transpose('image_data', (2, 0, 1))
    #transformer.set_raw_scale('image_data', 255)  # the reference model operates on images in [0,255] range instead of [0,1]
    #transformer.set_channel_swap('image_data',(2, 1, 0))  # the reference model has channels in BGR order instead of RGB
    transformer.set_mean('image_data', np.load(CONFIG["CAFFE_SPATIAL_MEAN"]).mean(1).mean(1))  # mean pixel

    caffe.set_mode_cpu()

    data = np.zeros_like(net.blobs['image_data'].data)
    data[0, :, :, :] = transformer.preprocess("image_data", load_frame_data(file_path))

    out = net.forward_all(image_data=data)
    return out['prob']


def bundle_response(media_file_path, predicitions):

    media_file_path += "?cachebuster=%s" % time.time()
    result = {
        "media": {
            "url": "%s" % media_file_path,
        },
        "fusion_predictions": [],
        "frames": None,
        "flows": None
    }

    label_mapping = linecache(CONFIG["IMAGE_LABEL_MAPPING"])

    # Overall fusion prediction top 5
    five_best_indices = n_largest(predicitions[0], 5)
    for i in np.sort(five_best_indices):
        result["fusion_predictions"].append({"label": label_mapping.getline(i), "prob": predicitions[0, i].item()})
