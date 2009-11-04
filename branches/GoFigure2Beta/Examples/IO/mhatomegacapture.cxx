#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include "vtkExtractVOI.h"
#include "vtkImageData.h"
#include "vtkPNGWriter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>


int main( int argc, char** argv )
{
  if( argc != 5 )
    {
    std::cerr <<"1-filename (mha)" <<std::endl;
    std::cerr <<"2-megacapture header (.meg)" <<std::endl;
    std::cerr <<"3-time point" <<std::endl;
    std::cerr <<"4-channel" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkImageData* image = reader->GetOutput();

  double spacing[3];
  image->GetSpacing( spacing );

  int dim[3];
  image->GetDimensions( dim );

  int extent[6];
  image->GetExtent( extent );

  std::ofstream file( argv[2], std::ios_base::app );

  int iTimePoint = atoi( argv[3] );
  int iChannel = atoi( argv[4] );

  int plaque = 0;
  int column = 0;
  int row = 0;
  int ztile = 0;
  int ytile = 0;
  int xtile = 0;

  char timeStr[ 100 ] = "";
  struct stat buf;

  if( !stat( argv[1], &buf ) )
    {
    strftime( timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime( &buf.st_mtime ) );
    }

  for( int i = extent[4]; i < extent[5]; i++ )
    {
    std::stringstream filename;
    filename <<"image-PL" << setfill('0') << setw(2) << plaque;
    filename <<"-CO" << setfill('0') << setw(2) << column;
    filename <<"-RO" << setfill('0') << setw(2) << row;
    filename <<"-ZT" << setfill('0') << setw(2) << ztile;
    filename <<"-YT" << setfill('0') << setw(2) << ytile;
    filename <<"-XT" << setfill('0') << setw(2) << xtile;
    filename <<"-TM" << setfill('0') << setw(4) << iTimePoint;
    filename <<"-ch" << setfill('0') << setw(2) << iChannel;
    filename <<"-zs" << setfill('0') << setw(4) << i;
    filename <<".png";

    file <<"<Image>"<<std::endl;
    file <<"Filename " <<filename.str() <<std::endl;
    file <<"DateTime " <<timeStr <<std::endl;
    file <<"StageX 1000" <<std::endl;
    file <<"StageY -1000" <<std::endl;
    file <<"Pinhole 44.216" <<std::endl;
    file <<"</Image>"<<std::endl;

    vtkExtractVOI* extract = vtkExtractVOI::New();
    extract->SetSampleRate( 1, 1, 1 );
    extract->SetInput( image );

    extract->SetVOI( extent[0], extent[1], extent[2], extent[3], i, i );
    extract->Update();

    vtkPNGWriter* writer2d = vtkPNGWriter::New();
    writer2d->SetInput( extract->GetOutput() );
    writer2d->SetFileName( filename.str().c_str() );
    writer2d->Write();

    writer2d->Delete();
    extract->Delete();
    }
  file.close();

  image->Delete();
  reader->Delete();

  return EXIT_SUCCESS;
}
