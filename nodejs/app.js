var fs = require('fs');
var http = require('http');
var binary = require('binary');
var express = require('express');

var app = express();
  
app.get('/stamina', function (req, res) {

    res.writeHead(200, { 'Content-Type' : 'text/html' });

    fs.open('/dev/stamina', 'r', function(status, fd) {
	if (status) {
		console.log(status.message);
		return;
	}
/*

  core dumps :(

	var buffer = new Buffer(16384);
	fs.read(fd, buffer, 0, 16384, 0, function(err, nbytes, buffer){

		binary.parse(buffer).loop(function(end, vars){
			if (this.eof()) {
				end();
			}
		});

		console.log(nbytes);
    		res.end();
	});
    });

*/

});

var server = app.listen(3000, function () {
});