import React from "react";
import ReactAddons from "react/addons.js";

function componentDidRender() {
  if (this.componentDidRender) {
    this.componentDidRender();
  }
}

module.exports = {
  updateState(query) {
    this.setState(React.addons.update(this.state, query));
  },

  componentDidMount : componentDidRender,

  componentDidUpdate: componentDidRender
};
