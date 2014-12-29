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
        var token: String?
        var errorLog = ""
        init(source: String) {
            self.source = source
            self.cursor = source.startIndex
            self.token = getToken()
        }
        func getToken() -> String? {
            enum ScanStatus { case None, String, Number};
            var start = cursor;
            var status = ScanStatus.None;
            for i in cursor ..< source.endIndex {
                let c = source[i]
                switch c {
                case "~", "!", "@", "^", "(", ")", "{", "}", ",", "#":
                    if status == .None {
                        ++cursor
                        return String(c)
                    } else {
                        cursor = i
                        return source.substringWithRange(start ..< i)
                    }
                case ".":
                    if status == .None {
                        ++cursor
                        return "."
                    } else if status == .String {
                        cursor = i;
                        return source.substringWithRange(start ..< i)
                    }
                case " ":
                    ++cursor
                case "$":
                    if status == .None {
                        start = cursor
                        status = .Number
                    }
                default:
                    if status == .None {
                        start = cursor
                        status = .String
                    }
                }
            }
            cursor = source.endIndex
            return (start == source.endIndex || status == .None) ? nil : source.substringFromIndex(start)
        }
        func match(expected: String) -> Bool {
            if expected == token {
                token = getToken()
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
        func getLiteral() -> OLValue? {
            if let literal = token {
                switch literal {
                case "~", "!", "@", "^", "(", ")", "{", "}", ",", ".", "#":
                    return nil
                default:
                    token = getToken()
                    if literal[literal.startIndex] == "$" {
                        return Number(value: (literal.substringFromIndex(literal.startIndex) as NSString).doubleValue)
                    } else {
                        return String2(value: literal)
                    }
                }

            } else {
                return nil
            }
        }
        func getPath() -> OLValue? {
            if let root = token {
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
            }
            return nil
        }
        func getString() -> OLValue? {
            if let ret = getLiteral() {
                if ret is String2 {
                    return ret
                }
            }
            return nil
        }
        func getKey() -> OLValue? {
            return getString() ?? getFragment() ?? getList()
        }
        func getFragment() -> OLValue? {
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
        func getList() -> OLValue? {
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
        func getNegative() -> OLValue? {
            if match("!") {
                if let value = getValue() {
                    return Negative(value: value)
                } else {
                    error("can not match value for !")
                }
            }
            return nil
        }
        func getOuote() -> OLValue? {
            if match("#") {
                if let value = getValue() {
                    return Quote(value: value)
                } else {
                    error("can not match value for #")
                }
            }
            return nil
        }
        func getValue() -> OLValue? {
            return getLiteral() ?? getPath() ?? getList() ?? getNegative() ?? getOuote()
        }
    }
    
    class func parse(source: String) -> OLValue? {
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