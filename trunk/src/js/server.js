var net = require('net');
var conn = require('clientServerConnector');

var server = net.createServer(function(c) {
	var connector = new conn.ServerConnector(c);
});

server.listen(8124, function() { //'listening' listener
	console.log('server bound');
});