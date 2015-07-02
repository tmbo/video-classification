import React from 'react';
import { RouteHandler } from 'react-router';
import Header from '../components/header.jsx'

class App extends React.Component {

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

};

export default App;
