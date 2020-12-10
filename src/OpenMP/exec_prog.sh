#!/bin/bash

mpiexec -machinefile machines -np $1 ./heat $2 $3
