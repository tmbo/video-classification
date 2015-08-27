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

  static getFramesPredictions() {
    const frames = this.getState().frames;
    if (frames) {
      return _.flatten(_.pluck(frames, "predictions"));
    } else {
      return null;
    }
  }

  static getFlowPredictions() {
    const flows = this.getState().flows;
    if (flows) {
      return _.flatten(_.pluck(flows, "predictions"));
    } else {
      return null;
    }
  }

  static getGroupedFramePredictions() {
    const predictions = this.getFramesPredictions();
    return this.getGroupedPredictions(predictions);
  }

  static getGroupedFlowPredictions() {
    const predictions = this.getFlowPredictions();
    return this.getGroupedPredictions(predictions);
  }

  static getGroupedPredictions(predictions) {
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