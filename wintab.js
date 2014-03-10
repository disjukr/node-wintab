var wintab = require('./build/Release/wintab');

setInterval(function () {
    wintab.peekMessage();
    if (wintab.checkOverlapped())
        wintab.enableContext();
}, 1);

function minLevel() {
    return wintab.minPressure();
}
exports.minLevel = minLevel;

function maxLevel() {
    return wintab.maxPressure();
}
exports.maxLevel = maxLevel;

function levels() {
    var min = wintab.minPressure();
    if (min === null)
        return null;
    var max = wintab.maxPressure();
    return max - min + 1;
}
exports.levels = levels;

function pressure() {
    var _levels = levels();
    if (_levels === null)
        return null;
    return (wintab.pressure() - wintab.minPressure()) / (_levels - 1);
}
exports.pressure = pressure;
