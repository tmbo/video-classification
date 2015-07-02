import _ from "lodash"
import alt from '../alt';
import VideoActions from '../actions/videoActions';
import API from "../lib/api"


class VideoStore {

  constructor() {
    this.bindActions(VideoActions);
  }

  onUploadVideo() {
      API.postVideo(payload)
      this.isUploading = true;
  }

  onReceivePrediction() {
    this.isUploading = false;
  }

};

export default alt.createStore(VideoStore, "VideoStore");