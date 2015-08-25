import React from "react";
import ReactAddons from "react/addons"
import _ from "lodash";
import connectToStores from "alt/utils/connectToStores";

import Component from "../components/baseComponent.jsx";
import FileInput from "../components/fileInput.jsx";
import Spinner from "../components/spinner.jsx";
import ImageCard from "../components/imageCard.jsx";
import VideoCapture from "../components/videoCapture.jsx";

import VideoStore from "../stores/videoStore.js";
import SyncActions from "../actions/syncActions.js";


class VideoUpload extends Component {

  static getStores() {
    return [VideoStore];
  }

  static getPropsFromStores() {
    return VideoStore.getState();
  }

  handleSubmitVideo(evt) {

    evt.preventDefault();

    const file = this.refs.fileInput.getFiles()[0];
    if (file) {
      SyncActions.uploadVideo(file)
    }
  }

  handleClickExample(videoID, evt) {

    evt.preventDefault();
    if (!this.props.isUploading)
      SyncActions.useExample(videoID);

  }

  getSpinner() {

    if (this.props.isUploading) {
      return <Spinner size="small"/>;
    } else {
      return <span/>
    }
  }

  getExampleVideoCards() {

    const exampleVideos = VideoStore.getExampleVideos();
    return exampleVideos.map((video) => {
      const actions = [
        <a href="#" key={_.uniqueId()} onClick={this.handleClickExample.bind(this, video.id)}>Get Prediction</a>
      ];

      return (
        <div className="col s12 m4" key={video.title}>
          <div className="card-link" onClick={this.handleClickExample.bind(this, video.id)}>
            <ImageCard
              image={video.thumbnail}
              title={video.title}
              content={"Class: " + video.activity}
              actions={actions}
            />
          </div>
        </div>
      )
    });
  }

  render() {

    const spinner =  this.getSpinner();
    const exampleVideos = this.getExampleVideoCards();
    const CSSTransitionGroup = ReactAddons.addons.CSSTransitionGroup;

               // Webcam Video Recording
              // <div className="card-title">Capture a Facial Expression Video with your Webcam</div>
              // <VideoCapture ref="videoCapture"/>

    return (
      <div>
        <div className="row">
          <div className="col s12">
            <div className="card-panel">
              <div className="card-title">Upload a Video File for Classification</div>
              <form action="" onSubmit={this.handleSubmitVideo.bind(this)} >
                <FileInput placeholder="Select or drop a video file." fileFilter="video/*" ref="fileInput" />
                <div className="form-submit">
                  <button
                    className="btn waves-effect waves-light"
                    type="submit">
                    Submit
                    <i className="material-icons right">send</i>
                  </button>
                  <CSSTransitionGroup transitionName="spinner">
                    {spinner}
                  </CSSTransitionGroup>
                </div>
              </form>
            </div>
          </div>
        </div>
        <div className="row">
          {exampleVideos}
        </div>
      </div>
    );
  }
}

export default connectToStores(VideoUpload);
