#!/bin/bash

a=0

until [ ! $a -lt 20 ]
do
   echo $a
   a=`expr $a + 1`
   ((a++))
done
