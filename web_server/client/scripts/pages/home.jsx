import React from "react";
import ReactAddons from "react/addons"
import _ from "lodash";
import connectToStores from "alt/utils/connectToStores";
import Component from "../components/baseComponent.jsx";
import Tabs from "../components/tabs.jsx";
import VideoUploadPage from "../pages/videoUpload.jsx";
import ImageUploadPage from "../pages/imageUpload.jsx";
import VideoStore from "../stores/videoStore.js";


class Home extends Component {

  static getStores() {
    return [VideoStore];
  }

  static getPropsFromStores() {
    return VideoStore.getState();
  }

  getErrorPanel() {

    if (this.props.isInvalidFile)
      return (
         <div className="row">
          <div className="col s12">
            <div className="card-panel red">
              <p className="white-text">
                <i className="material-icons">error</i>
                 You uploaded an invalid file. Only image and video files are allowed. (jpeg, bmp, png, avi, mpg, mpeg, mkv, webm, mp4)
              </p>
            </div>
          </div>
        </div>
      );
  }

  render() {

    const errorPanel = this.getErrorPanel();

    return (
      <div className="home-page">
        {errorPanel}
        <Tabs headers={["Video Upload", "Image Upload"]} className="z-depth-1">
          <VideoUploadPage />
          <ImageUploadPage />
        </Tabs>
      </div>
    );
  }
}

export default connectToStores(Home);
