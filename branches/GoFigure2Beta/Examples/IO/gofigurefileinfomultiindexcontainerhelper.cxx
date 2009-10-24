#include "GoFigureFileInfoMultiIndexContainerHelper.h"

#include <iostream>

int main( int , char** )
{
  FileInfoHelperMultiIndexContainer container;

  int plaque = 0;
  int r = 0;
  int c = 0;
  int xtile = 0;
  int ytile = 0;
  int ztile = 0;
  int xs = 0;
  int ys = 0;

  for( int t = 0; t < 10; t++ )
    {
    for( int ch = 0; ch < 5; ch++ )
      {
      for( int z = 0; z < 20; z++ )
        {
        std::stringstream filename;
        filename << "yo_p0_r0_c0_xt0_yt0_zt0_zs" <<z <<"_ch" <<ch <<"_t" <<t <<".png";
        container. insert( FileInfoHelper( plaque, r, c, xtile, ytile, ztile,
                  xs, ys, z, t, ch, filename.str() ) );
        }
      }
    }


  std::list< std::string > list =
    GetAllFileNamesForGivenTCoordAndChannel( container, 5, 2 );

  for( std::list< std::string >::iterator it = list.begin();
      it != list.end();
      ++it )
    {
    std::cout <<*it <<std::endl;
    }

  list = GetAllFileNamesForGivenZCoordPointAndChannel( container, 12, 4 );

    for( std::list< std::string >::iterator it = list.begin();
      it != list.end();
      ++it )
    {
    std::cout <<*it <<std::endl;
    }

  return EXIT_SUCCESS;
}
