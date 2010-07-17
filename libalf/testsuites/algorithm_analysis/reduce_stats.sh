#!/bin/sh

gawk '{ if ( $5 <= 45 ) { print $5 " " $11 " " $16 " " $21 " " $26 } }' < statistics > bounded_stats

