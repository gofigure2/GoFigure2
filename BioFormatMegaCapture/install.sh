#!/bin/bash


#This script will download the bio-formats command line tools in bftools.zip 
#and the LOCI Tools complete bundle loci_tools.jar for reading images
#into ImageJ from the Bio-Formats website 
#http://www.loci.wisc.edu/bio-formats/downloads

mkdir bioformatsTools

wget "http://www.loci.wisc.edu/files/software/loci_tools.jar"

mv loci_tools.jar bioformatsTools

wget "http://www.loci.wisc.edu/files/software/bftools.zip"

unzip bftools.zip -d bioformatsTools

rm bftools.zip
