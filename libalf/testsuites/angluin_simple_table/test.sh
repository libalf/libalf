#!/bin/sh

let NUM=0;

cat regex_sample_set.txt |
	while read LINE; do
		REGEX=`echo "$LINE" | gawk '{print $3}'`
		MINSTATES=`echo "$LINE" | gawk '{print $2}'`
		echo $NUM: $REGEX;
		MY_MINSTATES=` ./learn_regex $REGEX | grep 'minimal state count:' | gawk '{print $4}'; `
		if [ "$MINSTATES" != "$MY_MINSTATES" ]; then
			echo $NUM: $REGEX >> testlog
			echo "fail: minimal state count differs: org: $MINSTATES, new: $MY_MINSTATES" >> testlog
			echo "" >> testlog
		fi;
		let NUM++;
	done;

