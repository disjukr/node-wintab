var wintab = require('./build/Release/wintab');

setInterval(wintab.peekMessage, 0);

setInterval(function () {
    console.log(wintab.pressure());
}, 100);
