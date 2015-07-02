const ReactUtils = {

  updateState(query) {
    this.setState(React.addons.update(this.state, query));
  }
}

export default ReactUtils