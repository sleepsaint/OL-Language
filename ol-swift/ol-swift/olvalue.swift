//
//  olvalue.swift
//  ol-swift
//
//  Created by 伍 威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

extension OL {
    
    class Value {
        
        class String2: Value, Printable {
            let value: String
            init(value: String) {
                self.value = value
            }
            var description : String {
                return value
            }
        }
        
        class Number: Value, Printable {
            let value: Double
            init(value: Double) {
                self.value = value
            }
            var description : String {
                return "\(value)"
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
        }
        
        class Negative: Value, Printable {
            let value: Value
            init(value: Value) {
                self.value = value
            }
            var description : String {
                return "!\(value)"
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
        }
    }
 
}