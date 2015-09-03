import FetchUtils from "./fetchUtils";
import SyncActions from "../actions/syncActions.js";

const API = {

  postVideo: function(content) {
    const url = "/api/upload_video";
    const options = {
      method: "POST",
      body: content,
      type: "formdata"
    };

    return FetchUtils.fetchJson(url, options)
      .then(
        (data) => SyncActions.receiveVideoPrediction(data),
        (error) => SyncActions.receiveUploadError(error)
      );
  },

  getPredictionForExample(exampleId) {
    const url = `/api/example/${exampleId}`;

    return FetchUtils.fetchJson(url)
      .then((data) => SyncActions.receiveVideoPrediction(data));
  },

  postImage: function(content) {
    const url = "/api/upload_image";
    const options = {
      method: "POST",
      body: content,
      type: "formdata"
    };

    return FetchUtils.fetchJson(url, options)
      .then(
        (data) => SyncActions.receiveImagePrediction(data),
        (error) => SyncActions.receiveUploadError(error)
      );
  },
};

export default API