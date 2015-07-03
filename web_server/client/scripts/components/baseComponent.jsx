import React from "react";

class BaseComponent extends React.Component {

  componentWillMount() {

    // Auto-bind all methods to it#s instance
    const methodNames = Object.getOwnPropertyNames(this.constructor.prototype).filter(prop => typeof this[prop] === "function")

    methodNames.forEach(name => {
      this[name] = this[name].bind(this);
    });
  }

};

export default BaseComponent;
