import alt from "../alt";

class SyncActions {
  constructor() {
    this.generateActions(
      "receiveVideoPrediction",
      "receiveImagePrediction",
      "receiveUploadError",
      "uploadVideo",
      "uploadImage",
      "useExample"
    )
  }
}

export default alt.createActions(SyncActions)