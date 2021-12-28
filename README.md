#OL Language

OL is "One Line". So it is very simple and easy to use.

OL is designed to get value from a tree. It regards a tree as an object and use a serial of "." to get value like javascript. And it use a same way as LISP to describe function just like `(function-name param...)`. So, OL can also be called "Object LISP".

##Lexical

+ _delimiter_ = @|!|(|)|{|}|,|.
+ _string_ = [^`delimiter`]+

___NOTE___:

+ float number must be marked like `5\.3`
+ SPACE before a string is ignored (if you need it, use `\ `), but in a string is not.

##Syntax

+ _value_ → ___string___|_path_|_list_|_negative_
+ _key_ → ___string___|_list_|_fragment_
+ _root_ → __^__|__~__|__@__|_list_|_fragment_
+ _path_ → _root_[._key_]*
+ _fragment_ → __{__*path*__}__
+ _list_ → __(__*value*[,*value*]*__)__
+ _negative_ → !_value_

