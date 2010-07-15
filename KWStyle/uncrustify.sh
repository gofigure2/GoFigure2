#!/bin/sh
#
# Linux Shell Script to run ITK tests in linux
#
command=$1;
working_directory=`pwd`;
configuration_file="$working_directory"/uncrustify.cfg;
cd ..;

# find cxx files
for i in `find . -name *.cxx`;
do
   echo "Correcting file style: $i";
   $command -c $configuration_file -f $i -o $i;
   echo "Done";
done;

# find cxx.in files
for i in `find . -name *.cxx.in`;
do
   echo "Correcting file style: $i";
   $command -c $configuration_file -f $i -o $i;
   echo "Done";
done;

# find h files
for i in `find . -name *.h`;
do
   echo "Correcting file style: $i";
   $command -c $configuration_file -f $i -o $i;
   echo "Done";
done;

# find h.in files
for i in `find . -name *.h.in`;
do
   echo "Correcting file style: $i";
   $command -c $configuration_file -f $i -o $i;
   echo "Done";
done;

# find txx files
for i in `find . -name *.txx`;
do
   echo "Correcting file style: $i";
   $command -c $configuration_file -f $i -o $i;
   echo "Done";
done;

echo "Project's style updated"
