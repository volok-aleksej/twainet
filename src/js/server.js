var module = require('clientServerModule');

var server = new module.ClientServerModule("MainServer");
server.StartServer(8124);

process.on('uncaughtException', function (err) {
  console.error(err.stack);
});