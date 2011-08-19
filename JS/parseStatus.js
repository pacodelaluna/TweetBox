/*
 TweetBox
 

 created 2011
 by AngeZanetti <@AngeZanetti> 
 for CoWorkingLille <http://coworkinglille.com>

licenced under GNU/GPL v2
*/

var fs = require('fs');

module.exports = function(idxStatus, type, method, newStatus, newKeyword) {
	if (method == 'get') {										// If the method is get
		var status = parseJSON(idxStatus, type);				// Parse the JSON find the status and return it
		return status;
	}
	else if (method == 'post') {
		writeJSON(idxStatus, newStatus, newKeyword);			// if method is post write a new JSON file with the updated status
	}
	else if (method == 'del') {									// else with del method delete the status
		delJSON(idxStatus);
	};
	
};

function parseJSON (id, type) {
	var data = fs.readFileSync('json/listeStatus.json');
	if (type == 'html') {									
		if (id == 'rien') {										// without any id it returns the whole list 
			return data;
		}
		else {													// else it show the status with the id in params
			var list = JSON.parse(data);
			console.log(list);
			var fullStatus = list.fullStatus[id];
			return fullStatus;
		};
	}
	else if (type == 'text/plain') {							// if it accept only text/plain then it return a keyword only
		list = JSON.parse(data);
		var keywordStatus = list.keywords[id];
		return keywordStatus;
	};	
} 

function writeJSON (id, theNewStatus, theNewKeyword) {			// Update the status with the new values passed in params
 
	var data = fs.readFileSync('json/listeStatus.json');
	var list = JSON.parse(data);
	list.fullStatus[id] = theNewStatus;
	list.keywords[id] = theNewKeyword;
	fs.writeFile('json/listeStatus.json',JSON.stringify(list, null, ' '), function (err) {
	  if (err) throw err;
	  console.log('It\'s saved!');
	});
	
}

function delJSON (id) {										// Delete the stauts passed in params
 
	var data = fs.readFileSync('json/listeStatus.json');
	 console.log('read');
	var list = JSON.parse(data);
	
	var output=[];											// This little trick is for delted the whole line it create an empty list
	  var j=0;
	  for(var i in list) {								
	    if (i!= id) {										// and copy the n element in the N+1 in our list
	      output[j] = list[i];
	      j++;
	    }
	  }
	
	fs.writeFile('json/listeStatus.json',JSON.stringify(list, null, ' '), function (err) {
	  if (err) throw err;
	  console.log('It\'s saved!');							// and finally write in a file
	});
	
} 