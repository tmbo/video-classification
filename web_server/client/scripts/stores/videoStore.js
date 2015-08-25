import _ from "lodash"
import alt from '../alt';
import SyncActions from '../actions/syncActions';
import API from "../lib/api"

import VideoThumbnail1 from "../../images/examples/video1.jpg"
import VideoThumbnail2 from "../../images/examples/video2.jpg"
import VideoThumbnail3 from "../../images/examples/video3.jpg"


class VideoStore {

  constructor() {
    this.bindActions(SyncActions);

    this.isUploading = false;
    this.isInvalidFile = false;
    this.exampleVideos = [
      {
        id : 1,
        thumbnail : "/dist/images/examples/video1.jpg",
        title : "Archery",
        activity : "archery"
      },
      {
        id : 2,
        thumbnail : "/dist/images/examples/video2.jpg",
        title : "Wall Pushups",
        activity : "wallpushup"
      },
      {
        id : 3,
        thumbnail : "/dist/images/examples/video3.jpg",
        title : "Baby Crawling",
        activity : "babycrawling"
      }
    ];
  }

  static getExampleVideos() {
    return this.getState().exampleVideos;
  }

  onUploadVideo(videoFile) {

    const payload = {
      video : videoFile
    };

    API.postVideo(payload)
    this.isUploading = true;
  }

  onUseExample(videoId) {

    API.getPredictionForExample(videoId);
    this.isUploading = true;
  }

  onReceiveVideoPrediction() {
    this.isUploading = false;
    this.isInvalidFile = false;
  }

  onReceiveUploadError() {
    this.isUploading = false;
    this.isInvalidFile = true;
  }

};

export default alt.createStore(VideoStore, "VideoStore");