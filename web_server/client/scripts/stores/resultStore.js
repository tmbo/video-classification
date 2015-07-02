import _ from "lodash"
import alt from '../alt';
import VideoActions from '../actions/videoActions';

class ResultStore {

  constructor() {
    this.bindActions(VideoActions);
    this.predictions = []
  }

  onReceivePrediction(prediction) {
    this.predictions.push(predictions);
  }

};

export default alt.createStore(ResultStore, "ResultStore");