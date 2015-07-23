import React from "react";
import Component from "./baseComponent.jsx";
import RecordRTC from "recordRTC";
import VideoActions from "../actions/videoActions"

class VideoCapture extends Component {

  constructor(props) {

    super(props);
    this.state = {
      isRecording : false,
      videoURL : null,
      videoOptions : {
        type: "video",
        video: {
          width: 640,
          height: 480
       },
       canvas: {
          width: 640,
          height: 480
       }
      }
    };

    navigator.getUserMedia = navigator.getUserMedia ||
                             navigator.webkitGetUserMedia ||
                             navigator.mozGetUserMedia;

    navigator.getUserMedia(
      {video : true},
      (mediaStream) => {

        this.mediaStream = mediaStream;
        this.refs.daVideo.getDOMNode().src = window.URL.createObjectURL(mediaStream);
      },
      (error) => console.error(error)
    );

  }

  handleClick() {

    if (this.state.isRecording == false) {

      this.recordRTC = RecordRTC(this.mediaStream, this.state.videoOptions)
      this.recordRTC.startRecording();
      this.updateState({isRecording : {$set : true}})

    } else {

      this.recordRTC.stopRecording((videoURL) => {

        this.updateState({videoURL : {$set : videoURL}})
        this.updateState({isRecording : {$set : false}})
        this.refs.daVideo.getDOMNode().src = videoURL;

        const recordedBlob = this.recordRTC.getBlob();
        VideoActions.uploadVideo(recordedBlob);

        // Stop video stream?
        this.mediaStream.stop();
      });
    }
  }

  render() {

    const buttonText = this.state.isRecording ? "Stop Recording & Submit" : "Start Recording";
    const videoStyle = {
      width : this.state.videoOptions.video.width,
      height : this.state.videoOptions.video.height
    };

    return (
      <div className="center-align">
        <div className="oval-mask-container">
          <video ref="daVideo" autoPlay muted style={videoStyle}/>
          <div className="oval-mask"/>
        </div>
        <div>
          <a className="waves-effect waves-light btn" onClick={this.handleClick.bind(this)}> {buttonText} <i className="material-icons right">videocam</i></a>
        </div>
      </div>
    )

  }

};


export default VideoCapture;



