// Listen2EyeTribe.js
// Listen to Eye Tribe tracker 

// send JSON frames to socket.io
// 
// Ahmed Abdelali <aabdelali@qf.org.qa>
// Last update : 09/28/2014  06:38 PM
//

var net = require('net')
	util = require('util'),
    io = require('socket.io').listen(8080, {log: false}),  // socket server/port
	connectionOptions = {
		ip: 'localhost',  // Eye Tribe Server
		port: 6555		  // Eye Tribe port
	}; 
var database;
var extra = database; 

var date;
var milliseconds;
var csv1;
var csv2 = csv1;
var converter = require('json-2-csv');
var json2csvCallback = function (err, csv) {
    if (err) throw err;
    console.log(csv);
};

var socket = net.createConnection(connectionOptions, function() {
	setInterval(function() {
		socket.write(JSON.stringify({"category": "heartbeat"}));
	}, 200);

	console.log('Socket on port '+connectionOptions.port+' (TheEyeTribe server) connected');
	socket.on('error', function(data) {
		console.log('TheEyeTribe error', data);
	})

	socket.on('close', function(data) {
		console.log('TheEyeTribe close');
	})

	socket.on('data', function(data) {
		var fixed="";
		try {
			// Parse json data
			console.log(data);
			data = JSON.parse(data);
			console.log(data.values.frame.avg.x+' ' + data.values.frame.avg.y +'  '+ data.values.frame.time);

			if(data.values && data.values.frame) {
				console.log(JSON.stringify(data.values.frame));
				// send the selected json data
				//io.sockets.emit('message',JSON.stringify(data.values.frame));
				database = JSON.stringify(data.values.frame.avg) + ', ' + JSON.stringify(data.values.frame.fix);
			
				date = new Date(data.values.frame.timestamp);
				milliseconds = date.getTime();

			}
			// for (var x = 0; x < database.length; x++)
				// {
					// var c = database.charAt(x);
					// if (c == ':'){
						// fixed = fixed + ',';
					// }
					// else if (c == '{' || c == '}') {
					// fixed = fixed;}
					// else {fixed = fixed + c;}

				// }
						fixed = data.values.frame.avg.x + ', ' + data.values.frame.avg.y + ', ' + JSON.stringify(data.values.frame.fix) + ', ' + "'" + milliseconds + "'" +'\r\n';		

				
		} catch(e) {
		console.error('Malformed JSON', e);
		}
		extra = database;
		var fs = require('fs');


	//fs.appendFile('./extra.txt', extra, function (err) {
    // if (err) {
      // console.log('There has been an error saving your configuration data.');
      // console.log(err.message);
      // return;
    // }
    // console.log('Configuration saved successfully.')
  // });
  	fs.appendFile('./Alaa_Test1.csv', fixed, function (err) {
    if (err) {
      console.log('There has been an error saving your configuration data.');
      console.log(err.message);
      return;
    }
    console.log('Configuration saved successfully.')
  });
	})
	
	socket.write(JSON.stringify({
		category: 'tracker',
		request: 'set',
	values: {push: true}
	}));
}); 

socket.setEncoding('utf8');
 