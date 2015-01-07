//
//  OLSourceValue.swift
//  ol-swift
//
//  Created by 伍 威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

import Foundation

class OLSourceValue: OLValue {
    func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        return NSNull()
    }
    func toBool() -> Bool {
        return true
    }
    func getValueByKey(key: OLValue) -> OLValue {
        return NSNull()
    }
    func compare(b: OLValue) -> Int {
        return 0
    }
    func filter(function: OLList, root: OLValue, temp: OLValue) -> OLValue {
        return self
    }
    func some(function: OLList, root: OLValue, temp: OLValue) -> Bool {
        return true
    }
    func toArray() -> [AnyObject] {
        return []
    }
    func sort(inout array: [AnyObject], root: OLValue, temp: OLValue) {
        
    }
    func compare(a: OLValue, _ b: OLValue, root: OLValue, temp: OLValue) -> Int {
        return lookup(root, temp: temp, now: a).compare(lookup(root, temp: temp, now: b))
    }
}

class OLString: OLSourceValue, Printable {
    let value: NSString
    init(value: NSString) {
        self.value = value
    }
    var description : String {
        return value
    }
    override func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        return value
    }
}

class OLNumber: OLSourceValue, Printable {
    let value: NSNumber
    init(value: NSNumber) {
        self.value = value
    }
    var description : String {
        return "\(value)"
    }
    override func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        return value
    }
}

class OLPath: OLSourceValue, Printable {
    let root: Character
    var keys = Array<OLSourceValue>()
    init(root: Character) {
        self.root = root
    }
    func addKey(key: OLSourceValue) {
        keys.append(key)
    }
    var description : String {
        return "\(root)\(keys)"
    }
    override func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        var current : OLValue
        switch self.root {
        case "^":
            current = root
        case "~":
            current = temp
        case "@":
            current = now
        default:
            return NSNull()
        }
        for k in keys {
            let key = k.lookup(root, temp: temp, now: now)
            current = autoLookup(root, temp, now, current)
            current = current.getValueByKey(key)
        }
        return current
    }
    override func sort(inout array: [AnyObject], root: OLValue, temp: OLValue) {
        array.sort({
            return self.compare(toOLValue($0), toOLValue($1), root: root, temp: temp) < 0
        })
    }
}

class OLList: OLSourceValue, Printable {
    let head: OLSourceValue
    var tail = Array<OLSourceValue>()
    init(head: OLSourceValue) {
        self.head = head
    }
    func addItem(item: OLSourceValue) {
        tail.append(item)
    }
    var description : String {
        return "\(head)(\(tail))"
    }
    override func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        if let name = head.lookup(root, temp: temp, now: now) as? NSString {
            if let function = OLFunction[name] {
                return function(tail.map({$0.lookup(root, temp: temp, now: now)}), root, temp, now)
            }
        }
        
        return NSNull()
    }
    override func sort(inout array: [AnyObject], root: OLValue, temp: OLValue) {
        array.sort({
            var ret = self.head.compare(toOLValue($0), toOLValue($1), root: root, temp: temp)
            for i in self.tail {
                if ret != 0 { return ret < 0 }
                else { ret = i.compare(toOLValue($0), toOLValue($1), root: root, temp: temp) }
            }
            return ret < 0
        })
    }
}

class OLNegative: OLSourceValue, Printable {
    let value: OLSourceValue
    init(value: OLSourceValue) {
        self.value = value
    }
    var description : String {
        return "!\(value)"
    }
    override func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        return true
    }
    override func sort(inout array: [AnyObject], root: OLValue, temp: OLValue) {
        array.sort({
            return self.value.compare(toOLValue($0), toOLValue($1), root: root, temp: temp) > 0
        })
    }
    override func compare(a: OLValue, _ b: OLValue, root: OLValue, temp: OLValue) -> Int {
        return -value.lookup(root, temp: temp, now: a).compare(value.lookup(root, temp: temp, now: b))
    }
}

class OLQuote: OLSourceValue, Printable {
    let value: OLSourceValue
    init(value: OLSourceValue) {
        self.value = value
    }
    var description : String {
        return "#\(value)"
    }
    override func lookup(root: OLValue, temp: OLValue, now: OLValue) -> OLValue {
        return value
    }
}

func autoLookup(root: OLValue, temp: OLValue, now: OLValue, current: OLValue) -> OLValue {
    var ret = current
    while let source = ret as? NSString {
        if let value = OLSource.parse(source) {
            ret = value.lookup(root, temp: temp, now: now)
        }
    }
    return ret
}

