import React from 'react';
import Router from 'react-router';
import routes from './routes.jsx';
import Styles from "../styles/main.less"

console.log("It works");

Router.run(routes, Handler => React.render(<Handler />, document.body));