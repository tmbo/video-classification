import React from "react";
import API from "../lib/api"
import GeneralMixin from "../mixins/generalMixin"

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
    const file = this.refs.fileInput.getDOMNode().files[0];

    if (file) {
      const payload = {
        video : file
      }

      this.updateState({isUploading :{$set : true}})
      API.postVideo(payload)
    }
  },

  render() {

    const icon = this.state.isUploading ? "uploading" : "send"

    return (
      <div>
        <h1>Upload a video</h1>
        <div>
          <form action="" onSubmit={this.handleSubmit} >
            <div className="file-field input-field">
              <input className="file-path validate" type="text"/>
              <div className="btn">
                <span>File</span>
                <input type="file" ref="fileInput"/>
              </div>
            </div>
            <button
              className="btn waves-effect waves-light"
              type="submit">
              Submit
              <i className="material-icons">{icon}</i>
          </button>
        </form>
        </div>
      </div>
    );
  }

});

module.exports = Home;