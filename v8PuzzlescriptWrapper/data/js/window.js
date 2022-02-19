var console = {};
console.log = function(t){
	print(t);
};

var window={};

var setInterval = window.setInterval = function (updateFunc, deltaTime) {

}

window.addEventListener = function(type, listener, useCapture){};
window.setInterval = setInterval;
window.console = console;

var localStorage = {};
localStorage.removeItem = function (item) {
    delete this[item];
}

localStorage.setItem = function(key,value) {
	this[key]=value;
}

localStorage.getItem = function(key) {
	return this[key] ? this[key] : null;
}

// Saving current progression. Called on quitting native c++ program
localStorage.serialize = function(){
	var data = '';
	for(var key in this){
		if(typeof this[key] !== 'function' && typeof this[key] !== 'undefined')
		{
			data += 'localStorage.' + key + '=' + this[key] + ';\n';
		}
	}

   return data;
}

window.localStorage = localStorage;

