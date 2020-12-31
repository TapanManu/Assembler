#!/bin/bash 

g++ first-pass.cpp 
./a.out $1

g++ second-pass.cpp 
./a.out `head -1 $1 | awk 'NR==1 {print $1}'`_intermediate.txt
