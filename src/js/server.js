var servermodule = require('clientServerModule');
var ipcmodule = require('ipcModule');

var app = new ipcmodule.IPCModule("Test");
app.Start();

process.on('uncaughtException', function (err) {
  console.error(err.stack);
});
