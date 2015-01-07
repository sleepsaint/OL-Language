//
//  OLValue.swift
//  ol-swift
//
//  Created by 伍 威 on 15/1/7.
//  Copyright (c) 2015年 sleepsaint. All rights reserved.
//

import Foundation

protocol OLValue {
    func toBool() -> Bool
    func getValueByKey(key: OLValue) -> OLValue
    func compare(b: OLValue) -> Int
    func filter(function: OLList, root: OLValue, temp: OLValue) -> OLValue
    func some(function: OLList, root: OLValue, temp: OLValue) -> Bool
    func toArray() -> [AnyObject]
}

func OLCompare(a: Double, b: Double) -> Int {
    if a > b {
        return 1
    } else if a < b {
        return -1
    } else {
        return 0
    }
}

func OLCompare(a: Int, b: Int) -> Int {
    if a > b {
        return 1
    } else if a < b {
        return -1
    } else {
        return 0
    }
}

func OLCompare(a: String, b: String) -> Int {
    if a > b {
        return 1
    } else if a < b {
        return -1
    } else {
        return 0
    }
}

extension NSNumber: OLValue {
    func toBool() -> Bool {
        return doubleValue != 0
    }
    func getValueByKey(key: OLValue) -> OLValue {
        return NSNull()
    }
    func compare(b: OLValue) -> Int {
        switch b {
        case let n as NSNumber:
            return OLCompare(doubleValue, n.doubleValue)
        case let s as NSString:
            return OLCompare(doubleValue, s.doubleValue)
        default:
            return 0
        }
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
}

extension NSString: OLValue {
    func toBool() -> Bool {
        return length > 0
    }
    func getValueByKey(key: OLValue) -> OLValue {
        return NSNull()
    }
    func compare(b: OLValue) -> Int {
        switch b {
        case let n as NSNumber:
            return OLCompare(doubleValue, n.doubleValue)
        case let s as NSString:
            return OLCompare(self, s)
        default:
            return 0
        }
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
}

extension NSArray: OLValue {
    func toBool() -> Bool {
        return count > 0
    }
    func getValueByKey(key: OLValue) -> OLValue {
        switch key {
        case let a as NSString:
            return toOLValue(objectAtIndex(a.integerValue))
        case let a as NSNumber:
            return toOLValue(objectAtIndex(a.integerValue))
        default:
            return NSNull()
        }
    }
    func compare(b: OLValue) -> Int {
        switch b {
        case let n as NSArray:
            return OLCompare(count, n.count)
        default:
            return 0
        }
    }
    func filter(function: OLList, root: OLValue, temp: OLValue) -> OLValue {
        let ret = NSMutableArray();
        for i in self {
            if function.lookup(root, temp: temp, now: toOLValue(i)).toBool() {
                ret.addObject(i)
            }
        }
        return ret
    }
    func some(function: OLList, root: OLValue, temp: OLValue) -> Bool {
        for i in self {
            if function.lookup(root, temp: temp, now: toOLValue(i)).toBool() {
                return true
            }
        }
        return false
    }
    func toArray() -> [AnyObject] {
        return self
    }
}

extension NSDictionary: OLValue {
    func toBool() -> Bool {
        return count > 0
    }
    func getValueByKey(key: OLValue) -> OLValue {
        switch key {
        case let a as NSString:
            return toOLValue(objectForKey(a))
        default:
            return NSNull()
        }
    }
    func compare(b: OLValue) -> Int {
        switch b {
        case let n as NSDictionary:
            return OLCompare(count, n.count)
        default:
            return 0
        }
    }
    func filter(function: OLList, root: OLValue, temp: OLValue) -> OLValue {
        let ret = NSMutableDictionary();
        for (key, i) in self {
            if function.lookup(root, temp: temp, now: toOLValue(i)).toBool() {
                ret[key as NSString] = i
            }
        }
        return ret
    }
    func some(function: OLList, root: OLValue, temp: OLValue) -> Bool {
        for (key, i) in self {
            if function.lookup(root, temp: temp, now: toOLValue(i)).toBool() {
                return true
            }
        }
        return false
    }
    func toArray() -> [AnyObject] {
        return allValues
    }
}

extension NSNull: OLValue {
    func toBool() -> Bool {
        return false
    }
    func getValueByKey(key: OLValue) -> OLValue {
        return NSNull()
    }
    func compare(b: OLValue) -> Int {
        switch b {
        case let n as NSNull:
            return 0
        default:
            return -1
        }
    }
    func filter(function: OLList, root: OLValue, temp: OLValue) -> OLValue {
        return self
    }
    func some(function: OLList, root: OLValue, temp: OLValue) -> Bool {
        return false
    }
    func toArray() -> [AnyObject] {
        return []
    }
}

func toOLValue(a: AnyObject?) -> OLValue {
    switch a {
    case let b as NSNumber:
        return b
    case let b as NSString:
        return b
    case let b as NSArray:
        return b
    case let b as NSDictionary:
        return b
    default:
        return NSNull()
    }
}


