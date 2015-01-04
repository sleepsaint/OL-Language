//
//  olbuffer.h
//  ol-cpp
//
//  Created by 伍 威 on 15/1/4.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__olbuffer__
#define __ol_cpp__olbuffer__

namespace OL {
    
    class Buffer {
        enum { STATIC_MAX_LENGTH = 8192 };
        char _static[STATIC_MAX_LENGTH];
        char* _begin;
        char* _cursor;
        char* _end;
    public:
        
        Buffer() {
            _begin = _static;
            _cursor = _static;
            _end = _static + STATIC_MAX_LENGTH;
        }
        
        void append(char c) {
            if (_cursor < _end) {
                *_cursor = c;
                ++_cursor;
            }
        }
        
        void reset() {
            _begin = _static;
            _cursor = _static;
            _end = _static + STATIC_MAX_LENGTH;
        }
        
        const char* begin() {
            return _begin;
        }
        
        const char* end() {
            return _cursor;
        }
    };
}




#endif /* defined(__ol_cpp__olbuffer__) */
