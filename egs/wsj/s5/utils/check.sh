#!/bin/bash

mkdir -p ckpts

fullname="./ckpts/`echo $@ | sed 's:[/!]:_:g'`"
if [[ ${#fullname} -gt 255 ]] ; then
  hash="`echo $fullname | md5`"
  prefix_len=`expr 255 - ${#hash}`
  name="${fullname:0:$prefix_len}$hash"
else
  name="$fullname"
fi

echo -e -n "\nCheckpoint $name "
if [ -f "$name" ] ; then
  echo "found! Skipping!"
else
  echo -e " will be created.\nRunning $@.\n"
  "$@" || return $?
  echo "$fullname" > "$name"
fi

return 0
