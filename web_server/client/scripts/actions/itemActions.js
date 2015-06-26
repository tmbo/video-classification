import alt from '../alt';

class ItemActions {
  constructor() {
    this.generateActions(
      'create',
      'updateText',
      'loadItems'
    )
  }
}

module.exports = alt.createActions(ItemActions)