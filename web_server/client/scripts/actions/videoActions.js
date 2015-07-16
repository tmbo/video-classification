import alt from "../alt";

class VideoActions {
  constructor() {
    this.generateActions(
      "receivePrediction",
      "uploadVideo"
    )
  }
}

module.exports = alt.createActions(VideoActions)