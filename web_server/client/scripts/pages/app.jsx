import React from 'react';
import { RouteHandler } from 'react-router';
import Header from '../components/header.jsx'

var App = React.createClass({

  render() {

    return (
      <div>
        <Header />
        <div className="container">
          <div className="row">
            <RouteHandler/>
          </div>
        </div>
      </div>
    );
  }

});

module.exports = App;