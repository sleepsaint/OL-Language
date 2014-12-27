#OL Language

OL is "One Line". So it is very simple and easy to use.

OL is designed to get value from a tree. It regards a tree as an object and use a serial of "." to get value like javascript. And it use a same way as LISP to describe function just like `(function-name param...)`. So, OL can also be called "Object LISP".

##Lexical

+ **delimiter** = ~|!|@|^|(|)|{|}|,|.
+ **number** = #[-][0~9]*
+ **string** = [!**delimiter**]*

##Syntax

+ *literal* -> **number**|**string**
+ *value* -> *literal*|*path*|*function*|*negative*
+ *path* -> (^|@|~){.*key*}
+ *key* -> **string**|*function*|*fragment*
+ *fragment* -> "{"*path*"}"
+ *function* -> "("*value*{,*value*}")"

##Example

###Data
	{
		person: {
			"0001": {"name": "Peter", "age": 30, "spouse": "0002"},
			"0002": {"name": "Jane", "age": 28, "spouse": "0001"},
			"0003": {"name": "Tom", "age": 28}
		}
	}

