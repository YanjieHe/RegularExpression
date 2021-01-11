#!/bin/bash

clear
cd build/
cmake ..
status=$?
if test $status -eq 0
then
	make -j8
	status=$?
	if test $status -eq 0
	then
		./RegularExpression
	fi
fi
cd ..
read -p 'Press [Enter] key to continue...'
