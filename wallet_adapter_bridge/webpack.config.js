
const webpack = require('webpack');
module.exports = {
  mode: "production",
  resolve: {
    fallback: {
      crypto: false,
      zlib: false,
      url: false,
      https: false,
      http: false,
      stream: false,
      require: false
    },
  },
  entry: './src/index.js',
  output: {
    path: __dirname + '/dist',
    filename: 'index_minified.js',
  },
  plugins: [
    new webpack.optimize.LimitChunkCountPlugin({
      maxChunks: 1,
    }),
  ],
};
