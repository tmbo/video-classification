import _ from "lodash"
import alt from "../alt";
import VideoActions from "../actions/videoActions";
import RouterActions from "../actions/routerActions";

class ResultStore {

  constructor() {
    this.bindActions(VideoActions);

    this.video = null;
    this.frames = null;
  }

  onReceivePrediction(response) {
    this.video = response.video;
    this.frames = response.frames;

    RouterActions.transition("result")

  }

  static getPredictions() {
    const frames = this.getState().frames;
    if (frames) {
      return _.flatten(_.pluck(frames, "predictions"));
    } else {
      return null;
    }
  }

  static getGroupedPredictions() {
    const predictions = this.getPredictions();
    if (predictions) {
      const labels = _.unique(_.pluck(predictions, "label"));

      return _.transform(labels, (result, label) => {

        result[label] =
          _.chain(predictions)
           .filter(pred => pred.label == label)
           .pluck("prob")
           .value()
      }, {})
    } else {
      return null;
    }
  }

  static getFrameNumbers() {
    const frames = this.getState().frames;
    if (frames) {
      return _.pluck(frames, "frameNumber");
    } else {
      return null;
    }
  }

};

export default alt.createStore(ResultStore, "ResultStore");