var fs = require('fs');
var http = require('http');
var binary = require('binary');
var express = require('express');

var app = express();
  
app.get('/stamina', function (req, res) {

	res.writeHead(200, { 'Content-Type' : 'text/html' });

	fs.readFile('/dev/stamina', function (err, data) {
		if (err) {
			console.log(err);
		} else {
			var syscall = 0;

			binary.parse(data).loop(function(end, vars){
				this.word64lu('usage').word64lu('stack_min').word64lu('stack_max').tap(function (vars) {
					vars.syscall = syscall++;
					console.dir(vars);
				})
				if (this.eof()) end();
			})
		}

		res.end();
	})
})

var server = app.listen(3000, function () {
});