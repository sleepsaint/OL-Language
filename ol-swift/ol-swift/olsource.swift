//
//  ol.swift
//  ol-swift
//
//  Created by 伍 威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

extension OL {
    
    class Source {
        let source: String
        var cursor: String.Index
        var token: Character = "\0"
        var errorLog = ""
        var tokenString: String = ""
        var tokenNumber: Double = 0
        init(source: String) {
            self.source = source
            self.cursor = source.startIndex
            nextToken()
        }
        func nextToken() {
            var start = cursor;
            if cursor < source.endIndex {
                let c = source[cursor]
                switch c {
                case "~", "!", "@", "^", "(", ")", "{", "}", ",", "#", ".":
                    token = c;
                    ++cursor;
                case " ", "\n":
                    ++cursor
                    nextToken()
                case "$":
                    token = "n"
                    ++cursor;
                    start = cursor;
                    PP: while cursor < source.endIndex {
                        switch source[cursor] {
                        case "~", "!", "@", "^", "(", ")", "{", "}", ",", "#", " ", "\n", "$":
                            break PP;
                        default:
                            ++cursor;
                        }
                    }
                    tokenNumber = (source.substringWithRange(start ..< cursor) as NSString).doubleValue;
                default:
                    token = "s"
                    unescape()
                }
            } else {
                token = "\0"
            }
        }
        func unescape() {
            tokenString.removeAll(keepCapacity: true);
            while cursor < source.endIndex {
                let c = source[cursor]
                switch c {
                case "~", "!", "@", "^", "(", ")", "{", "}", ",", "#", ".", "$":
                    return
                case "\\":
                    ++cursor
                    let d = source[cursor]
                    switch d {
                    case "\\":
                        tokenString.append(Character("\\"))
                    case "n":
                        tokenString.append(Character("\n"))
                    case "r":
                        tokenString.append(Character("\r"))
                    case "t":
                        tokenString.append(Character("\t"))
                    default:
                        tokenString.append(d)
                    }
                    ++cursor
                default:
                    tokenString.append(c)
                    ++cursor;
                }
            }
        }
        func match(expected: Character) -> Bool {
            if expected == token {
                nextToken()
                return true
            } else {
                return false
            }
        }
        func error(e: String) {
            if errorLog.isEmpty {
                errorLog = "\(e) at \(cursor)"
            }
        }
        func getNumber() -> Value? {
            if match("n") {
                return Number(value: tokenNumber)
            }
            return nil
        }
        func getString() -> Value? {
            if match("s") {
                return String2(value: tokenString)
            }
            return nil
        }
        func getPath() -> Value? {
            let root = token
            if match("^") || match("~") || match("@") {
                let ret = Path(root: root)
                while match(".") {
                    if let key = getKey() {
                        ret.addKey(key)
                    } else {
                        error("can not match a key")
                        return nil
                    }
                }
                return ret
            }
            return nil
        }
        
        func getKey() -> Value? {
            return getString() ?? getFragment() ?? getList()
        }
        func getFragment() -> Value? {
            if match("{") {
                if let ret = getPath() {
                    if match("}") {
                        return ret
                    } else {
                        error("can not match }")
                    }
                } else {
                    error("in {} must a path")
                }
            }
            return nil
        }
        func getList() -> Value? {
            if match("(") {
                if let head = getValue() {
                    let ret = List(head: head)
                    while match(",") {
                        if let item = getValue() {
                            ret.addItem(item)
                        } else {
                            error("tail can not match a value")
                            return nil
                        }
                    }
                    if match(")") {
                        return ret
                    } else {
                        error("can not match )")
                    }
                }
            }
            return nil
        }
        func getNegative() -> Value? {
            if match("!") {
                if let value = getValue() {
                    return Negative(value: value)
                } else {
                    error("can not match value for !")
                }
            }
            return nil
        }
        func getOuote() -> Value? {
            if match("#") {
                if let value = getValue() {
                    return Quote(value: value)
                } else {
                    error("can not match value for #")
                }
            }
            return nil
        }
        func getValue() -> Value? {
            return getString() ?? getNumber() ?? getPath() ?? getList() ?? getNegative() ?? getOuote()
        }
    }
    
    static func parse(source: String) -> Value? {
        let s = Source(source: source)
        if let ret = s.getValue() {
            return ret
        } else {
            println("parse error: \(source)")
            println(s.errorLog)
            return nil
        }
    }
}