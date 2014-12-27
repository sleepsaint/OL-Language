var OL = {};

OL.Source = {
	cursor: 0
};
OL.Source.debug = {};

OL.Source.debug.literal = function() {
	return this.value;
}

OL.Source.debug.path = function() {
	return this.root + "[" + this.keys.map(function(a){ return a.debug(); }).join(", ") + "]";
}

OL.Source.debug.fun = function() {
	return this.name.debug() + "(" + this.params.map(function(a){ return a.debug(); }).join(", ") + ")";
}

OL.Source.debug.negative = function() {
	return "!" + this.value.debug();
}

OL.Source.getToken = function() {
	var end = this.source.length;
	var start = this.cursor;
	var NONE = 0;
	var STRING = 1;
	var NUMBER = 2;
	var status = NONE;
	for (var i = this.cursor; i < end; ++i) {
		var c = this.source[i];
		switch(c) {
		case "~":
		case "!":
		case "@":
		case "^":
		case "(":
		case ")":
		case "{":
		case "}":
		case ",":
			if (status == NONE) {
				++this.cursor;
				return c;
			} else {
				this.cursor = i;
				return this.source.substring(start, i);
			}
		case ".":
			this.cursor = i;
			if (status == NONE) {
				++this.cursor;
				return c;
			} else if  (status == NUMBER) {
				break;
			} else if (status == STRING) {
				this.cursor = i;
				return this.source.substring(start, i);
			}	
		case " ":
			++this.cursor;
			break;
		case "#":
			if (status == NONE) {
				start = this.cursor;
				status = NUMBER;
			}
			break;
		default:
			if (status == NONE) {
				start = this.cursor;
				status = STRING;
			}
		}
	}
	this.cursor = end;
	return start == end ? null : this.source.substring(start);
}

OL.Source.match = function(expected) {
	if (expected == this.token) {
		this.token = this.getToken();
		return true;
	} else {
		return false;
	}
}

OL.Source.error = function(e) {
	console.log(e + " at " + this.cursor);
}

OL.Source.getValue = function() {
	return this.getLiteral() || this.getPath() || this.getFunction() || this.getNegative();
}

OL.Source.getLiteral = function() {
	if (!this.token) return;
	switch(this.token) {
	case "~":
	case "!":
	case "@":
	case "^":
	case "(":
	case ")":
	case "{":
	case "}":
	case ",":
	case ".":
		return;
	default:
		var literal = this.token;
		this.token = this.getToken();
		if (literal[0] == "#") {
			return {type:"literal", value:literal.substring(1) - 0, debug:this.debug.literal};
		} else {
			return {type:"literal", value:literal, debug:this.debug.literal};
		}
	}
}

OL.Source.getPath = function() {
	var root = this.token;
	if (!this.match("^") && !this.match("~") && !this.match("@")) return;
	var keys = [];
	while (this.match(".")) {
		var key = this.getKey();
		if (key) {
			keys.push(key);
		} else {
			this.error("can not match a key");
		}
	}
	return {type:"path", root:root, keys:keys, debug:this.debug.path}; 
}

OL.Source.getString = function() {
	var literal = this.getLiteral();
	if (literal) {
		if (literal.value.length) {
			return literal;
		} else {
			this.error("key can not be a number");
			return;
		}
	} 
}

OL.Source.getKey = function() {
	return this.getString() || this.getFragment() || this.getFunction();
}

OL.Source.getFragment = function() {
	if (!this.match("{")) return;
	var ret = this.getPath();
	if (ret) {
		if (this.match("}")) {
			return ret
		} else {
			this.error("can not match }")
		}
	} else {
		this.error("in {} must a path");
	}
}

OL.Source.getFunction = function() {
    if (!this.match("(")) return;
	var name = this.getValue();
	if (name) {
		var params = [];
		while (this.match(",")) {
			var param = this.getValue();
			if (param) {
				params.push(param);
			}
		}
		if (this.match(")")) {		
			return {type:"function", name:name, params:params, debug:this.debug.fun}
		} else {
			this.error("can not match )")
		}
	} else {
		this.error("function must have a name");
	}
}

OL.Source.getNegative = function() {
	if (!this.match("!")) return;
	var value = this.getValue();
	if (value) {
		return {type:"negative", value:value, debug:this.debug.negative};
	} else {
		this.error("can not match value for !")
	}
}



OL.parse = function(source) {
	var s = Object.create(this.Source);
	s.source = source;
	s.token = s.getToken();
	return s.getValue();
}

