#!/bin/bash
loc=70
COUNT=0

while [ $loc -gt -155 ]; do
   echo "$COUNT $loc 5.00 -94.00" >> path 
   echo "$COUNT $loc 5.00 -116.00" >> path 

   let "COUNT += 1"
   let "loc -= 1"
   let "loc = $loc - $RANDOM%3"
done
