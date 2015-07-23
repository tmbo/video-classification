import FetchUtils from "./fetchUtils";
import VideoActions from "../actions/videoActions.js";

const BASE_URL = "http://localhost:9000"; 

const API = {

  postVideo: function(content) {
    const url = `${BASE_URL}/api/upload`;
    const options = {
      method: "POST",
      body: content,
      type: "formdata"
    };

    return FetchUtils.fetchJson(url, options)
      .then(
        (data) => VideoActions.receivePrediction(data),
        (error) => VideoActions.receiveUploadError(error)
      );
  },

  getPredictionForExample(exampleId) {
    const url = `${BASE_URL}/api/example/${exampleId}`;

    return FetchUtils.fetchJson(url)
      .then((data) => VideoActions.receivePrediction(data));
  },
};

export default API