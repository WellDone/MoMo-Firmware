#!/bin/bash

rm -f /tmp/xc8_report

for var
do
egrep -o '^[ ]*__size_of_[a-zA-Z_0-9]+ [0-9A-F]{4}' "$var" | sed "s/[ \t]*$//" | sed "s/^[ \t]*//" | cut -c 11- >> /tmp/xc8_report
egrep -o '__size_of_[a-zA-Z_0-9]+ [0-9A-F]{4}[ ]*$' "$var" | sed "s/[ \t]*$//" | sed "s/^[ \t]*//" | cut -c 11- >> /tmp/xc8_report
done 

cat /tmp/xc8_report
awk --non-decimal-data '{s+=$2} END {print s}' < /tmp/xc8_report
