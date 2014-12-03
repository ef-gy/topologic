jQuery(document).on('touchmove', function(e) {
    if (!jQuery(e.target).parents('.ui-panel-inner')[0]) {
        e.preventDefault();
    }
});

var setActiveDimension = Module.cwrap ('setActiveDimension', 'number', ['number']);
var forceRedraw = Module.cwrap('forceRedraw', null, []);
var setFlameColouring = Module.cwrap('setFlameColouring', null, ['number']);
var setIFSParameters = Module.cwrap('setIFSParameters', null, ['number', 'number', 'number', 'number', 'number']);
var setColour = Module.cwrap('setColour', null, ['number', 'number', 'number', 'number', 'number']);
var setFlameParameters = Module.cwrap('setFlameParameters', null, ['number']);
var setViewportSize = Module.cwrap('setViewportSize', null, ['number','number']);
var resetColourMap = Module.cwrap('resetColourMap', null, []);
var getJSON = Module.cwrap('getJSON', 'string', []);
var getSVG = Module.cwrap('getSVG', 'string', []);
var parseJSON = Module.cwrap('parseJSON', null, ['string']);
var interpretDrag = Module.cwrap('interpretDrag', 'number', ['number','number','number']);
var getModels = Module.cwrap('getModels', 'string', []);

var topologicMaxDepth = 7;
var topologicActiveDimension=3;

var topologicIFSIterations=4;
var topologicIFSFunctions=3;
var topologicIFSSeed=0;
var topologicIFSPreRotate=true;
var topologicIFSPostRotate=false;

var topologicFlameColouring=false;
var topologicFlameVariants=3;

var topologicColour =
    [[1,1,1,1],
     [0,0,0,0.8],
     [0,0,0,0.2]];

var topologicIgnoreHashChange = false;
var originalSettingsJSON = getJSON();
var originalSettings = JSON.parse(originalSettingsJSON);
var settings = JSON.parse(originalSettingsJSON);

function parseHash()
{
  if (topologicIgnoreHashChange) return;

  var json = window.location.hash.substring(1);
  try {
    JSON.parse(json);
    parseJSON(json);
  } catch (e) {
    try {
      console.error(e);
      json = decodeURIComponent(json);
      JSON.parse(json);
      parseJSON(json);
    } catch (e) {
      return;
    }
  }
  forceRedraw();

  var val = JSON.parse(getJSON());
  topologicIFSIterations     = val['iterations'];
  topologicIFSSeed           = val['seed'];
  topologicIFSFunctions      = val['functions'];
  topologicIFSPreRotate      = val['preRotate'];
  topologicIFSPostRotate     = val['postRotate'];
//  topologicIFSFlameColouring = val['flameColouring'];
  topologicIFSFlameVariants  = val['flameCoefficients'];

  topologicColour = [ val['background'].slice(1),
                      val['wireframe'].slice(1),
                      val['surface'].slice(1) ];

  topologicUpdateCurrentModelData();
}

window.addEventListener('hashchange', parseHash, false);

(function(){
    var x, y;
    jQuery('#canvas').on({ 'touchstart' : function(ev) {
        ev = ev.originalEvent.touches[0];
        x = ev.clientX;
        y = ev.clientY;
    }});

    jQuery('#canvas').on({ 'touchmove' : function(ev) {
        ev = ev.originalEvent.touches[0];

        interpretDrag(ev.clientX - x, ev.clientY - y, 0);

        x = ev.clientX;
        y = ev.clientY;

        forceRedraw();
    }});
})();

function topologicUpdateDimension()
{
    var output = document.getElementById('activeDimension');
    if (output)
    {
        output.textContent=topologicActiveDimension;
    }
    setActiveDimension(topologicActiveDimension);
}

function topologicUpdateCurrentModelData()
{
    if (topologicIFSIterations < 2)
    {
        topologicIFSIterations = 2;
    }
    if (topologicIFSFunctions < 2)
    {
        topologicIFSFunctions = 2;
    }
    if (topologicFlameVariants < 1)
    {
        topologicFlameVariants = 1;
    }
    output = document.getElementById('IFSIterations');
    if (output)
    {
        output.value=topologicIFSIterations;
    }
    output = document.getElementById('IFSSeed');
    if (output)
    {
        output.value=topologicIFSSeed;
    }
    output = document.getElementById('IFSFunctions');
    if (output)
    {
        output.value=topologicIFSFunctions;
    }
    output = document.getElementById('FlameVariants');
    if (output)
    {
        output.value=topologicFlameVariants;
    }
}

function topologicUpdateCurrentModel()
{
    topologicUpdateCurrentModelData();

    setIFSParameters(topologicIFSIterations, topologicIFSSeed, topologicIFSFunctions, topologicIFSPreRotate ? 1 : 0, topologicIFSPostRotate ? 1 : 0);
    setFlameColouring(topologicFlameColouring);
    setFlameParameters(topologicFlameVariants);

    setColour(0, topologicColour[0][0], topologicColour[0][1], topologicColour[0][2], topologicColour[0][3]);
    setColour(1, topologicColour[1][0], topologicColour[1][1], topologicColour[1][2], topologicColour[1][3]);
    setColour(2, topologicColour[2][0], topologicColour[2][1], topologicColour[2][2], topologicColour[2][3]);

    forceRedraw();

    topologicIgnoreHashChange = true;
    window.location.hash = getHash();
    topologicIgnoreHashChange = false;
}

function arrayEquals (a, b) {
  if (Array.isArray(a)) {
    if (Array.isArray(b)) {
      if (a.length === b.length) {
        for (j in a) {
          if (!arrayEquals(a[j], b[j])) {
            return false;
          }
        }
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return a == b;
  }
}

function getHash() {
  var data = JSON.parse(getJSON());

  for (i in data) {
    if (arrayEquals(originalSettings[i], data[i])) {
      delete data[i];
    } else if ((i === 'camera') || (i === 'transformation')) {
      var t = [];
      for (k in data[i]) {
        if (!arrayEquals(originalSettings[i][k], data[i][k])) {
          t.push(data[i][k]);
        }
      }
      data[i] = t;
    }
  }

  return JSON.stringify(data);
}

function updateHash(silent)
{
    silent = typeof silent !== 'undefined' ? silent : true;

    topologicIgnoreHashChange = silent;
    window.location.hash = getHash();
    topologicIgnoreHashChange = !silent;
}

(function() {
    var canvas = document.getElementById('canvas');

    if (!canvas)
    {
        return;
    }

    window.addEventListener('resize', resizeCanvas, false);
    window.setTimeout(resizeCanvas, 100);

    function resizeCanvas()
    {
        canvas.width  = window.innerWidth;
        canvas.height = window.innerHeight;
        setViewportSize(canvas.width, canvas.height);
    }

    resizeCanvas();
})();

function getLink()
{
  return 'https://dee.pe/r:' + encodeURIComponent(getHash());
}

function getEmbed()
{
  return '<iframe height="720" width="1280" src=\'' + getLink() + '\'></iframe>'
       + '<!-- iframe code generated by Topologic/V10; hosted on https://ef.gy/ -->';
}

jQuery(document).ready(function() {
  models = JSON.parse(getModels());
  models['models'].forEach(function(value) {
    jQuery('#model').append(jQuery('<option></option>').text(value));
  });
  models['formats'].forEach(function(value) {
    jQuery('#coordinateFormat').append(jQuery('<option></option>').text(value));
  });

  for (s in settings) (function(s) {
    jQuery('#' + s).change(function() {
      console.log(s, '=', jQuery(this).val());
      settings[s] = jQuery(this).val();
      if (typeof originalSettings[s] === 'number') {
        settings[s] = parseFloat(settings[s]);
      }
      parseJSON(JSON.stringify(settings));
      updateHash();
    }).val(settings[s]);
  })(s);
});
