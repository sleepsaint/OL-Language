class Path {
	root = null
	keys = null
	constructor(root, keys) {
		this.root = root;
		this.keys = keys;
	}
	getValue(len) {
		let current;
		len = len !== undefined ? Math.min(len, this.keys.length) : this.keys.length;
		switch (this.root) {
			case "^":
				current = OL.root; break;
			case "~":
				current = OL.temp; break;
			case "@":
				current = OL.now; break;
			default:
				current = this.root.valueOf();
		}
		for (let i = 0; i < len; ++i) {
			let key = this.keys[i].valueOf();
			if (key !== null && current instanceof Object) current = current[key];
			else return null;
		}
		return current;
	}
	valueOf() {
		return this.getValue();
	}
	set(value) {
		let previous = this.getValue(this.keys.length - 1);
		if (previous instanceof Object) {
			let key = this.keys[this.keys.length - 1].valueOf();
			if (key !== null) previous[key] = value;
		}
		return value;
	}
	del() {
		let previous = this.getValue(this.keys.length - 1);
		if (previous) {
			delete previous[this.keys[this.keys.length - 1]];
		}
	}
}

class List {
	head = null
	tail = null
	constructor(head, tail) {
		this.head = head;
		this.tail = tail;
	}
	valueOf() {
		let func = OL.functionMap[this.head];
		if (func) return func(...this.tail);
		else {
			console.error("function is not exist: " + this.head);
			return "";
		}
	}
}

class Negative {
	value = null
	constructor(value) {
		this.value = value;
	}
	valueOf() {
		return !this.value;
	}
}

class Parser {
	cursor = 0
	end = 0
	source = ""
	token = ""
	tokenString = ""
	errorString = ""
	constructor(source = "") {
		this.setSource(source);
	}
	setSource(source) {
		this.source = source;
		this.cursor = 0;
		this.end = source.length;
		this.errorString = "";
		this.nextToken();
	}
	nextToken() {
		if (this.cursor < this.end) {
			let c = this.source[this.cursor];
			switch (c) {
				case "~": case "^": case "@":
				case ".": case ",": case "!":
				case "(": case ")":
				case "{": case "}":
					this.token = c; ++this.cursor; break;
				case " ": case "\n":
					++this.cursor; this.nextToken(); break;
				default:
					this.token = "s";
					this.unescape();
			}
		} else {
			this.token = "";
		}
	}
	unescape() {
		this.tokenString = "";
		while (this.cursor < this.end) {
			let c = this.source[this.cursor];
			switch (c) {
				case "~": case "^": case "@":
				case ".": case ",": case "!":
				case "(": case ")":
				case "{": case "}":
					return;
				case "\\":
					++this.cursor;
					let d = this.source[this.cursor];
					switch (d) {
						case "\\":
							this.tokenString += "\\"; break;
						case "n":
							this.tokenString += "\n"; break;
						case "r":
							this.tokenString += "\r"; break;
						case "t":
							this.tokenString += "\t"; break;
						case " ":
							this.tokenString += " "; break;
						default:
							this.tokenString += d;
					}
					++this.cursor; break;
				default:
					this.tokenString += c; ++this.cursor;
			}
		}
	}
	error(message) {
		if (!this.errorString) {
			this.errorString = "parse error at:[ " + this.source.substring(0, this.cursor) + " ]" + message;
			console.error(this.errorString);
		}
	}
	match(expected) {
		if (expected == this.token) {
			this.nextToken();
			return true;
		} else {
			return false;
		}
	}
	getString() {
		return this.match("s") ? this.tokenString : null;
	}
	getRoot() {
		let token = this.token;
		if (this.match("^") || this.match("~") || this.match("@")) return token;
		else return this.getList() ?? this.getFragment();
	}
	getPath() {
		let root = this.getRoot();
		if (root === null) return null;
		let keys = [];
		while (this.match(".")) {
			let key = this.getKey();
			if (key !== null) {
				keys.push(key);
			} else {
				this.error("can not match a key")
				return null;
			}
		}
		return (keys.length > 0 || !(root instanceof List)) ? new Path(root, keys) : root;
	}
	getFragment() {
		if (this.match("{")) {
			let path = this.getPath();
			if (path) {
				if (this.match("}")) {
					return path;
				} else {
					this.error("can not match }");
				}
			} else {
				this.error("in {} must a path");
				return null;
			}
		}
		return null;
	}
	getList() {
		if (this.match("(")) {
			let head = this.getValue();
			if (head) {
				let tail = [];
				while (this.match(",")) {
					let t = this.getValue();
					if (t !== null) {
						tail.push(t);
					} else {
						this.error("tail can not match a value");
						return null;
					}
				}
				if (this.match(")")) {
					return new List(head, tail);
				} else {
					this.error("can not match )");
				}
			} else {
				this.error("head can not match a value");
			}
		}
		return null;
	}
	getNegative() {
		if (this.match("!")) {
			let v = this.getValue();
			if (v !== null) {
				return new Negative(v);
			} else {
				this.error("can not match value for !");
			}
		}
		return null;
	}
	getKey() {
		return this.getString() ?? this.getFragment() ?? this.getList();
	}
	getValue() {
		return this.getString() ?? this.getPath() ?? this.getList() ?? this.getNegative();
	}

}

let OL = {
	parser: new Parser(),
	get(path) {
		return this.parse(path).valueOf();
	},
	set(path, value) {
		let a = this.parse(path);
		if (a.set) a.set(value);
		return value;
	},
	del(path) {
		let a = this.parse(path);
		if (a.del) a.del();
	},
	cache: {},
	parse(path) {
		let a = this.cache[path];
		if (!a) {
			this.parser.setSource(path);
			a = this.cache[path] = this.parser.getValue();
		}
		return a;
	},
	root: {},
	temp: {},
	now: null,
	compare(a, b, condition) {
		let reverse = false;
		if (condition instanceof Negative) {
			condition = condition.value;
			reverse = true;
		}
		OL.now = a;
		let v1 = condition.valueOf();
		OL.now = b;
		let v2 = condition.valueOf();
		return reverse ? (v2 - v1) : (v1 - v2);
	},
	functionMap: {
		"+": function (a, b) {
			return +a + (+b);
		},
		"-": function (a, b) {
			return a - b;
		},
		">": function (a, b) {
			return a > b;
		},
		"<": function (a, b) {
			return a < b;
		},
		">=": function (a, b) {
			return a >= b;
		},
		"<=": function (a, b) {
			return a <= b;
		},
		"=": function (a, b) {
			return a == b;
		},
		"==": function (a, b) {
			return a == b;
		},
		or(a, ...b) {
			if (a.valueOf()) return true;
			for (const c of b) {
				if (c.valueOf()) return true;
			}
			return false;
		},
		"if": function (conditon, a, b) {
			return conditon.valueOf() ? a : b;
		},
		get(path) {
			return OL.get(path.valueOf());
		},
		some(data, condition) {
			let d = Object.values(data.valueOf());
			return d.some(a => {
				OL.now = a;
				return condition.valueOf();
			});
		},
		random(a, b) {
			let r = Math.random();
			if (a === undefined) return r;
			else if (b === undefined) return r * a;
			else return +a + r * (b - a);
		},
		filter(data, condition) {
			let d = Object.values(data.valueOf());
			return d.filter(a => {
				OL.now = a;
				return condition.valueOf();
			});
		},
		sort(data, list) {
			if (typeof list === "string") {
				return OL.functionMap.sort(data, OL.parse(OL.get(list)));
			}
			let d = Object.values(data.valueOf());
			if (list instanceof Path || list instanceof Negative) {
				d.sort((a, b) => OL.compare(a, b, list));
			} else {
				d.sort((a, b) => {
					let r = OL.compare(a, b, list.head);
					if (r == 0) {
						for (const c of list.tail) {
							r = OL.compare(a, b, c);
							if (r) return r;
						}
					}
					return r;
				});
			}
			return d;
		},
	}
};
