var net = require('net');
var conn = require('connector');

var server = net.createServer(function(c) {
	var connector = new conn.Connector(c);
});

server.listen(8124, function() { //'listening' listener
	console.log('server bound');
});