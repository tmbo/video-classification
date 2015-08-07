import React from "react";
import ReactAddons from "react/addons"
import _ from "lodash";
import connectToStores from "alt/utils/connectToStores";

import Component from "../components/baseComponent.jsx";
import FileInput from "../components/fileInput.jsx";
import Spinner from "../components/spinner.jsx";
import ImageCard from "../components/imageCard.jsx";

import ImageStore from "../stores/imageStore.js";
import SyncActions from "../actions/syncActions.js";


class ImageUpload extends Component {

  static getStores() {
    return [ImageStore];
  }

  static getPropsFromStores() {
    return ImageStore.getState();
  }

  handleSubmit(evt) {

    evt.preventDefault();

    const file = this.refs.fileInput.getFiles()[0];
    if (file) {
      SyncActions.uploadImage(file)
    }
  }

  getSpinner() {

    if (this.props.isUploading) {
      return <Spinner size="small"/>;
    } else {
      return <span/>
    }
  }

  render() {

    const spinner =  this.getSpinner();
    const CSSTransitionGroup = ReactAddons.addons.CSSTransitionGroup;

    return (
      <div>
        <div className="row">
          <div className="col s12">
            <div className="card-panel">
              <div className="card-title">Upload an Image File for Classification</div>
              <form action="" onSubmit={this.handleSubmit.bind(this)} >
                <FileInput placeholder="Select or drop a picture." fileFilter="image/*" ref="fileInput" />
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
      </div>
    );
  }
}

export default connectToStores(ImageUpload);
