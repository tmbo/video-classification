import alt from "../alt";

class VideoActions {
  constructor() {
    this.generateActions(
      "receivePrediction",
      "receiveUploadError",
      "uploadVideo",
      "useExample"
    )
  }
}

export default alt.createActions(VideoActions)