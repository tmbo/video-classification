import _ from "lodash"
import alt from '../alt';
import ItemActions from '../actions/itemActions';

var todoStore = alt.createStore(class TodoStore {
  constructor() {
    this.bindActions(ItemActions);
    this.myData = [];
  }

  onCreate(text) {
    // ...
  }

  onLoadItems() {
    this.data = _.range(1, 20);
  }

  static isSomething() {
    // ...
  }
})

module.exports = todoStore