#!/bin/bash

if [ "$1" == "" ]
then
	mpirun -n 2 ./kkk -t online-mpi -i 10000 -a 1000
else
	mpirun -n $1 ./kkk -t online-mpi -i 10000 -a 1000
fi
