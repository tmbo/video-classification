import _ from "lodash"
import alt from "../alt";
import SyncActions from "../actions/syncActions";
import RouterActions from "../actions/routerActions";

class ResultStore {

  constructor() {
    this.bindActions(SyncActions);

    this.video = null;
    this.image = null;
    this.frames = null;
    this.fusionPredictions = null;
  }

  onReceiveVideoPrediction(response) {
    this.video = response.media;
    this.image = null;
    this.frames = response.frames;
    this.flows = response.flows;
    this.fusionPredictions = response.fusion_predictions;

    RouterActions.transition("result", {type : "video"})

  }

  onReceiveImagePrediction(response) {
    this.image = response.media;
    this.video = null;
    this.frames = response.frames;
    this.fusionPredictions = response.fusion_predictions;

    RouterActions.transition("result", {type : "image"})

  }

  static getFusionPredictions() {
    return this.getState().fusionPredictions;
  }



  static getGroupedFramePredictions() {
    const predictions = this.getState().frames;
    return this.getGroupedPredictions(predictions);
  }

  static getGroupedFlowPredictions() {
    const predictions = this.getState().flows;
    return this.getGroupedPredictions(predictions);
  }

  static getGroupedPredictions(probs_per_frame) {

    if (probs_per_frame) {
      const labels = _.chain(probs_per_frame)
        .pluck("predictions")
        .flatten()
        .pluck("label")
        .unique()
        .value();

      return _.transform(labels, (result, label) => {

        let per_frame = _.range(0, 16)
        for (let i in per_frame) {

          per_frame[i] = _.chain(probs_per_frame)
            .filter(frame => frame.frameNumber == i)
            .pluck("predictions")
            .flatten()
            .filter(pred => pred.label == label)
            .pluck("prob")
            .first()
            .value() || 0
        }

        result[label] = per_frame

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