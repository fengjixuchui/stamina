var fs = require('fs');
var http = require('http');
var binary = require('binary');
var express = require('express');

var app = express();

app.set('views', './views')
app.set('view engine', 'jade');

//
// 1. read system call table (sync)
//

var syscallTable = new Object;
fs.readFileSync('syscalls.list').toString().split('\n').forEach(function (line) {
	if (!line) return;
	var s = line.split(" "); // __NR_read 0 ...
	syscallTable[ s[1] ] = s[0];
});

//
// 2. read system call usage information (async)
//

var syscallUsage = new Object;
app.get('/stamina', function (req, res) {

	res.writeHead(200, { 'Content-Type' : 'text/html' });

	syscallUsage = {};

	fs.readFile('/dev/stamina', function (err, data) {
		if (err) {
			console.log(err);
		} else {
			var syscall = 0;
			binary.parse(data).loop(function(end, vars){
				this.word64lu('usage').word64lu('stack_min').word64lu('stack_max').tap(function (vars) {
					syscallUsage[syscall] = {};
					syscallUsage[syscall]['usage'] = vars['usage'];
					syscallUsage[syscall]['stack_min'] = vars['stack_min'];
					syscallUsage[syscall]['stack_max'] = vars['stack_max'];
					syscall++;
				})
				if (this.eof()) end();
			})
		}

		console.dir(syscallUsage);

		res.end();
	})
})

app.get('/', function(req, res){
	res.render('index.jade');
});

var server = app.listen(3000, function () {
});