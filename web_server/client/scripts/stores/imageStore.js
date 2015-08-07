import _ from "lodash"
import alt from '../alt';
import SyncActions from '../actions/syncActions';
import API from "../lib/api"

class ImageStore {

  constructor() {
    this.bindActions(SyncActions);

    this.isUploading = false;
    this.isInvalidFile = false;
  }

  onUploadImage(imageFile) {

    const payload = {
      video : imageFile
    };

    API.postImage(payload)
    this.isUploading = true;
  }

  onReceiveImagePrediction() {
    this.isUploading = false;
    this.isInvalidFile = false;
  }

  onReceiveUploadError() {
    this.isUploading = false;
    this.isInvalidFile = true;
  }

};

export default alt.createStore(ImageStore, "ImageStore");