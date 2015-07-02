import React from "react";
import API from "../lib/api"
import GeneralMixin from "../mixins/generalMixin.js"
import FileInput from "../components/fileInput.jsx"
import Spinner from "../components/spinner.jsx"

var Home = React.createClass({

  mixins : [GeneralMixin],

  getInitialState() {
    return {
      isUploading : false
    }
  },

  handleSubmit(evt) {

    evt.preventDefault();

    const reader = new FileReader();
    const file = this.refs.fileInput.getFiles()[0];

    if (file) {
      const payload = {
        video : file
      }

      this.updateState({isUploading :{$set : true}})
      API.postVideo(payload)
    }
  },

  getIcon() {
    if (this.state.isUploading) {
      return <Spinner/>
    } else {
      return <i className="material-icons right">send</i>
    }
  },

  render() {

    const icon =  this.getIcon();

    return (
      <div>
        <h1>Upload a video</h1>
        <div>

          <form action="" onSubmit={this.handleSubmit} >
            <FileInput placeholder="Upload a video file." fileFilter="video/*" ref="fileInput" />
            <button
              className="btn waves-effect waves-light"
              type="submit">
              Submit
              {icon}
          </button>
        </form>
        </div>
      </div>
    );
  }

});

module.exports = Home;