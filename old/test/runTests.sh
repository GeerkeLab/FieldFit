#! /bin/bash

while read line
do
	name=$( echo $line | awk '{ print $1 }' );
	arguments=$( echo $line | awk '{ $1="" }1');

	echo -e "\n\n__________________________________________";
        echo -e "\nRUNNING TEST; \n\t$name\n";
        echo -e "__________________________________________\n";
	
	../bin/x64/fieldfit $arguments > error.out
	
	if (( $? == 1 )); then

		grep . error.out;

		echo -e "<<<<<  FAILED TEST! >>>>\n";

		exit 1;
	fi

	rm error.out;	

	echo -e "Finished!";

done < test.in
