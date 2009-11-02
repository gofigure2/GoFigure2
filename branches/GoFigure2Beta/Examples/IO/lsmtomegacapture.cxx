#include "LSMToMegaCapture.h"

int main( int argc, char** argv )
{
  LSMToMegaCapture converter;
  converter.SetFileName( argv[1] );
  converter.Export();

  return EXIT_SUCCESS;
}
