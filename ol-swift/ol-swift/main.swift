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

test_parse()