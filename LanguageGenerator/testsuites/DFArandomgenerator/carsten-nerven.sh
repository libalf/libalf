#!/bin/sh

let a=3;

while [ $a -le 50 ]; do
	export a
	LD_LIBRARY_PATH=../../src/ ./DFArandomgenerator 3 $a
	make jpegs
	mv random-f.dot.jpg random-`printf "%03d" $a`.jpg

	echo -e "welcome subscriber,\n\nnow we have a $a state automaton for you :)" | mutt -s "random automaton with $a states" -a random-`printf "%03d" $a`.jpg kern@cs.rwth-aachen.de

	let a++
	# wait 15 minutes :)
	sleep 900
done;

