import React from "react";

var ProgressBar = React.createClass({

  mixins : [GeneralMixin],

  propTypes: {
    progress : React.PropTypes.number,
  },

  getDefaultProps() {
    return {
      progress: "",
    };
  },

  render() {

    styles = {
      width : this.props.progress
    }

    return (
      <div class="progress">
        <div class="determinate" style={styles}></div>
      </div>
    );
  }

});

module.exports = ProgressBar;