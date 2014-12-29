//
//  olvalue.swift
//  ol-swift
//
//  Created by 伍 威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

extension OL {
    
    class String2: OLValue, Printable {
        let value: String
        init(value: String) {
            self.value = value
        }
        var description : String {
            return value
        }
        func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup? {
            return value
        }
    }
    
    class Number: OLValue, Printable {
        let value: Double
        init(value: Double) {
            self.value = value
        }
        var description : String {
            return "\(value)"
        }
        func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup? {
            return value
        }
    }
    
    class Path: OLValue, Printable {
        let root: String
        var keys = Array<OLValue>()
        init(root: String) {
            self.root = root
        }
        func addKey(key: OLValue) {
            keys.append(key)
        }
        var description : String {
            return "\(root)\(keys)"
        }
        func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup? {
            var current : OLLookup!
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
                if let value = key.lookup(root, temp: temp, now: now) {
                    current = current[value]
                } else {
                    return nil
                }
            }
            return current
        }
    }
    
    class List: OLValue, Printable {
        let head: OLValue
        var tail = Array<OLValue>()
        init(head: OLValue) {
            self.head = head
        }
        func addItem(item: OLValue) {
            tail.append(item)
        }
        var description : String {
            return "\(head)(\(tail))"
        }
        func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup? {
            var current : OLLookup!
            return current
            
        }
        
    }
    
    class Negative: OLValue, Printable {
        let value: OLValue
        init(value: OLValue) {
            self.value = value
        }
        var description : String {
            return "!\(value)"
        }
        func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup? {
            var current : OLLookup!
            return current
            
        }
        
    }
    
    class Quote: OLValue, Printable {
        let value: OLValue
        init(value: OLValue) {
            self.value = value
        }
        var description : String {
            return "#\(value)"
        }
        func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup? {
            var current : OLLookup!
            return current
            
        }
    }
    
    class func autoLookup(root: OLLookup, temp: OLLookup, now: OLLookup, current: OLLookup?) -> OLLookup? {
        return current
    }
    
    class func autoParse(root: OLLookup, temp: OLLookup, now: OLLookup, current: OLLookup?) -> OLLookup? {
        return current
    }
}