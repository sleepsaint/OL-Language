//
//  olfunction.swift
//  ol-swift
//
//  Created by 伍威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

var OLFunction = [
    "+": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        var ret:Double = 0
        for a in params {
            if let i = a as? NSNumber {
                ret += i.doubleValue
            }
        }
        return NSNumber(double:ret)
    },
    "*": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        if params.count == 0 { return 0}
        var ret:Double = 1
        for a in params {
            if let i = a as? NSNumber {
                ret *= i.doubleValue
            } else {
                return 0
            }
        }
        return NSNumber(double:ret)
    },
    "-": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        if let a = params[0] as? NSNumber {
            if let b = params[1] as? NSNumber {
                return NSNumber(double:a.doubleValue - b.doubleValue)
            }
        }
        return NSNull()
    },
    "/": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        if let a = params[0] as? NSNumber {
            if let b = params[1] as? NSNumber {
                return NSNumber(double:a.doubleValue / b.doubleValue)
            }
        }
        return NSNull()
    },
    ">": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:params[0].compare(params[1]) > 0)
    },
    ">=": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:params[0].compare(params[1]) >= 0)
    },
    "=": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:params[0].compare(params[1]) == 0)
    },
    "<": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:params[0].compare(params[1]) < 0)
    },
    "<=": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:params[0].compare(params[1]) <= 0)
    },
    "!=": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:params[0].compare(params[1]) != 0)
    },
    "not": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        return NSNumber(bool:!params[0].toBool())
    },
    "or": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        for p in params {
            if p.toBool() {
                return true
            }
        }
        return false
    },
    "and": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        for p in params {
            if p.toBool() {
                return false;
            }
        }
        return NSNumber(bool:params.count > 0)
    },
    "filter": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        if let function = autoLookup(root, temp, now, params[1]) as? OLList {
            return autoLookup(root, temp, now, params[0]).filter(function, root: root, temp: temp)
        }
        return NSNull()
    },
    "sort": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        var result = autoLookup(root, temp, now, params[0]).toArray()
        if let a = autoLookup(root, temp, now, params[1]) as? OLSourceValue {
            a.sort(&result, root: root, temp: temp)
        }
        return result as NSArray
    },
    "some": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        if let function = autoLookup(root, temp, now, params[1]) as? OLList {
            return NSNumber(bool: autoLookup(root, temp, now, params[0]).some(function, root: root, temp: temp))
        }
        return NSNull()
    },
    "random": {(params:Array<OLValue>, root:OLValue, temp: OLValue, now: OLValue) -> OLValue in
        let ret = Double(arc4random()) / Double(UInt32.max)
        switch params.count {
        case 0:
            return NSNumber(double: ret)
        case 1:
            if let a = params[0] as? NSNumber {
                return NSNumber(double:floor(ret * a.doubleValue))
            }
        default:
            if let a = params[0] as? NSNumber {
                if let b = params[1] as? NSNumber {
                    return NSNumber(double:a.doubleValue + floor(ret * (b.doubleValue - a.doubleValue)))
                }
            }
        }
        return NSNull()
    }
]

