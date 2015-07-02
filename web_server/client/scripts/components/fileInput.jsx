import React from "react";
import GeneralMixin from "../mixins/generalMixin"

var FileInput = React.createClass({

  mixins : [GeneralMixin],

  propTypes: {
    placeholder : React.PropTypes.string,
    fileFilter : React.PropTypes.string,
  },

  getDefaultProps() {
    return {
      placeholder: "",
      fileFilter: ""
    };
  },

  getInitialState() {
    return {
      fileNames : ""
    }
  },

  handleChange(evt) {
    const files = this.getFiles();
    let fileNames = [];
    for (var i=0; i < files.length; i++) {
      fileNames.push(files[i].name);
    }
    this.updateState({fileNames : {$set : fileNames.join(", ")}})
  },

  handleClick(evt) {
    this.refs.fileInput.getDOMNode().click();
  },

  getFiles() {
    return this.refs.fileInput.getDOMNode().files;
  },

  render() {
    console.log("fuuu");

    return (
      <div className="file-field input-field">
	<div className="btn">
	  <span>File</span>
	  <input
	    type="file"
	    name="video"
	    ref="fileInput"
	    accept={this.props.fileFilter}
	    onChange={this.handleChange} />
	</div>
	<div className="file-path-wrapper">
	  <input
	    value={this.state.fileNames}
	    className="file-path validate"
	    type="text"
	    placeholder={this.props.placeholder}
	    onClick={this.handleClick}
	    ref="filePath"
	     />
	</div>
      </div>
    );
  }

});

module.exports = FileInput;
