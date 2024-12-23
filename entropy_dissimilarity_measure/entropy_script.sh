#!/bin/bash

M=2
nbin=20
ns=20
np=3

rm -f entropy_measure.log
cd samples
N="$(ls -1 | wc -l)"
COUNT=`expr $N - 1`

echo Generating entropy dissimilarity measure ...
for (( i=0; i<=$COUNT; i++ ))
do
	file1=$(printf '%0.5ddtlz2.out' "$i")
        j=`expr $i + 1`
        file2=$(printf '%0.5ddtlz2.out' "$j")
	../entropy_measure/Release/entropy_measure $M $nbin $file1 $file2 | sed '/D:/!d' | sed 's/^.\{3\}//' >> ../entropy_measure.log
done

cd ..

./which_gen_single/Release/which_gen_single -s $ns -d $np entropy_measure.log

