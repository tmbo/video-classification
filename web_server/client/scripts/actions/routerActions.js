import alt from "../alt";
import Router from "../lib/router";

class RouterActions {

  transition(nextPage, options) {

    Router.transitionTo("result", {stepUrl: nextPage}, options);

  }
}

export default alt.createActions(RouterActions)