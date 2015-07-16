import React from "react";
import Router from 'react-router';
import routes from './routes.jsx';
// import MaterializeJS from "materialize-js";

import FavIcon from "../images/favicon-96x96.png";
import FavIcon2 from "../images/favicon.ico";

import Styles from "../styles/main.less";
import MaterializeCSS from "materialize-css";


Router.run(routes, Router.HistoryLocation, Handler => React.render(<Handler />, document.body));