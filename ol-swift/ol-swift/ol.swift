//
//  ol.swift
//  ol-swift
//
//  Created by 伍 威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

class OL {
    
}

protocol OLLookup {
    subscript (index: OLLookup) -> OLLookup? {get}
}

protocol OLValue {
    func lookup(root: OLLookup, temp: OLLookup, now: OLLookup) -> OLLookup?
}

extension String : OLLookup {
    subscript (index: OLLookup) -> OLLookup? {
        return nil
    }
}

extension Double: OLLookup {
    subscript (index: OLLookup) -> OLLookup? {
        return nil
    }
}

class OLArray : OLLookup {
    var array = Array<OLLookup>()
    subscript (index: OLLookup) -> OLLookup? {
        if let s = index as? String {
            if let i = s.toInt() {
                return array[i]
            }
        }
        return nil
    }
}

class OLDictionary : OLLookup {
    var dict = Dictionary<String,OLLookup>()
    subscript (key: OLLookup) -> OLLookup? {
        if let s = key as? String {
                return dict[s]
        }
        return nil

    }
}