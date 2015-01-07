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
		case "#":
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
                    case '#':
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
            case '#':
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
	return this.getString() || this.getNumber() || this.getPath() || this.getList() || this.getNegative() || this.getQuote();
}

OL.Source.getNumber = function() {
	if (this.match("n")) {
		return {type:"number", value:this.tokenNumber, debug:this.debug.literal, lookup:this.lookup.literal };
	}
}

OL.Source.getString = function() {
	if (this.match("s")) {
		return {type:"string", value:this.tokenString, debug:this.debug.literal, lookup:this.lookup.literal };
	}
}

OL.Source.getPath = function() {
	var root = this.token;
	if (this.match("^") || this.match("~") || this.match("@")) {
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
}

OL.Source.getKey = function() {
	return this.getString() || this.getFragment() || this.getList();
}

OL.Source.getFragment = function() {
	if (this.match("{")) {
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
}

OL.Source.getList = function() {
    if (this.match("(")) {
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
}

OL.Source.getNegative = function() {
	if (this.match("!")) {
		var value = this.getValue();
		if (value) {
			return {type:"negative", value:value, debug:this.debug.negative, lookup:this.lookup.negative};
		} else {
			this.error("can not match value for !");
		}
	}
}

OL.Source.getQuote = function() {
	if (this.match("#")) {
		var value = this.getValue();
		if (value) {
			return {type:"quote", value:value, debug:this.debug.quote, lookup:this.lookup.quote};
		} else {
			this.error("can not match value for #")
		}
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


