//
//  Value.swift
//  ol-swift
//
//  Created by 伍 威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

extension OL {
    
    class Value {
        func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            return nil
        }
    }
    
    class String2: Value, Printable {
        let value: NSString
        init(value: NSString) {
            self.value = value
        }
        var description : String {
            return value
        }
        override func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            return value
        }
    }
    
    class Number: Value, Printable {
        let value: NSNumber
        init(value: NSNumber) {
            self.value = value
        }
        var description : String {
            return "\(value)"
        }
        override func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            return value
        }
    }
    
    class Path: Value, Printable {
        let root: String
        var keys = Array<Value>()
        init(root: String) {
            self.root = root
        }
        func addKey(key: Value) {
            keys.append(key)
        }
        var description : String {
            return "\(root)\(keys)"
        }
        override func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            var current : AnyObject!
            switch self.root {
            case "^":
                current = root
            case "~":
                current = temp
            case "@":
                current = now
            default:
                return nil
            }
            for key in keys {
                if let value = key.lookup(root, temp: temp, now: now) as? NSString {
                    current = OL.autoLookup(root, temp: temp, now: now, current: current)
                    switch current {
                    case let array as NSArray:
                        current = array[Int(value.intValue)]
                    case let dict as NSDictionary:
                        current = dict[value]
                    default:
                        return nil
                    }
                } else {
                    return nil
                }
            }
            return current
        }
    }
    
    class List: Value, Printable {
        let head: Value
        var tail = Array<Value>()
        init(head: Value) {
            self.head = head
        }
        func addItem(item: Value) {
            tail.append(item)
        }
        var description : String {
            return "\(head)(\(tail))"
        }
        override func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            if let name = head.lookup(root, temp: temp, now: now) as? NSString {
                if let function = OL.Function[name] {
                    return function(tail.map({$0.lookup(root, temp: temp, now: now)}), root, temp, now)
                }
            }
            
            return nil
        }
        
    }
    
    class Negative: Value, Printable {
        let value: Value
        init(value: Value) {
            self.value = value
        }
        var description : String {
            return "!\(value)"
        }
        override func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            return !OL.toBool(value.lookup(root, temp: temp, now: now))
        }
    }
    
    class Quote: Value, Printable {
        let value: Value
        init(value: Value) {
            self.value = value
        }
        var description : String {
            return "#\(value)"
        }
        override func lookup(root: AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? {
            return value
        }
    }
    
    static func autoLookup(root: AnyObject, temp: AnyObject, now: AnyObject, current: AnyObject?) -> AnyObject? {
        var ret : AnyObject? = current
        while let source = ret as? NSString {
            if let value = OL.parse(source) {
                ret = value.lookup(root, temp: temp, now: now)
            }
        }
        return ret
    }
}