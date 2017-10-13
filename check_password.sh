#!/bin/bash

EXIT_ERROR=2
EXIT_FOUND=0
EXIT_NOTFOUND=1

if [ $# -lt 1 ]
then
    echo "Usage: $0 password"
    exit $EXIT_ERROR
fi

export PATH=$PATH:.

EXEC_DEPS="openssl find_password"
OTHER_DEPS="pwned-passwords-1.0.txt pwned-passwords-update-1.txt pwned-passwords-update-2.txt"
for dep in $EXEC_DEPS; do
    # check for dependencies
    which -s $dep
    if [ $? -ne 0 ]; then
        echo "Missing dependency $dep!"
        exit $EXIT_ERROR
    fi
done

for dep in $OTHER_DEPS; do
    if [ ! -f $dep ]; then
        echo "Missing dependency $dep!"
        exit $EXIT_ERROR
    fi
done



PASSWORD="$1"
HASH=$(echo -n "$PASSWORD" | openssl sha1)

PASSWORD_LISTS=$(ls pwned-passwords-*.txt)

for list in $PASSWORD_LISTS
do
    PATH=$PATH:. find_password $list "$HASH" >> .find_password_log 2>&1
    RET=$?
    if [ $RET -eq $EXIT_FOUND ]
    then
        exit $EXIT_FOUND
    elif [ $RET -ne $EXIT_NOTFOUND ]
    then
        echo "Error calling find_password"
        exit $EXIT_ERROR
    fi
done

echo "Password $PASSWORD not found"
exit $EXIT_NOTFOUND
    
