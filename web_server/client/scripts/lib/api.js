import FetchUtils from "./fetchUtils";
import VideoActions from "../actions/videoActions.js";

const BASE_URL = "http://localhost:9000"; 

module.exports = window.api = {

  /*
   * Video Upload
   */

  // deleteChapter: function(chapterId) {
  //   const url = `${BASE_URL}/api/v2/chapters/${chapterId}`;
  //   const options = {
  //     method: "DELETE",
  //     type: "json"
  //   };

  //   return FetchUtils.fetchJsonWithToken(url, accessToken, options)
  //     .then((data) => SyncActions.deleteChapter(chapterId));
  // },

  postVideo: function(content) {
    const url = `${BASE_URL}/api/upload`;
    const options = {
      method: "POST",
      body: content,
      type: "formdata"
    };

    return FetchUtils.fetchJson(url, options)
      .then((data) => VideoActions.receivePrediction(data));
  },

};
