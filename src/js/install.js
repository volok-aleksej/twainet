var https = require('https');
var fs = require('fs');

https.get('https://codeload.github.com/bigeasy/udt/zip/master', function(res) {
	var stream = fs.createWriteStream("node_modules\\udt.zip");
	stream.once('open', function(fd) {
		res.on('data', function(d) {
			stream.write(d);
		});
		res.on('close', function() {
			console.log("close");
			stream.end();
		});
	});
}).on('error', function(e) {
	console.error(e);
});