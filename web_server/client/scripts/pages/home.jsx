import React from 'react';
import Nyan from "../../images/nyan_cat.png"
import ItemList from '../components/itemList.jsx';

var Home = React.createClass({

  render() {
    return (
      <div>
	<h1>Home Area</h1>
	<ItemList />
	<img src="/images/nyan_cat.png"/>
      </div>
    );
  }

});

module.exports = Home;