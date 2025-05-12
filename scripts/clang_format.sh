#!/bin/bash

function format() {
    files=$(find bin src tests -iname *.$1)
    if [[ -n "$files" ]]; then
        echo $files | xargs clang-format-17 -i
    fi
}

cd ..

for fmt in hpp cpp h c; do
    format $fmt
done
