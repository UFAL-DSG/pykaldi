#!/bin/bash

svndir=$1

git ls-files |
while read f ; do
    dir_name="$svndir/`dirname $f`"
    # echo $dir_name
    mkdir -p "$svndir/$dir_name"
    cp -f "$f" "$svndir/$f"
    pushd "$svndir" >/dev/null
    svn add "$f"
    popd >/dev/null
done

