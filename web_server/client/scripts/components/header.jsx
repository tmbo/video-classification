import React from "react";
import { Link } from "react-router";
import Logo from "../../images/apple-icon-72x72.png";

var Header = React.createClass({

  render() {
    return (
      <nav>
        <div className="nav-wrapper">
          <div className="col s12">
            <a href="#" className="brand-logo">
              <img src="../../images/apple-icon-72x72.png" width="48px" height="48px"/>
              Video Classifier
            </a>
            <ul className="right hide-on-med-and-down">
              <li><Link to="home">Home</Link></li>
              <li><Link to="info">Info</Link></li>
            </ul>
          </div>
        </div>
      </nav>
    );
  }

});

module.exports = Header;