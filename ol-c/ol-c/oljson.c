//
//  oljson.c
//  ol-c
//
//  Created by 伍 威 on 15/1/9.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "oljson.h"

const int STATIC_BUFFER_SIZE = 8192;

typedef struct {
    char static_buffer[STATIC_BUFFER_SIZE];
    char* begin;
    char* end;
    char* cursor;
} OLBuffer;

static inline void init_buffer(OLBuffer* buffer) {
    buffer->cursor = buffer->begin = buffer->static_buffer;
    buffer->end = buffer->static_buffer + STATIC_BUFFER_SIZE - 1;
}

static inline void append_buffer(OLBuffer* buffer, char c) {
    if (buffer->cursor < buffer ->end) {
        *buffer->cursor = c;
        ++buffer->cursor;
    }
}

static inline void reset_buffer(OLBuffer* buffer) {
    init_buffer(buffer);
}

typedef struct {
    const char* source;
    const char* end;
    const char* cursor;
    int token;
    double tokenNumber;
    bool tokenBool;
    OLBuffer tokenString;
    
} OLJSON;

enum {
    STRING_TOKEN = 100000,
    NUMBER_TOKEN,
    BOOL_TOKEN,
    NULL_TOKEN
};

static inline int char_to_num(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return 0;
    }
}

static void unescape(OLJSON* json) {
    OLBuffer* buffer = &json->tokenString;
    reset_buffer(buffer);
    while (json->cursor < json->end) {
        char c = *json->cursor++;
        if (c == '\\') {
            char d = *json->cursor++;
            switch (d) {
                case '"':
                    append_buffer(buffer, '"');
                    break;
                case '\\':
                    append_buffer(buffer, '\\');
                    break;
                case '/':
                    append_buffer(buffer, '/');
                    break;
                case 'b':
                    append_buffer(buffer, '\b');
                    break;
                case 'f':
                    append_buffer(buffer, '\f');
                    break;
                case 'n':
                    append_buffer(buffer, '\n');
                    break;
                case 'r':
                    append_buffer(buffer, '\r');
                    break;
                case 't':
                    append_buffer(buffer, '\t');
                    break;
                case 'u':
                {
                    int hex = (char_to_num(json->cursor[0]) << 12) | (char_to_num(json->cursor[1]) << 8) | (char_to_num(json->cursor[2]) << 4) | char_to_num(json->cursor[3]);
                    json->cursor += 4;
                    if (hex < 0x80) {
                        append_buffer(buffer, hex);
                    } else if (hex < 0x800) {
                        append_buffer(buffer, 0xc0 | (hex >> 6));
                        append_buffer(buffer, 0x80 | (hex & 0x3f));
                    } else if (hex < 0x10000) {
                        append_buffer(buffer, 0xe0 | (hex >> 12));
                        append_buffer(buffer, 0x80 | ((hex >> 6) & 0x3f));
                        append_buffer(buffer, 0x80 | (hex & 0x3f));
                    }
                    
                }
                    
                    break;
                default:
                    append_buffer(buffer, d);
            }
        } else {
            if (c == '"') {
                return;
            } else {
                append_buffer(buffer, c);
            }
        }
    }
}

static void next_token(OLJSON* json) {
    const char* start;
    if (json->cursor < json->end) {
        char c = *json->cursor;
        switch (c) {
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
            case ':':
                json->token = c;
                ++json->cursor;
                break;
            case '"':
                json->token = STRING_TOKEN;
                ++json->cursor;
                unescape(json);
                break;
            case 'n':
                json->token = NULL_TOKEN;
                json->cursor += 4;
                break;
            case 't':
                json->token = BOOL_TOKEN;
                json->cursor += 4;
                json->tokenBool = true;
                break;
            case 'f':
                json->token = BOOL_TOKEN;
                json->cursor += 5;
                json->tokenBool = false;
                break;
            case '\n':
            case ' ':
                ++json->cursor;
                next_token(json);
                break;
            default:
                json->token = NUMBER_TOKEN;
                start = json->cursor;
                while (json->cursor < json->end) {
                    switch (*json->cursor) {
                        case '{':
                        case '}':
                        case '[':
                        case ']':
                        case ',':
                            goto PP;
                        default:
                            ++json->cursor;
                    }
                }
            PP: json->tokenNumber = atof(start);
        }
    } else {
        json->token = 0;
    }
    
}

static inline void init_json(OLJSON* json, const char* source, size_t length) {
    json->cursor = json->source = source;
    json->end = source + length;
    json->token = 0;
    init_buffer(&json->tokenString);
    next_token(json);
}

static inline bool json_match(OLJSON* json, int expected) {
    if (json->token == expected) {
        next_token(json);
        return true;
    } else {
        return false;
    }
}

static inline bool json_pair(OLJSON* json, OLArray* object);
static inline bool json_element(OLJSON* json, OLArray* array);

static OLValue json_value(OLJSON* json) {
    OLValue ret = {OL_NULL};
    size_t length;
    switch (json->token) {
        case STRING_TOKEN:
            ret.type = OL_STRING;
            length = json->tokenString.cursor - json->tokenString.begin;
            ret.stringValue = malloc(length + 1);
            strncpy(ret.stringValue, json->tokenString.begin, length);
            next_token(json);
            break;
        case NUMBER_TOKEN:
            ret.type = OL_NUMBER;
            ret.numberValue = json->tokenNumber;
            next_token(json);
            break;
        case BOOL_TOKEN:
            ret.type = OL_BOOL;
            ret.boolValue = json->tokenBool;
            next_token(json);
            break;
        case NULL_TOKEN:
            ret.type = OL_NULL;
            next_token(json);
            break;
        case '{':
            next_token(json);
            ret.type = OL_OBJECT;
            ret.arrayValue = OLArrayCreate();
            if (json_pair(json, ret.arrayValue)) {
                while (json_match(json, ',')) {
                    if (!json_pair(json, ret.arrayValue)) {
                        OLValueCleanup(&ret);
                        return OLNullValue;
                    }
                }
            }
            if (!json_match(json, '}')) {
                OLValueCleanup(&ret);
            }
            break;
        case '[':
            next_token(json);
            ret.type = OL_ARRAY;
            ret.arrayValue = OLArrayCreate();
            if (json_element(json, ret.arrayValue)) {
                while (json_match(json, ',')) {
                    if (!json_element(json, ret.arrayValue)) {
                        OLValueCleanup(&ret);
                        return OLNullValue;
                    }
                }
            }
            if (!json_match(json, ']')) {
                OLValueCleanup(&ret);
            }
        default:
            break;
    }
    return ret;
}

static inline bool json_pair(OLJSON* json, OLArray* object) {
    OLPair* pair = OLPairCreate();
    pair->key = json_value(json);
    if (json_match(json, ':')) {
        pair->value = json_value(json);
        OLValue* a = OLArrayAppend(object);
        a->type = OL_PAIR;
        a->pairValue = pair;
        return true;
    }
    OLPairCleanup(pair);
    return false;
}
static inline bool json_element(OLJSON* json, OLArray* array) {
    *OLArrayAppend(array) = json_value(json);
    return true;
}
OLValue OLParseJSON(const char* source, size_t length) {
    OLJSON json;
    init_json(&json, source, length);
    return json_value(&json);
}




