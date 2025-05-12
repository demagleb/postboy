#!/bin/bash

cd .. && cd build && rm -rf .[!.] .??* *
echo "*
!.gitignore" > .gitignore
