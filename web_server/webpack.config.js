var ExtractTextPlugin = require("extract-text-webpack-plugin");
var nodePath = "node_modules/";

module.exports = {

  context: __dirname + "/client",
  entry: {
    app: "./scripts/main.jsx",
    html: "./index.html"
  },

  output: {
    path: __dirname + "/dist",
    filename: "[name].js",
    sourceMapFilename: "[file].map",
    publicPath: __dirname + "/dist/"
  },

  resolve: {
    cache: true,
    alias: {
      "lodash" : "lodash",
    },
    extensions: ['', '.js', '.json', '.coffee']
   },

  module: {
    loaders: [
      {
	test: /\.jsx?$/,
	exclude: /node_modules/,
	loaders: ["babel-loader"],
      },
      {
	test: /\.html$/,
	loader: "file?name=[name].[ext]",
      },
      {
	test: /\.less$/,
	loader: ("style!css!less"), // ExtractTextPlugin.extract()
      },
      {
	test: /\.(png|jpg)$/,
	loader: 'file-loader?name=images/[name].[ext]'
      },
    ],

  },
  plugins: [
    new ExtractTextPlugin("[name].css", {
      allChunks: true
    })
  ],

  devtool: "source-map",
}