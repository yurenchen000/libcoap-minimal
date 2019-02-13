#!/bin/sh

echo ------ env:
env
echo
echo ------ stdin:
cat
echo
echo ------ end.

#exec 0<&-
#exec 1>&-
#exec 2>&-
#exit
#echo hello

