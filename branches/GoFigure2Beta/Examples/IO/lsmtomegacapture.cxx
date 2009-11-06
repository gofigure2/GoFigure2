#include "LSMToMegaCapture.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cerr <<"lsmtomegacapture requires 1 argument:" <<std::endl;
    std::cerr <<"1-lsm filename" <<std::endl;
    return EXIT_FAILURE;
    }

  LSMToMegaCapture converter;
  converter.SetFileName( argv[1] );
  converter.Export( "image.meg" );

  return EXIT_SUCCESS;
}
