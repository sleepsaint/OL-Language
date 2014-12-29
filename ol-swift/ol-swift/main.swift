//
//  main.swift
//  ol-swift
//
//  Created by 伍 威 on 14/12/29.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

import Foundation

func test_parse() {
    let test = [
        "#(abd,bdf)",
        "(abd, adfsdf, asdfd, ",
        "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4) )",
        "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4)",
        "!()",
        "!(abd, !@.123.abd.{@.abd}.)",
        "(abd, !@.123.abd.$123.{@.abd})",
        "(abd, sdfe",
        "(abd, !@.123.abd.123.{@.abd.{~.abd})",
        "(abd, !@.123.abd.123.{@.abd.{.abd})",
        "(abd, !)"
    ]
    for t in test {
        if let value = OL.parse(t) {
            println(value)
        }
    }
}

func test_lookup() {
    var root = OLDictionary()
    root.dict = ["abc":1]
    let test = [
        "^.abc"
    ]
    for t in test {
        if let value = OL.parse(t) {
            if let a = value.lookup(root, temp: root, now: root) {
                println(a)
            }
        }
    }

}


test_lookup()