#! /bin/bash

while read line
do
	name=$( echo $line | awk '{ print $1 }' );
	arguments=$( echo $line | awk '{ $1="" }1');

	echo -e "\n\n__________________________________________";
        echo -e "\nRUNNING TEST; \n\t$name\n";
        echo -e "__________________________________________\n";
	
	../bin/x64/fieldfit $arguments;
	
	if (( $? == 1 )); then

		 echo -e "FAILED!\n";

		exit 1;
	fi
	echo -e "Finished!";

done < test.in
