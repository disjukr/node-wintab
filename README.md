Node-wintab
===
node.js wintab binding for getting wacom tablet pressure data.

Windows only.

Install
---
### prerequisite

 * python 2.x
 * MSVC++
 * node-gyp

### run
```
npm install node-wintab
```

Usage
---

```
var wintab = require('node-wintab');
setInterval(function () {
    console.log(wintab.pressure());
}, 1);
```
