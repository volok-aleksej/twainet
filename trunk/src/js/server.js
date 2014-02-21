var servermodule = require('clientServerModule');
var ipcmodule = require('ipcModule');


var server = new servermodule.ClientServerModule("MainServer");
var app = new ipcmodule.IPCModule("App1");
server.StartServer(8124);
//server.StartAsCoordinator();
app.Start();

process.on('uncaughtException', function (err) {
  console.error(err.stack);
});