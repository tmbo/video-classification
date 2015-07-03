import React from "react";
import Component from "./baseComponent.jsx";

class FileInput extends Component {

  constructor(props) {
    super(props);
    this.state = {fileNames : ""};
  }

  handleChange(evt) {
    const files = this.getFiles();
    let fileNames = [];

    for (var i=0; i < files.length; i++) {
      fileNames.push(files[i].name);
    }

    this.setState({fileNames : fileNames.join(", ")});
  }

  handleClick(evt) {
    this.refs.fileInput.getDOMNode().click();
  }

  getFiles() {
    return this.refs.fileInput.getDOMNode().files;
  }

  render() {

    return (
      <div className="file-field input-field">
        <div className="btn">
          <span>File</span>
          <input
            type="file"
            name="video"
            ref="fileInput"
            accept={this.props.fileFilter}
            onChange={this.handleChange.bind(this)} />
        </div>
        <div className="file-path-wrapper">
          <input
            readOnly
            value={this.state.fileNames}
            className="file-path validate"
            type="text"
            placeholder={this.props.placeholder}
            onClick={this.handleClick.bind(this)}
            ref="filePath"
             />
        </div>
      </div>
    );
  }

};

FileInput.propTypes = {
  placeholder : React.PropTypes.string,
  fileFilter : React.PropTypes.string,
}

FileInput.defaultProps = {
  placeholder: "",
  fileFilter: ""
}

module.exports = FileInput;
