import React from "react";
import connectToStores from "alt/utils/connectToStores";
import FileInput from "../components/fileInput.jsx"
import ProgressBar from "../components/progressBar.jsx"
import VideoStore from "../stores/videoStore.js"

class Home extends React.Component {

  static getStores() {
    return [VideoStore];
  }

  static getPropsFromStores() {
    return VideoStore.getState();
  }

  handleSubmit(evt) {

    evt.preventDefault();

    const reader = new FileReader();
    const file = this.refs.fileInput.getFiles()[0];

    if (file) {
      const payload = {
        video : file
      }
    }
  }

  getProgressBar() {
    if (this.props.isUploading) {
      uploadProgress = 50;
      return <ProgressBar progress={uploadProgress}/>;
    } else {
      return <span/>
    }
  }

  render() {

    const progressBar =  this.getProgressBar();

    return (
      <div>
        <h1>Upload a video</h1>
        <div>
          <form action="" onSubmit={this.handleSubmit.bind(this)} >
            <FileInput placeholder="Upload a video file." fileFilter="video/*" ref="fileInput" />
            {progressBar}
            <button
              className="btn waves-effect waves-light"
              type="submit">
              Submit
              <i className="material-icons right">send</i>
          </button>
        </form>
        </div>
      </div>
    );
  }
}

export default connectToStores(Home);
