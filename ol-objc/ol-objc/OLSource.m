//
//  OLSource.m
//  ol-objc
//
//  Created by 伍威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#import "OLSource.h"
#import "OLSourceValue.h"
#import "OLBuffer.h"

@implementation OLSource

- (instancetype) initWithSource:(const char*)source length:(NSUInteger)length {
    if (self = [super init]) {
        _source = source;
        _end = source + length;
        _cursor = source;
        _token = 0;
        _tokenString = [[OLBuffer alloc] init];
        [self nextToken];
    }
    return self;
}

- (void) nextToken {
    if (_cursor < _end) {
        char c = *_cursor;
        const char* start;
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
                _token = c;
                ++_cursor;
                break;
            case '\n':
            case ' ':
                ++_cursor;
                [self nextToken];
                break;
            case '$':
                _token = NUMBER_TOKEN;
                ++_cursor;
                start = _cursor;
                while (_cursor < _end) {
                    switch (*_cursor) {
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
                            goto PP;
                        default:
                            ++_cursor;
                    }
                }
            PP: _tokenNumber = atof(start);
                break;
            default:
                _token = STRING_TOKEN;
                [self unescape];
        }
    } else {
        _token = 0;
    }
}

- (void) unescape {
    [_tokenString reset];
    while (_cursor < _end) {
        char c = *_cursor;
        char d;
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
                ++_cursor;
                d = *_cursor;
                switch (d) {
                    case '\\':
                        [_tokenString append:'\\'];
                        break;
                    case 'n':
                        [_tokenString append:'\n'];
                        break;
                    case 'r':
                        [_tokenString append:'\r'];
                        break;
                    case 't':
                        [_tokenString append:'\t'];
                        break;
                    default:
                        [_tokenString append:d];
                }
                ++_cursor;
                break;
            default:
                [_tokenString append:c];
                ++_cursor;
                break;
        }
    }
}

- (BOOL) match:(int)expected {
    if (_token == expected) {
        [self nextToken];
        return true;
    } else {
        return false;
    }
}


- (void) error:(NSString*)e {
    if (!_errorLog) {
        _errorLog = [NSString stringWithFormat:@"%@ at %d", e, _cursor - _source];
    }
}

- (OLNumber*) number {
    if ([self match:NUMBER_TOKEN]) {
        return [OLNumber value:_tokenNumber];
    }
    return nil;
}

- (OLString*) string {
    if ([self match:STRING_TOKEN]) {
        return [OLString value:_tokenString.string];
    }
    return nil;
}

- (OLPath*) path {
    int root = _token;
    if ([self match:'^'] || [self match:'~'] || [self match:'@']) {
        NSMutableArray* keys = [NSMutableArray arrayWithCapacity:8];
        while ([self match:'.']) {
            OLSourceValue* key = self.key;
            if (key) {
                [keys addObject:key];
            } else {
                [self error:@"can not match a key"];
                return nil;
            }
        }
        return [OLPath root:root keys:keys];
    }
    return nil;
}

- (OLSourceValue*) key {
    OLSourceValue* ret;
    if ((ret = self.string)) {
        return ret;
    } else if ((ret = self.fragment)) {
        return ret;
    } else if ((ret = self.list)) {
        return ret;
    } else {
        return nil;
    }
}

- (OLPath*) fragment {
    if ([self match:'{']) {
        OLPath* path = self.path;
        if (path) {
            if ([self match:'}']) {
                return path;
            } else {
                [self error:@"can not match }"];
            }
        } else {
            [self error:@"in {} must a path"];
        }
    }
    return nil;
}

- (OLList*) list {
    if ([self match:'(']) {
        OLSourceValue* head = self.value;
        if (head) {
            NSMutableArray* tail = [NSMutableArray arrayWithCapacity:8];
            while ([self match:',']) {
                OLSourceValue* item = self.value;
                if (item) {
                    [tail addObject:item];
                } else {
                    [self error:@"tail can not match a value"];
                    return nil;
                }
            }
            if ([self match:')']) {
                return [OLList head:head tail:tail];
            } else {
                [self error:@"can not match )"];
            }
        }
    }
    return nil;
}

- (OLNegative*) negative {
    if ([self match:'!']) {
        OLSourceValue* value = self.value;
        if (value) {
            return [OLNegative value:value];
        } else {
            [self error:@"can not match value for !"];
        }
    }
    return nil;
}

- (OLQuote*) quote {
    if ([self match:'`']) {
        OLSourceValue* value = self.value;
        if (value) {
            return [OLQuote value:value];
        } else {
            [self error:@"can not match value for `"];
        }
    }

    return nil;
}

- (OLSourceValue*) value {
    OLSourceValue* ret;
    if ((ret = self.string)) {
        return ret;
    } else if ((ret = self.number)) {
        return ret;
    } else if ((ret = self.path)) {
        return ret;
    } else if ((ret = self.list)) {
        return ret;
    } else if ((ret = self.negative)) {
        return ret;
    } else if ((ret = self.quote)) {
        return ret;
    } else {
        return nil;
    }

}

+ (OLSourceValue*) parse:(NSString*)source {
    return [self parse:source.UTF8String length:source.length];
}

+ (OLSourceValue*) parse:(const char*)source length:(NSUInteger)length {
    OLSource* s = [[OLSource alloc] initWithSource:source length:length];
    OLSourceValue* ret = s.value;
    if (!ret) {
        printf("%s\n", source);
        printf("%s\n", s->_errorLog.UTF8String);
    }
    return ret;
}

@end
