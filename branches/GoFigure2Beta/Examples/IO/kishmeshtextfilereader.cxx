#include "MeshTextFileImport.h"

#include <iostream>
#include <cstdlib>

int main( int argc, char** argv )
{
  if( argc != 4 )
    {
    std::cerr <<"./kishmeshtextfilereader 3 arguments:" <<std::endl;
    std::cerr <<"1-directory (path: ex: ~/Data/)"  <<std::endl;
    std::cerr <<"2-filename (ex: 0.txt)" <<std::endl;
    std::cerr <<"3-ImagingSessionId (number)" <<std::endl;
    return EXIT_FAILURE;
    }

  MeshTextFileImport reader( "localhost", "gofigure", "gofigure",
    "gofiguredatabase", atoi( argv[3] ) );
  reader.SetDirectory( argv[1] );
  reader.SetFileName( argv[2] );
  reader.Read();

  return EXIT_SUCCESS;
}
