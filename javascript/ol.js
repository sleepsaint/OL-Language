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
	return this.name.debug() + "(" + this.params.map(function(a){ return a.debug(); }).join(", ") + ")";
}

OL.Source.debug.negative = function() {
	return "!{" + this.value.debug() + "}";
}

OL.Source.lookup.literal = function() {
	return this.value;
}

OL.autoLookup = function(root, temp, now, current) {
	while (current && !(current instanceof Object)) {
		var a = OL.parse(current);
		if (a) {
			current = a.lookup(root, temp, now);
		} else {
			return;
		}
	}
	return current;	
}

OL.autoParse = function(root, temp, now, current) {
	while (current && current.type != "list") {
		current = OL.parse(current.lookup(root, temp, now));
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
	var fun = OL.fun[this.name.lookup(root, temp, now)];
	if (fun) {
		return fun(this.params, root, temp, now);
	}
}

OL.Source.lookup.negative = function(root, temp, now) {
	return !this.value.lookup(root, temp, now);
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
	return this.getLiteral() || this.getPath() || this.getList() || this.getNegative();
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
			return {type:"literal", value:literal.substring(1) - 0, debug:this.debug.literal, lookup:this.lookup.literal};
		} else {
			return {type:"literal", value:literal, debug:this.debug.literal, lookup:this.lookup.literal};
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
	var name = this.getValue();
	if (name) {
		var params = [];
		while (this.match(",")) {
			var param = this.getValue();
			if (param) {
				params.push(param);
			} else {
				this.error("can not match a param");
				return;
			}
		}
		if (this.match(")")) {		
			return {type:"list", name:name, params:params, debug:this.debug.list, lookup:this.lookup.list};
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
		return {type:"negative", value:value, debug:this.debug.negative, lookup:this.lookup.negative};
	} else {
		this.error("can not match value for !")
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
	return params[0].lookup(root, temp, now) > params[1].lookup(root, temp, now);
}

OL.fun[">="] = function(params, root, temp, now) {
	return params[0].lookup(root, temp, now) >= params[1].lookup(root, temp, now);
}

OL.fun["<"] = function(params, root, temp, now) {
	return params[0].lookup(root, temp, now) < params[1].lookup(root, temp, now);
}

OL.fun["<="] = function(params, root, temp, now) {
	return params[0].lookup(root, temp, now) <= params[1].lookup(root, temp, now);
}

OL.fun["="] = OL.fun["=="] = function(params, root, temp, now) {
	return params[0].lookup(root, temp, now) == params[1].lookup(root, temp, now);
}

OL.fun["+"] = function(params, root, temp, now) {
	var result = 0;
	for (var i in params) {
		result += params[i].lookup(root, temp, now);
	}
	return result;
}

OL.fun["*"] = function(params, root, temp, now) {
	var result = 1;
	for (var i in params) {
		result *= params[i].lookup(root, temp, now);
	}
	return result;
}

OL.fun["-"] = function(params, root, temp, now) {
	return params[0].lookup(root, temp, now) - params[1].lookup(root, temp, now);
}

OL.fun["/"] = function(params, root, temp, now) {
	return params[0].lookup(root, temp, now) / params[1].lookup(root, temp, now);
}

OL.fun.not = function(params, root, temp, now) {
	return !params[0].lookup(root, temp, now);
}

OL.fun.and = function(params, root, temp, now) {
	for (var i in params) {
		if (!params[i].lookup(root, temp, now)) {
			return false;
		}
	}
	return true;
}

OL.fun.or = function(params, root, temp, now) {
	for (var i in params) {
		if (params[i].lookup(root, temp, now)) {
			return true;
		}
	}
	return false;
}

OL.fun["filter"] = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0].lookup(root, temp, now));
	var fun = OL.autoParse(root, temp, now, params[1]);
	var result = {};
	for (var i in list) {
		if (fun.lookup(root, temp, list[i])) {
			result[i] = list[i];
		}
	}
	return result;
}

OL.fun["sort"] = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0].lookup(root, temp, now));
	var fun = OL.autoParse(root, temp, now, params[1]);
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
		var c = fun.name.lookup(root, temp, a);
		var d = fun.name.lookup(root, temp, b);
		if (c > d) {
			return 1;
		} else if (c  < d) {
			return -1;
		} else {
			for (var i in fun.params) {
				var e = fun.params[i];
				c = e.lookup(root, temp, a);
				d = e.lookup(root, temp, b);
				if (c > d) {
					return 1;
				} else if (c < d) {
					return -1;
				}
			}
		}
		return 0;
	});
	return result;
}


