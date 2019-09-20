var HTMLCanvasElement = {};
HTMLCanvasElement.prototype = {};

var document={};

function objToString (obj) {
    var str = '';
    for (var p in obj) {
        if (obj.hasOwnProperty(p)) {
            str += p + '::' + obj[p] + '\n';
        }
    }
    return str;
}

function createEvent(keyCode) {
	return {keyCode: keyCode};	
}

function createContext(width, height) {
	var context = {};
	context.fillStyle="#000000";
	
	// create a new context in C++ and get it's id
	context.nativeId = addNativeContext(width, height);
	
	context.clearRect = function(x, y, w, h){
		nativeClearRect(this.nativeId, x, y, w, h);
	};
	
	context.fillRect = function(x, y, w, h){
		nativeFillRect(this.nativeId, this.fillStyle, x, y, w, h);
	};
	
	context.drawImage = function(img, x, y){
		drawImageNative(img.context.nativeId, x, y);
	};
	return context;
}

function createCanvas(width, height){
	var canvas = {};
	canvas.width = width;
	canvas.height = height;
	canvas.context = createContext(width, height);
	canvas.getContext = function(contextType){return this.context;};
	 
	// Get display width and height from C++ engine
	canvas.parentNode = {};
	canvas.parentNode.clientWidth = getWidth();
	canvas.parentNode.clientHeight = getHeight();
	 
	return canvas;
}

 document.getElementById = function(id){
	if(id === 'separator') {
		return null;
	} else {
		return createCanvas(getWidth(), getHeight());
	}
}

document.getElementsByTagName = function(tagName){
	var res = [];
	return res;
}

document.createElement = function(tagName, options){
	return createCanvas(cellwidth, cellheight);
}

document.addEventListener = function(type, listener, useCapture){};

document.body = {};
document.body.style = {};
document.body.style.backgroundColor = '#000000';

document.URL = 'MyGame';