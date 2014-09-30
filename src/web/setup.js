
var Module = {
  arguments: ['topologic-webgl'].concat(window.location.hash.substring(1).match(/[^+\s]+/g)),
  preRun: [],
  postRun: [],
  print: console.log,
  printErr: function(text) {
    text = Array.prototype.slice.call(arguments).join(' ');
    console.log(text);
  },
  canvas: document.getElementById('canvas'),
  TOTAL_MEMORY: 268435456,
  doNotCaptureKeyboard: true
};
