var OL = {};
OL.Source = {};
OL.Source.debug = {};
OL.Source.lookup = {};
OL.Source.change = {};
OL.Source.remove = {};
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
	return "`" + this.value.debug();
}

OL.Source.lookup.literal = function() {
	return this.value;
}
OL.Source.change.literal = function() {
	
}
OL.Source.remove.literal = function() {
	
}
OL.autoLookup = function(root, temp, now, current) {
	while (current && current.substring) {
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
OL.Source.change.path = function(root, temp, now, toValue) {
	var current;
	var previous = null;
	var value = null;
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
		value = this.keys[i].lookup(root, temp, now);
		current = OL.autoLookup(root, temp, now, current);
		if (current instanceof Array) {
			previous = current;
			current = current[parseInt(value)];
		} else if (current instanceof Object) {
			previous = current;
			current = current[value];
		} else {
			return;
		}
	}
	if (previous) {
		if (previous instanceof Array) {
			previous[parseInt(value)] = toValue;
		} else if (previous instanceof Object) {
			previous[value] = toValue;
		} else {
			return;
		}
	}	
}

OL.Source.remove.path = function(root, temp, now) {
	var current;
	var previous = null;
	var value = null;
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
		value = this.keys[i].lookup(root, temp, now);
		current = OL.autoLookup(root, temp, now, current);
		if (current instanceof Array) {
			previous = current;
			current = current[parseInt(value)];
		} else if (current instanceof Object) {
			previous = current;
			current = current[value];
		} else {
			return;
		}
	}
	if (previous) {
		if (previous instanceof Array) {
			previous.splice(value, 1);
		} else if (previous instanceof Object) {
			delete previous[value];
		} else {
			return;
		}
	}		
}

OL.Source.lookup.list = function(root, temp, now) {
	var funName = OL.fun[this.head.lookup(root, temp, now)];
	if (funName) {
		return funName(this.tail.map(function(a){ return a.lookup(root, temp, now); }), root, temp, now);
	}
}
OL.Source.change.list = function() {
	
}
OL.Source.remove.list = function() {
	
}
OL.Source.lookup.negative = function(root, temp, now) {
	return !this.value.lookup(root, temp, now);
}
OL.Source.change.negative = function() {
	
}
OL.Source.remove.negative = function() {
	
}
OL.Source.lookup.quote = function(root, temp, now) {
	return this.value;
}
OL.Source.change.quote = function() {
	
}
OL.Source.remove.quote = function() {
	
}
OL.Source.nextToken = function() {
	if (this.cursor < this.end) {
		var c = this.source[this.cursor];
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
		case "`":
		case ".":
			this.token = c;
			++this.cursor;
			break;
		case " ":
		case "\n":
			++this.cursor;
			this.nextToken();
			break;
		case "$":
			this.token = "n";
			++this.cursor;
			var start = this.cursor;
			PP:while (this.cursor < this.end) {
                switch (this.source[this.cursor]) {
                    case '^':
                    case '@':
                    case '~':
                    case '!':
                    case '{':
                    case '}':
                    case '(':
                    case ')':
                    case ',':
                    case '`':
                    case '\n':
                    case ' ':
                    case '$':
                        break PP;
                    default:
                        ++this.cursor;
                }
			}
			this.tokenNumber = this.source.substring(start, this.cursor) - 0;	
			break;
		default:
			this.token = "s";
			this.unescape();
		}
	} else {
		this.token = 0
	}
	
}

OL.Source.unescape = function() {
	this.tokenString = "";
	while (this.cursor < this.end) {
        var c = this.source[this.cursor];
        switch (c) {
            case '^':
            case '@':
            case '~':
            case '!':
            case '{':
            case '}':
            case '(':
            case ')':
            case ',':
            case '`':
            case '.':
            case '$':
                return;
            case '\\':
                ++this.cursor;
                var d = this.source[this.cursor];
                switch (d) {
                    case '\\':
                        this.tokenString += '\\';
                        break;
                    case 'n':
                        this.tokenString += '\n';
                        break;
                    case 'r':
                        this.tokenString += '\r';
                        break;
                    case 't':
                        this.tokenString += '\t';
                        break;
                    default:
                        this.tokenString += d;
                }
                ++this.cursor;
                break;
            default:
                this.tokenString += c;
                ++this.cursor;
                break;
        }
    }

}

OL.Source.match = function(expected) {
	if (expected == this.token) {
		this.nextToken();
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
	var token = this.token;
	this.nextToken();
	switch(token) {
	case "s":
		return {type:"string", value:this.tokenString, debug:this.debug.literal, lookup:this.lookup.literal, change:this.change.literal, remove:this.remove.literal };
	case "n":
		return {type:"number", value:this.tokenNumber, debug:this.debug.literal, lookup:this.lookup.literal, change:this.change.literal, remove:this.remove.literal };
	case "^":
	case "@":
	case "~":
		return this.getPath(token);
	case "{":
		return this.getFragment();
	case "!":
		var value = this.getValue();
		if (value) {
			return {type:"negative", value:value, debug:this.debug.negative, lookup:this.lookup.negative, change:this.change.negative, remove:this.remove.negative};
		} else {
			this.error("can not match value for !");
			return;
		}
	case "`":
		var value = this.getValue();
		if (value) {
			return {type:"quote", value:value, debug:this.debug.quote, lookup:this.lookup.quote, change:this.change.quote, remove:this.remove.negative};
		} else {
			this.error("can not match value for `");
			return;
		}
	case "(":
		return this.getList();
	default:
		;
	}
}

OL.Source.getPath = function(root) {
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
	return {type:"path", root:root, keys:keys, debug:this.debug.path, lookup:this.lookup.path, change:this.change.path, remove:this.remove.path};
}

OL.Source.getKey = function() {
	var token = this.token;
	this.nextToken();
	switch(token) {
	case "s":
		return {type:"string", value:this.tokenString, debug:this.debug.literal, lookup:this.lookup.literal, change:this.change.literal, remove:this.remove.literal };
	case "{":
		return this.getFragment();
	case "(":
		return this.getList();
	default:
		;
	}
}

OL.Source.getFragment = function() {
	var token = this.token;
	this.nextToken();
	switch(token) {
	case "@":
	case "^":
	case "~":
		var ret = this.getPath(token);
		if (ret) {
			if (this.match("}")) {
				return ret;
			} else {
				this.error("can not match }");
			}
		} else {
			this.error("in {} must a path");
		}
	default:
		;
	}
	
}

OL.Source.getList = function() {
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
			return {type:"list", head:head, tail:tail, debug:this.debug.list, lookup:this.lookup.list, change:this.change.list, remove:this.remove.list};
		} else {
			this.error("can not match )");
		}
	} else {
		this.error("function must have a head");
	}
}

OL.parse = function(source) {
	var s = Object.create(this.Source);
	s.source = source;
	s.cursor = 0;
	s.end = source.length;
	s.token = 0;
	s.nextToken();
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
OL.fun["if"] = function(params, root, temp, now) {
	return params[0] ? params[1] : params[2];
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
	return params.length > 0;
}

OL.fun.or = function(params, root, temp, now) {
	for (var i in params) {
		if (params[i]) {
			return true;
		}
	}
	return false;
}

OL.fun.random = function(params, root, temp, now) {
	switch (params.length) {
	case 0:
		return Math.random();
	case 1:
		return Math.floor(Math.random() * params[0]);
	default:
		return params[0] + Math.floor(Math.random() * (params[1] - params[0]))
	}
}

OL.filter = function(list, fun, root, temp) {
	var result = {};
	for (var i in list) {
		if (fun.lookup(root, temp, list[i])) {
			result[i] = list[i];
		}
	}
	return result;	
}

OL.fun.filter = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0]);
	var fun = OL.autoLookup(root, temp, now, params[1]);
	return OL.filter(list, fun, root, temp);
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

OL.sort = function(list, fun, root, temp) {
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
		if (fun.type == "list") {
			var ret = OL.compare(fun.head, a, b, root, temp);
			for (var i in fun.tail) {
				if (ret != 0) return ret;
				else ret = OL.compare(fun.tail[i], a, b, root, temp);
			}
			return ret;
		} else if (fun.type == "path") {
			return OL.compare(fun, a, b, root, temp);
		} else if (fun.type == "negative") {
			return -OL.compare(fun.value, a, b, root, temp);
		} else {
			return 0;
		}

	});
	return result;	
}

OL.fun.sort = function(params, root, temp, now) {
	var list = OL.autoLookup(root, temp, now, params[0]);
	var fun = OL.autoLookup(root, temp, now, params[1]);
	return OL.sort(list, fun, root, temp);
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


