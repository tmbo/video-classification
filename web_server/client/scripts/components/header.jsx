import React from "react";
import { Link } from "react-router";
import Logo from "../../images/apple-icon-72x72.png";

class Header extends React.Component {

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
              <li><Link to="home"><i className="material-icons left">launch</i>Upload video</Link></li>
              <li><Link to="info"><i className="material-icons left">dashboard</i>Result</Link></li>
            </ul>
          </div>
        </div>
      </nav>
    );
  }

};

export default Header;
