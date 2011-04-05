#!/bin/bash
# Compare headers contained in a repository and the one installed
# ./verify_header_install.sh ~/GITROOT/GoFigure2 ~/GITROOT/BUILDGOFIGURE2 ~/TEMP ~/GITROOT/INSTALLGOFIGURE2

source_dir=${1}
binary_dir=${2}
temp_dir=${3}
install_dir=${4}

mkdir -p ${temp_dir}/gofigure2_includes

rm -rf ${temp_dir}/gofigure2_includes/*.*

for extension in h txx inl
do
  for dir in ${source_dir} ${binary_dir}
  do
    find ${dir} -name \*.${extension} -exec cp {} ${temp_dir}/gofigure2_includes/ \;
  done
done

cd ${source_dir}/Code/Attic;
for i in `ls *.h`;
do
  rm ${temp_dir}/gofigure2_includes/${i};
done

cd ${source_dir}/KWStyle;
for i in `ls *.h`;
do
  rm ${temp_dir}/gofigure2_includes/${i};
done

cd ${source_dir}/Plugins/ImageMedianPlugin;
for extension in h txx;
do
  for i in `ls *.${extension}`;
  do
    rm ${temp_dir}/gofigure2_includes/${i};
  done
done

#echo Attic
#find ${source_dir}/Code/Attic -name \*.h -print; #-exec rm {} ${temp_dir}/gofigure2_includes/ \;

diff ${temp_dir}/gofigure2_includes ${install_dir}/include/gofigure2
#| wc -l;

