chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('topologic.html', {
    'bounds': {
      'width': 1280,
      'height': 720
    }
  });
});
