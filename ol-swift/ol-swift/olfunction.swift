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
            if let a = params[0] as? NSNumber {
                if let b = params[1] as? NSNumber {
                    return a.doubleValue > b.doubleValue
                }
            }
            return nil
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
        }
    ]
}
