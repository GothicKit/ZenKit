/*
 * Copyright © 2024 GothicKit Contributors.
 * SPDX-License-Identifier: MIT
 */

const SPACER_TYPE_NAMES = new Map([
    ["sp-folder", "Property Group"],
    ["sp-int", "Integer"],
    ["sp-float", "Float"],
    ["sp-string", "String"],
    ["sp-misc", "Raw"],
    ["sp-vec", "3D Vector"],
    ["sp-bool", "Boolean"],
    ["sp-enum", "Enum"],
    ["sp-class", "Class"],
    ["sp-color", "Color"],
    ["sp-none", ""],
]);

document$.subscribe(() => {
    for (const [k, v] of SPACER_TYPE_NAMES) {
        const items = document.querySelectorAll(`.${k}`)
        items.forEach(e => {
            e.title = v;
        })
    }
})
