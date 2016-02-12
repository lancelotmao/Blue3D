/*
 * test for http
 */
var http = require('http');
var fs = require('fs');

// var HOST = '127.0.0.1';
var HOST = '120.25.219.17';

var options = {
    host: HOST,
    port: 8888,
    path: '/upload?projectId=Houyi&type=fmwk',
    method: 'POST'
};

var req = http.request(options, function(res) {
  	console.log('STATUS: ' + res.statusCode);
 	  console.log('HEADERS: ' + JSON.stringify(res.headers));
  	res.setEncoding('utf8');
  	res.on('data', function (chunk) {
    	console.log('BODY: ' + chunk);
  	});
});

var stream = fs.createReadStream('Houyi.fmwk');
stream.on('data', function(data) {
  req.write(data);
});

stream.on('end', function() {
  req.end();
});
