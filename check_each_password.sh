#!/bin/bash


cat $1 | (while read -r WORD; do   ./check_password.sh "$WORD"  > .out; cat .out; if [ -s .out ]; then echo "$WORD"; fi; done)
