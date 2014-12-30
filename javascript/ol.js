var OL = {};
OL.Source = {};
OL.Source.debug = {};
OL.Source.lookup = {};
OL.fun = {};

OL.Source.debug.literal = function() {
	return this.value;
}

OL.Source.debug.path = function() {
	return this.root + this.keys.map(function(a){ return "[" + a.debug() + "]";}).join("");
}

OL.Source.debug.list = function() {
	return this.head.debug() + "(" + this.tail.map(function(a){ return a.debug(); }).join(", ") + ")";
}

OL.Source.debug.negative = function() {
	return "!{" + this.value.debug() + "}";
}

OL.Source.debug.quote = function() {
	return "#" + this.value.debug();
}

OL.Source.lookup.literal = function() {
	return this.value;
}

OL.autoLookup = function(root, temp, now, current) {
	while (current && current.length) {
		var a = OL.parse(current);
		if (a) {
			current = a.lookup(root, temp, now);
		} else {
			return;
		}
	}
	return current;	
}

OL.Source.lookup.path = function(root, temp, now) {
	var current;
	switch(this.root) {
	case "^":
		current = root;
		break;
	case "~":
		current = temp;
		break;
	case "@":
		current = now;
		break;
	default:
		return;
	}
	for (var i in this.keys) {
		var value = this.keys[i].lookup(root, temp, now);
		current = OL.autoLookup(root, temp, now, current);
		if (current instanceof Array) {
			current = current[parseInt(value)];
		} else if (current instanceof Object) {
			current = current[value];
		} else {
			return;
		}
	}
	return current;
}

OL.Source.lookup.list = function(root, temp, now) {
	var funName = OL.fun[this.head.lookup(root, temp, now)];
	if (funName) {
		return funName(this.tail.map(function(a){ return a.lookup(root, temp, now); }), root, temp, now);
	}
}

OL.Source.lookup.negative = function(root, temp, now) {
	return !this.value.lookup(root, temp, now);
}

OL.Source.lookup.quote = function(root, temp, now) {
	return this.value;
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
		case "#":
			if (status == NONE) {
				++this.cursor;
				return c;
			} else {
				this.cursor = i;
				return this.source.substring(start, i);
			}
		case ".":
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
		case "$":
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
	return (start == end || status == NONE) ? null : this.source.substring(start);
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
	//only log the first error, because other errors are caused by it.
	if (!this.errorLog) this.errorLog = e + " at " + this.cursor;
}

OL.Source.getValue = function() {
	return this.getLiteral() || this.getPath() || this.getList() || this.getNegative() || this.getQuote();
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
	case "#":
		return;
	default:
		var literal = this.token;
		this.token = this.getToken();
		if (literal[0] == "$") {
			return {type:"literal", value:literal.substring(1) - 0, debug:this.debug.literal, lookup:this.lookup.literal};
		} else {
			return {type:"literal", value:literal, debug:this.debug.literal, lookup:this.lookup.literal};
		}
	}
}

OL.Source.getPath = function() {
	var root = this.token;
	if (!(this.match("^") || this.match("~") || this.match("@"))) return;
	var keys = [];
	while (this.match(".")) {
		var key = this.getKey();
		if (key) {
			keys.push(key);
		} else {
			this.error("can not match a key");
			return;
		}
	}
	return {type:"path", root:root, keys:keys, debug:this.debug.path, lookup:this.lookup.path}; 
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
	return this.getString() || this.getFragment() || this.getList();
}

OL.Source.getFragment = function() {
	if (!this.match("{")) return;
	var ret = this.getPath();
	if (ret) {
		if (this.match("}")) {
			return ret;
		} else {
			this.error("can not match }");
		}
	} else {
		this.error("in {} must a path");
	}
}

OL.Source.getList = function() {
    if (!this.match("(")) return;
	var head = this.getValue();
	if (head) {
		var tail = [];
		while (this.match(",")) {
			var value = this.getValue();
			if (value) {
				tail.push(value);
			} else {
				this.error("tail can not match a value");
				return;
			}
		}
		if (this.match(")")) {		
			return {type:"list", head:head, tail:tail, debug:this.debug.list, lookup:this.lookup.list};
		} else {
			this.error("can not match )");
		}
	} else {
		this.error("function must have a head");
	}
}

OL.Source.getNegative = function() {
	if (!this.match("!")) return;
	var value = this.getValue();
	if (value) {
		return {type:"negative", value:value, debug:this.debug.negative, lookup:this.lookup.negative};
	} else {
		this.error("can not match value for !");
	}
}

OL.Source.getQuote = function() {
	if (!this.match("#")) return;
	var value = this.getValue();
	if (value) {
		return {type:"quote", value:value, debug:this.debug.quote, lookup:this.lookup.quote};
	} else {
		this.error("can not match value for #")
	}	
}

OL.parse = function(source) {
	var s = Object.create(this.Source);
	s.source = source;
	s.cursor = 0;
	s.token = s.getToken();
	var ret = s.getValue();
	if (ret && !s.errorLog) {
		return ret;
	} else {
		console.log("parse error: " + source);
		console.log(s.errorLog);
	}
}

OL.fun[">"] = function(params, root, temp, now) {
	return params[0] > params[1];
}

OL.fun[">="] = function(params, root, temp, now) {
	return params[0] >= params[1];
}

OL.fun["<"] = function(params, root, temp, now) {
	return params[0] < params[1];
}

OL.fun["<="] = function(params, root, temp, now) {
	return params[0] <= params[1];
}

OL.fun["="] = OL.fun["=="] = function(params, root, temp, now) {
	return params[0] == params[1];
}

OL.fun["!="] = function(params, root, temp, now) {
	return params[0] != params[1];
}

OL.fun["+"] = function(params, root, temp, now) {
	var result = 0;
	for (var i in params) {
		result += params[i];
	}
	return result;
}

OL.fun["*"] = function(params, root, temp, now) {
	var result = 1;
	for (var i in params) {
		result *= params[i];
	}
	return result;
}

OL.fun["-"] = function(params, root, temp, now) {
	return params[0] - params[1];
}

OL.fun["/"] = function(params, root, temp, now) {
	return params[0] / params[1];
}

OL.fun.not = function(params, root, temp, now) {
	return !params[0];
}

OL.fun.and = function(params, root, temp, now) {
	for (var i in params) {
		if (!params[i]) {
			return false;
		}
	}
	return true;
}

OL.fun.or = function(params, root, temp, now) {
	for (var i in params) {
		if (params[i]) {
			return true;
		}
	}
	return false;
}

OL.fun.filter = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0]);
	var fun = OL.autoLookup(root, temp, now, params[1]);
	var result = {};
	for (var i in list) {
		if (fun.lookup(root, temp, list[i])) {
			result[i] = list[i];
		}
	}
	return result;
}

OL.compare = function(value, left, right, root, temp) {
	if (value.type == "negative") {
		var a = value.value.lookup(root, temp, left);
		var b = value.value.lookup(root, temp, right);
		if (a < b) {
			return 1;
		} else if (a > b) {
			return -1;
		} else {
			return 0;
		}
	} else {
		var a = value.lookup(root, temp, left);
		var b = value.lookup(root, temp, right);
		if (a > b) {
			return 1;
		} else if (a < b) {
			return -1;
		} else {
			return 0;
		}		
	}
}

OL.fun.sort = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0]);
	var fun = OL.autoLookup(root, temp, now, params[1]);
	var result;
	if (list instanceof Array) {
		result = list;
	} else {
		result = [];
		for (var i in list) {
			result.push(list[i]);
		}
	}
	result.sort(function(a, b){
		var ret = OL.compare(fun.head, a, b, root, temp);
		for (var i in fun.tail) {
			if (ret != 0) return ret;
			else ret = OL.compare(fun.tail[i], a, b, root, temp);
		}
		return ret;
	});
	return result;
}

OL.fun.some = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0]);
	var fun = OL.autoLookup(root, temp, now, params[1]);
	for (var i in list) {
		if (fun.lookup(root, temp, list[i])) {
			return true;
		}
	}
	return false;
}


