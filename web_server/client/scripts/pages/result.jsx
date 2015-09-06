import React from "react";
import connectToStores from "alt/utils/connectToStores";
import LineChart from "../components/linechart.jsx";
import BarChart from "../components/barchart.jsx";
import Component from "../components/baseComponent.jsx";
import ResultStore from "../stores/resultStore"
import ColorStore from "../stores/colorStore"

class Result extends Component {

  static getStores() {
    return [ResultStore];
  }

  static getPropsFromStores() {
    return ResultStore.getState();
  }

  onDataClicked(datum) {

    const video = this.props.video

    // Calculate video timepoint from frame number (datum.x)
    const timepoint = datum.x / video.framerate
    this.refs.video.getDOMNode().currentTime = timepoint;
  }

  getBarChartData() {

    const fusionPredictions = ResultStore.getFusionPredictions();
    const columns = _.chain(fusionPredictions)
      .sortByOrder((pred) => pred.prob, ["desc"])
      .transform((result, obj) => {
        result.push([obj.label, obj.prob])
      }, [])
      .value();

    const colors = _.chain(fusionPredictions)
      .pluck("label")
      .transform((result, value) => result[value] = ColorStore.getColorForLabel(value))
      .value();

    return {
      columns : columns,
      colors : colors
    }

  }

  getLineChartData(groupedPredictions) {

    const frameNumbers = ["frameNumber"].concat(ResultStore.getFrameNumbers());

    let columns = _.map(groupedPredictions, (value, key) => [key].concat(value));
    columns.push(frameNumbers);

    const colors = _.mapValues(groupedPredictions, (value, key) => ColorStore.getColorForLabel(key))

    return {
      x : "frameNumber",
      columns : columns,
      colors : colors
    }
  }

  onVideoHover(evt) {

    const videoElement = this.refs.video.getDOMNode();

    if(evt.type === "mouseenter") {
      videoElement.setAttribute("controls", null);
    } else if(evt.type === "mouseleave") {
      videoElement.removeAttribute("controls");
    }
  }

  isVideoPrediction() {
    return this.props.query.type == "video";
  }

  getPerFramePredictionPanel() {

    if (!this.isVideoPrediction()) return null;

    const groupedPredictions = ResultStore.getGroupedFramePredictions();
    const lineChartData = this.getLineChartData(groupedPredictions);

    return (
      <div className="row">
        <div className="col s12">
          <div className="card-panel">
            <h3 className="card-title">Top 5 Predicted Labels per Frame (Spatial)</h3>
            <LineChart data={lineChartData} onDataClick={this.onDataClicked.bind(this)} />
          </div>
        </div>
      </div>
    )
  }

  getPerFlowPredictionPanel() {

    if (!this.isVideoPrediction()) return null;

    const groupedPredictions = ResultStore.getGroupedFlowPredictions();
    const lineChartData = this.getLineChartData(groupedPredictions);

    return (
      <div className="row">
        <div className="col s12">
          <div className="card-panel">
            <h3 className="card-title">Top 5 Predicted Labels per Frame (Flow)</h3>
            <LineChart data={lineChartData} onDataClick={this.onDataClicked.bind(this)} />
          </div>
        </div>
      </div>
    )
  }

  getMediaContainer() {

    if (this.isVideoPrediction()) {
      return (
        <video
          ref="video"
          src={this.props.video.url}
          className="responsive-video valign"
          onMouseEnter={this.onVideoHover.bind(this)}
          onMouseLeave={this.onVideoHover.bind(this)}
          loop
          />
      );

    } else {
      return <img src={this.props.image.url} className="responsive-img"/>;
    }
  }

  render() {

    const framePredictionPanel = this.getPerFramePredictionPanel();
    const flowPredictionPanel = this.getPerFlowPredictionPanel();
    const mediaContainer = this.getMediaContainer();

    return (
      <div className="result-page">
        <div className="row">
          <div className="col s12 m6">
            <div className="card-panel teal video-panel valign-wrapper">
              {mediaContainer}
            </div>
          </div>
          <div className="col s12 m6">
            <div className="card-panel center-align prediction-panel">
              <BarChart data={this.getBarChartData()} />
            </div>
          </div>
        </div>
        {framePredictionPanel}
        {flowPredictionPanel}
      </div>
    );
  }

};

export default connectToStores(Result);

