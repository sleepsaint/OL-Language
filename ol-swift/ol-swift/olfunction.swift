//
//  olfunction.swift
//  ol-swift
//
//  Created by 伍威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

struct OL {
    
    static func toBool(a: AnyObject?) -> Bool {
        switch a {
        case let number as NSNumber:
            return number != 0
        case let string as NSString:
            return string.length > 0
        case let array as NSArray:
            return array.count > 0
        case let dict as NSDictionary:
            return dict.count > 0
        default:
            return false
        }
    }
    
    static func compare(a: Double, _ b: Double) -> Int {
        if a > b { return 1 }
        else if a < b { return -1 }
        else { return 0 }
    }
    
    static func compare(a: String, _ b: String) -> Int {
        if a > b { return 1 }
        else if a < b { return -1 }
        else { return 0}
    }
    
    static func compare(a: NSNumber, _ b: AnyObject?) -> Int {
        if let number = b as? NSNumber {
            return compare(a.doubleValue, number.doubleValue)
        } else if let s = b as? String {
            if let n = s.toInt() {
                return compare(a.doubleValue, Double(n))
            } else {
                return compare(a.intValue.description, s)
            }
        }
        return 0
    }
    
    static func compare(a: AnyObject?, _ b: AnyObject?) -> Int {
        if let number = a as? NSNumber {
            return compare(number, b)
        }
        
        if let number = b as? NSNumber {
            return -compare(number, a)
        }
        
        if let s1 = a as? String {
            if let s2 = b as? String {
                return compare(s1, s1)
            }
        }
        
        return 0
    }
    
    static func compare(value: Value, left: AnyObject?, right: AnyObject?, root: AnyObject, temp: AnyObject) -> Int {
        if let a: AnyObject = left {
            if let b: AnyObject = right {
                if let n = value as? Negative {
                    return -compare(n.value.lookup(root, temp: temp, now: a), n.value.lookup(root, temp: temp, now: b))
                } else {
                    return compare(value.lookup(root, temp: temp, now: a), value.lookup(root, temp: temp, now: b))
                }
            } else {
                return 1
            }
        } else {
            if right != nil {
                return -1
            } else {
                return 0
            }
        }
       
    }
    
    static var Function = [
        "+": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            var ret:Double = 0
            for a in params {
                if let i = a as? NSNumber {
                    ret += i.doubleValue
                }
            }
            return ret
        },
        "*": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            if params.count == 0 { return 0}
            var ret:Double = 1
            for a in params {
                if let i = a as? NSNumber {
                    ret *= i.doubleValue
                } else {
                    return 0
                }
            }
            return ret
        },
        "-": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            if let a = params[0] as? NSNumber {
                if let b = params[1] as? NSNumber {
                    return a.doubleValue - b.doubleValue
                }
            }
            return nil
        },
        "/": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            if let a = params[0] as? NSNumber {
                if let b = params[1] as? NSNumber {
                    return a.doubleValue / b.doubleValue
                }
            }
            return nil
        },
        ">": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
               return OL.compare(params[0], params[1]) > 0
        },
        ">=": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            return OL.compare(params[0], params[1]) >= 0
        },
        "=": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            return OL.compare(params[0], params[1]) == 0
        },
        "<": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            return OL.compare(params[0], params[1]) < 0
        },
        "<=": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            return OL.compare(params[0], params[1]) <= 0
        },
        "!=": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            return OL.compare(params[0], params[1]) != 0
        },
        "not": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            return !OL.toBool(params[0])
        },
        "or": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            for p in params {
                if OL.toBool(p) {
                    return true
                }
            }
            return false
        },
        "and": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            for p in params {
                if !OL.toBool(p) {
                    return false
                }
            }
            return params.count > 0
        },
        "filter": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            if let function = OL.autoLookup(root, temp: temp, now: now, current: params[1]) as? OL.List {
                switch OL.autoLookup(root, temp: temp, now: now, current: params[0]) {
                case let array as NSArray:
                    let ret = NSMutableArray()
                    for i in array {
                        if OL.toBool(function.lookup(root, temp: temp, now: i)) {
                            ret.addObject(i)
                        }
                    }
                    return ret
                case let dict as NSDictionary:
                    let ret = NSMutableDictionary()
                    for (key, value) in dict {
                        if OL.toBool(function.lookup(root, temp: temp, now: value)) {
                            ret.setObject(value, forKey: key as NSString)
                        }
                    }
                    return ret
                default:
                    return nil
                }
            }
            return nil
        },
        "sort": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            var result : Array<AnyObject>!
            switch OL.autoLookup(root, temp: temp, now: now, current: params[0]) {
            case let array as NSArray:
                result = array
            case let dict as NSDictionary:
                result = dict.allValues
            default:
                return nil
            }
            
            switch OL.autoLookup(root, temp: temp, now: now, current: params[1]) {
            case let list as OL.List:
                result.sort({
                    var ret = OL.compare(list.head, left: $0, right: $1, root: root, temp: temp)
                    for i in list.tail {
                        if ret != 0 { return ret < 0 }
                        else { ret = OL.compare(i, left: $0, right: $1, root: root, temp: temp) }
                    }
                    return ret < 0
                })
            case let path as OL.Path:
                result.sort({
                    return OL.compare(path, left: $0, right: $1, root: root, temp: temp) < 0
                })
            case let negative as OL.Negative:
                result.sort({
                    return OL.compare(negative.value, left: $0, right: $1, root: root, temp: temp) > 0
                })
            default:
                return nil
            }
            return result
        },
        "some": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            if let function = OL.autoLookup(root, temp: temp, now: now, current: params[1]) as? OL.List {
                switch OL.autoLookup(root, temp: temp, now: now, current: params[0]) {
                case let array as NSArray:
                    for i in array {
                        if OL.toBool(function.lookup(root, temp: temp, now: i)) {
                            return true
                        }
                    }
                    return false
                case let dict as NSDictionary:
                    for (key, value) in dict {
                        if OL.toBool(function.lookup(root, temp: temp, now: value)) {
                            return true
                        }
                    }
                    return false
                default:
                    return nil
                }
            }
            return nil
        },
        "random": {(params:Array<AnyObject?>, root:AnyObject, temp: AnyObject, now: AnyObject) -> AnyObject? in
            let ret = Double(arc4random()) / Double(UInt32.max)
            switch params.count {
            case 0:
                return ret
            case 1:
                if let a = params[0] as? NSNumber {
                    return floor(ret * a.doubleValue)
                }
            default:
                if let a = params[0] as? NSNumber {
                    if let b = params[1] as? NSNumber {
                        return a.doubleValue + floor(ret * (b.doubleValue - a.doubleValue))
                    }
                }
            }
            return nil
        }
    ]
}
