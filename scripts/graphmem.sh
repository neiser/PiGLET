#!/bin/zsh

for ((i=0;;++i)) { echo $i `grep VmSize /proc/\`pidof PiGLET\`/status | grep -o '[0-9]*'`; \
    sleep 1 || break; }
