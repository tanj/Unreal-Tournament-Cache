#!/bin/bash

# create all the files and directories to test against

CACHE=`find . -name ut99_cache.ini -type f -print`
CACHE_DIR="test/Cache"
rm -rf $CACHE_DIR

mkdir $CACHE_DIR

FILE=`awk '{print $2}' RS== $CACHE`
CONTENTS=(`awk '{print $1}' RS== $CACHE`)

#skip [Cache]
i=1


for new in $FILE
do
echo "${CONTENTS[i]}" > "${CACHE_DIR}/${new}.uxx"
i=$i+1
done

cp $CACHE $CACHE_DIR/cache.ini
exit 0
