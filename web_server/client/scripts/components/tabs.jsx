import React from "react";
import _ from "lodash";
import Component from "./baseComponent.jsx";

class Tabs extends Component {

  constructor() {
    super();

    this.state = {
      activeTab : 0
    }
  }

  getTabContent() {

    const activeTab = this.state.activeTab;
    const hasEnoughChildren = _.isArray(this.props.children) ? this.props.headers.length == this.props.children.length : false;

    if (hasEnoughChildren) {

      return this.props.children[activeTab];
    } else {
      console.warn("The number of tab headers and tab content children does nnot match.");
      return <span/>;
    }
  }

  changeTab(index) {

    this.updateState({
      activeTab : {$set : index}
    });
  }

  getIndicatorStyle() {

    const left = 15 * this.state.activeTab + "%";

    return {
      width : 15+"%",
      left : left
    }
  }


  getTabHeaders() {
    return this.props.headers.map((header, index) => {

      const styles = React.addons.classSet({
        "active" : this.state.activeTab == index
      })

      return (
        <li className="tab col s3" key={index}>
          <a onClick={this.changeTab.bind(this, index)} className={styles}>
            {header}
          </a>
        </li>
      )
    });
  }

  render() {

    const headers = this.getTabHeaders();
    const tabContent = this.getTabContent();
    const indicatorStyle = this.getIndicatorStyle();

    return (
      <div>
        <ul className={"tabs " + this.props.className}>
          {headers}
          <div className="indicator" ref="indicator" style={indicatorStyle}></div>
        </ul>
        {tabContent}
      </div>
    )
  }
};

Tabs.propTypes = {
  headers : React.PropTypes.array.isRequired
};

Tabs.defaultProps = {
};


export default Tabs;
