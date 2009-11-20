#include <iostream>
#include <cstdlib>
#include "LSMToMegaCapture.h"

int main( int argc, char** argv )
{
  if( argc != 4 )
    {
    std::cerr <<"lsmtomegacapture requires 3 argument:" <<std::endl;
    std::cerr <<"1-lsm filename" <<std::endl;
    std::cerr <<"2-Output directory" <<std::endl;
    std::cerr <<"3-png or tiff (png by default)" <<std::endl;
    return EXIT_FAILURE;
    }

  std::string type( argv[3] );
  GoFigure::FileType filetype = GoFigure::PNG;

  if( type.compare( "tiff" ) == 0 )
    {
    filetype = GoFigure::TIFF;
    }

  LSMToMegaCapture converter;
  converter.SetFileName( argv[1] );
  converter.SetOutputFileType( filetype );
  converter.Export( argv[2] );

  return EXIT_SUCCESS;
}
