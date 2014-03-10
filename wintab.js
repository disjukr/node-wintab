var wintab = require('./build/Release/wintab');

setInterval(wintab.peekMessage, 0);

setInterval(function () {
    var p = wintab.pressure();
    var line = '';
    if (p != null) {
        p = (p / 20) | 0;
        while (p-- > 0)
            line += '*';
    }
    console.log(line);
}, 0);
