# System imports
import subprocess
import time
import os
import shutil
import pipes
import numpy as np
from flask import request, Blueprint, jsonify
from werkzeug import secure_filename
import json
import caffe


CONFIG = json.load(file("files.json"))

NETWORK_BATCH_SIZE = 16

FLOW_STACK_SIZE = 10

LABEL_MAPPING = {}

video_blueprint = Blueprint('video_blueprint', __name__)

#---------- Routes -------------

@video_blueprint.route("/api/upload_video", methods=["POST"])
def upload_video():
    def is_allowed(file_name):
        return len(filter(lambda ext: ext in file_name, ["avi", "mpg", "mpeg", "mkv", "webm", "mp4", "mov"])) > 0

    video_file = request.files.getlist("video")[0]

    if file and is_allowed(video_file.filename):
        file_name = secure_filename(video_file.filename)
        file_path = os.path.join(CONFIG["UPLOAD_FOLDER"], file_name)
        video_file.save(file_path)

        response = jsonify(get_video_prediction(file_path))
    else:
        response = bad_request("Invalid file")

    return response


@video_blueprint.route("/api/example/<int:example_id>")
def use_example(example_id):
    if example_id <= 3:
        filename = "video%s.webm" % example_id
        file_path = os.path.join("videos", "examples", filename)
        response = jsonify(get_video_prediction(file_path))
    else:
        response = bad_request("Invalid Example")

    return response



def load_label_mapping():
    global LABEL_MAPPING
    with open(CONFIG["VIDEO_LABEL_MAPPING"], "r") as f:
        for line in f:
            splitted = line.split(" ")
            LABEL_MAPPING[int(splitted[1])] = splitted[0]


def slice_in_chunks(els, n):
    for i in range(0, len(els), n):
        yield els[i:i + n]


def clear_folder(folder):
    try:
        for the_file in os.listdir(folder):
            file_path = os.path.join(folder, the_file)
            try:
                if os.path.isfile(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)
            except Exception as e:
                print e
    except Exception:
        pass


def select_indices(max_len, n):
    l = range(0, max_len, (max_len - 1) / (n - 1))[:n]
    assert len(l) == n, "created invalid frame selection " + str(len(l))
    return l


def create_frames(video_file_name, frame_rate, output_folder):
    # TODO: CROP BETTER
    cmd = "ffmpeg -n -nostdin -i \"%s\" -r \"%d\" -qscale:v 2 -filter:v \"crop=224:224:0:0\" \"%s/%%3d.jpg\"" % (
        video_file_name, frame_rate, output_folder)
    subprocess.check_call(cmd, shell=True)
    return output_folder


def create_flows(frame_folder, output_folder):
    curr_path = os.path.dirname(os.path.abspath(__file__))
    in_path = os.path.join(curr_path, os.path.dirname(frame_folder))
    out_path = os.path.join(curr_path, os.path.dirname(output_folder))

    cmd = "%s %s %s 0" % (pipes.quote(CONFIG["FLOW_CMD"]), pipes.quote(in_path), pipes.quote(out_path))
    subprocess.check_call(cmd, shell=True)
    return output_folder


def load_frame_data(frame_file):
    return caffe.io.load_image(frame_file)


def load_frames(frame_list, w, h, transformer):
    data = np.zeros((len(frame_list), 3, w, h))

    for idx, frame in enumerate(frame_list):
        data[idx, :, :, :] = transformer.preprocess('frames_data', load_frame_data(frame))

    return data


def load_flows(selected_frames, all_flows_list, w,
               h):  # , transformer): --- we removed the transformer for the flow and do it manually
    assert len(all_flows_list) % 2 == 0, "Error: Number of flows need to be divisible by 2"

    y_start = len(all_flows_list) / 2

    def load_flow_stack(start_idx):
        images = []
        for flow_offset in range(0, FLOW_STACK_SIZE):
            try:
                image1 = all_flows_list[start_idx + flow_offset]
                image2 = all_flows_list[start_idx + flow_offset + y_start]
            except:
                image1 = "empty_flow.jpg"
                image2 = "empty_flow.jpg"
            images.append(caffe.io.load_image(image1, False))
            images.append(caffe.io.load_image(image2, False))
        stacking = np.dstack(images)
        stacking = np.transpose(stacking, (2, 0, 1))
        return stacking

    data = np.zeros((len(selected_frames), FLOW_STACK_SIZE * 2, w, h))

    for idx, frame_id in enumerate(selected_frames):
        data[idx, :, :, :] = load_flow_stack(frame_id)
        # transformer.preprocess('flow_data', load_flow_stack(frame_id))

    data *= 255
    data -= 127.0
    return data


def frames_in_folder(folder):
    return map(lambda p: os.path.join(folder, p), os.listdir(folder))


def predict_caffe(all_frame_files, all_flow_files):
    idxs = select_indices(len(all_frame_files), NETWORK_BATCH_SIZE)
    frame_files = [all_frame_files[idx] for idx in idxs]
    net = caffe.Net(str(CONFIG["CAFFE_FUSION_PROTO"]), str(CONFIG["CAFFE_FUSION_MODEL"]), caffe.TEST)

    transformer = caffe.io.Transformer({'frames_data': net.blobs['frames_data'].data.shape})
    # data transformations
    transformer.set_transpose('frames_data', (2, 0, 1))
    transformer.set_raw_scale('frames_data',
                              255)  # the reference model operates on images in [0,255] range instead of [0,1]
    transformer.set_channel_swap('frames_data',
                                 (2, 1, 0))  # the reference model has channels in BGR order instead of RGB
    transformer.set_mean('frames_data', np.load(CONFIG["CAFFE_SPATIAL_MEAN"]).mean(1).mean(1))  # mean pixel

    caffe.set_mode_cpu()

    frame_data = load_frames(frame_files, net.blobs['frames_data'].data.shape[2],
                             net.blobs['frames_data'].data.shape[3], transformer)
    flow_data = load_flows(idxs, all_flow_files, net.blobs['flow_data'].data.shape[2],
                           net.blobs['flow_data'].data.shape[
                               3])  # , flow_transformer) # we removed the flow transformer

    out = net.forward_all(frames_data=frame_data, flow_data=flow_data)
    return out['prob'], out['frames_prob'], out['flow_prob']

# -------- Prediction & Features --------

# Predicition for a video file including video preprocessing
def get_video_prediction(file_path):
    global NETWORK_BATCH_SIZE
    NETWORK_BATCH_SIZE = 16

    temp_dir = os.path.join(CONFIG["TEMP_FOLDER"], "frames", file_path)
    shutil.rmtree(temp_dir, ignore_errors=True)
    os.makedirs(temp_dir)
    create_frames(file_path, 15, temp_dir)
    flow_dir = os.path.join(CONFIG["TEMP_FOLDER"], "flows", file_path)
    os.makedirs(flow_dir)
    create_flows(temp_dir, flow_dir)

    # predictions = external_script.predict(file_path)
    fusion_predictions, frame_predictions, flow_predictions = predict_caffe(frames_in_folder(temp_dir),
                                                                            frames_in_folder(flow_dir))

    # Decide which prediction to display in the line plot
    #    predictions = flow_predictions
    print "Shape of predictions", fusion_predictions.shape, "Type", type(fusion_predictions)
    print "Max ", np.argmax(fusion_predictions, axis=1)
    print "predictions", fusion_predictions

    clear_folder(os.path.join(CONFIG["TEMP_FOLDER"], "frames"))
    clear_folder(os.path.join(CONFIG["TEMP_FOLDER"], "flows"))

    result = bundle_response(file_path, fusion_predictions, frame_predictions, flow_predictions)
    print result
    return result



def bundle_response(media_file_path, fusion_predictions, frame_predictions, flow_predictions):

    def top5_predictions(predictions):
        top5 = []
        for idx, row in enumerate(predictions):
            predictions_per_label = []

            five_best_indices = n_largest(row, 5)
            print "five_best", five_best_indices

            for i in five_best_indices:
                predictions_per_label.append({"label": LABEL_MAPPING[i], "prob": row[i].item()})

            new_frame = {
                "frameNumber": idx,
                "predictions": predictions_per_label
            }

            top5.append(new_frame)
        return top5

    media_file_path += "?cachebuster=%s" % time.time()
    result = {
        "media": {
            "url": "/%s" % media_file_path,
            "type": "video"
        },
        "fusion_predictions": [],
        "frames": [],
        "flows": []
    }

    # Overall fusion prediction top 5
    five_best_indices = n_largest(fusion_predictions[0], 5)
    for i in np.sort(five_best_indices):
        result["fusion_predictions"].append({"label": LABEL_MAPPING[i], "prob": fusion_predictions[0, i].item()})

    # Per frame spatial prediction top 5
    result["frames"] = top5_predictions(frame_predictions)

    # Per frame flow prediction top 5
    result["flows"] = top5_predictions(flow_predictions)

    return result

def n_largest(np_array, n):
        return np.argpartition(np_array, -n)[-n:]

def bad_request(reason):
    response = jsonify({"error": reason})
    response.status_code = 400
    return response


# Run once at start
clear_folder(os.path.join(CONFIG["TEMP_FOLDER"], "frames"))
clear_folder(os.path.join(CONFIG["TEMP_FOLDER"], "flows"))

load_label_mapping()