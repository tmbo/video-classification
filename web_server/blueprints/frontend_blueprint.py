# System imports
from os import path
from flask import *
import os
import re
import mimetypes
import json

CONFIG = json.load(file("files.json"))

frontend_blueprint = Blueprint('frontend_blueprint', __name__, static_folder="../dist")

# ----- Routes ----------
@frontend_blueprint.route("/", defaults={"fall_through": ""})
@frontend_blueprint.route("/<path:fall_through>")
def index(fall_through):
    if fall_through:
        return redirect(url_for("frontend_blueprint.index"))
    else:
        return frontend_blueprint.send_static_file("index.html")


@frontend_blueprint.route("/dist/<path:asset_path>")
def send_static(asset_path):
    return send_from_directory(static_assets_path, asset_path)


@frontend_blueprint.route("/videos/<path:video_path>")
def send_video(video_path):
    return send_file_partial(path.join(CONFIG["UPLOAD_FOLDER"], video_path))


def send_file_partial(path):
    """
        Simple wrapper around send_file which handles HTTP 206 Partial Content
        (byte ranges)
        TODO: handle all send_file args, mirror send_file's error handling
    """
    range_header = request.headers.get('Range', None)
    if not range_header: return send_file(path)

    size = os.path.getsize(path)
    byte1, byte2 = 0, None

    m = re.search('(\d+)-(\d*)', range_header)
    g = m.groups()

    if g[0]: byte1 = int(g[0])
    if g[1]: byte2 = int(g[1])

    length = size - byte1
    if byte2 is not None:
        length = byte2 - byte1

    data = None
    with open(path, 'rb') as f:
        f.seek(byte1)
        data = f.read(length)

    response = Response(data,
                        206,
                        mimetype=mimetypes.guess_type(path)[0],
                        direct_passthrough=True)
    response.headers.add('Content-Range', 'bytes {0}-{1}/{2}'.format(byte1, byte1 + length - 1, size))

    return response
