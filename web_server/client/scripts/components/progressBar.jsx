import React from "react";

class ProgressBar extends React.Component {

  render() {

    styles = {
      width : this.props.progress + "%"
    }

    return (
      <div class="progress">
        <div class="determinate" style={styles}></div>
      </div>
    );
  }

};

ProgressBar.propTypes = {
  progress : React.PropTypes.number,
}

ProgressBar.defaultProps = {
  progress: 0
}

export default ProgressBar;
