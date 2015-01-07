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
        "^.wear.{^.person.{~.person}.wear.hat}.price",
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
        if let value = OLSource.parse(t) {
            println(value)
        }
    }
}
let root = NSJSONSerialization.JSONObjectWithData(NSString(string: "{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}}}").dataUsingEncoding(NSUTF8StringEncoding)!, options: NSJSONReadingOptions(0), error: nil) as NSDictionary
let temp = NSJSONSerialization.JSONObjectWithData(NSString(string: "{\"person\":\"P0001\",\"person2\":\"^.person.P0001\",\"wearnow\":\"upper\",\"personwear\":\"^.wear.{~.person2.wear.{~.wearnow}}\",\"wearfilter1\":\"#(>, @.price, $150)\",\"wearsorter1\":\"#(!(=,@.name,Red Hat),!@.price))\",\"now\":\"^.wear\"}").dataUsingEncoding(NSUTF8StringEncoding)!, options: NSJSONReadingOptions(0), error: nil) as NSDictionary

let test = [
    "^.wear.{^.person.{~.person}.wear.hat}.price",
    "^.wear.{~.person2.wear.hat}.price",
    "~.personwear.price",
    "(-, (+, ^.wear.W0001.price, ^.wear.W0002.price), ^.wear.W0002.price)",
    "(filter, ^.wear, #(>, @.price, $150))",
    "(filter, ^.wear, ~.wearfilter1)",
    "(or, !(>,^.wear.W0001.price,50), (>, ^.wear.W0002.price, 100) )",
    "(sort, ^.wear, #(@.price))",
    "(sort, ^.wear, #(!(=,@.name,Red Hat),@.price))",
    "(sort, ^.wear, ~.wearsorter1)",
    "(some, ^.wear, ~.wearfilter1)",
    "(sort, ~.now, ~.wearsorter1)",
    "(some, ~.now, #(>,@.price,50))",
    "(sort, ^.wear, #@.price)",
    "(sort, ^.wear, #!@.price)",
    "(random)",
    "(random, $5)",
    "(random, $-5, $-3)"
]

func test_lookup() {
    for t in test {
        if let value = OLSource.parse(t) {
            let a = value.lookup(root, temp: temp, now: root)
            println(t)
            println(a)
        }
    }
    
}
func test_lookup2() {
    for t in test {
        if let value = OLSource.parse(t) {
            let a = value.lookup(root, temp: temp, now: root)
            //            println(t)
            //            println(a)
        }
    }
    
}
func PP(f: ()->()) {
    let start = clock()
    for i in 0 ..< 10 {
        f()
    }
    let end = clock()
    println(end - start)
}

//test_parse()
//test_lookup()
PP(test_lookup2)