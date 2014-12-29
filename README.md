#OL Language

OL is "One Line". So it is very simple and easy to use.

OL is designed to get value from a tree. It regards a tree as an object and use a serial of "." to get value like javascript. And it use a same way as LISP to describe function just like `(function-name param...)`. So, OL can also be called "Object LISP".

##Lexical

+ _delimiter_ = ~|!|@|^|(|)|{|}|,|.
+ _string_ = [^#_delimiter_]+
+ _number_ = #_string_(._string_)?

___NOTE___:

+ _number_ is only a string start by "#" and can contain "."
+ SPACE before a string is ignored, but in a string is not.

##Syntax

+ _value_ → _literal_|_path_|_list_|_negative_|_quote_
+ _literal_ → ___number___|___string___
+ _path_ → (__^__|__@__|__~__)[._key_]*
+ _key_ → ___string___|_list_|_fragment_
+ _fragment_ → __{__*path*__}__
+ _list_ → __(__*value*[,*value*]*__)__
+ _quote_ → `_list_
