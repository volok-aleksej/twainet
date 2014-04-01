var servermodule = require('clientServerModule');
var ipcmodule = require('ipcModule');


var server = new servermodule.ClientServerModule("MainServer");
var app = new ipcmodule.IPCModule("Test");
var app1 = new ipcmodule.IPCModule("Test");
server.StartServer(8124);
server.StartAsCoordinator();
//app.Start();
//app1.Start();
setTimeout(function(){ server.ChangeModuleName("NewMainServer");}, 5000);

process.on('uncaughtException', function (err) {
  console.error(err.stack);
});