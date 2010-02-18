#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include <fstream>
#include <sstream>

int main( int argc, char** argv )
{
  if( argc != 5 )
    {
    std::cerr <<"1-mha filename" <<std::endl;
    std::cerr <<"2-megacapture header (*.meg)" <<std::endl;
    std::cerr <<"3-Time Interval (in sec)" <<std::endl;
    std::cerr <<"4-Number of time points" <<std::endl;
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

  double iTimeInterval = atof( argv[3] );
  int iNumberOfTimePoints = atoi( argv[4] );

  std::ofstream file( argv[2] );
  file <<"MegaCapture" <<std::endl;
  file <<"<ImageSessionData>" <<std::endl;
  file <<"Version 3.0" <<std::endl;
  file <<"ExperimentTitle " <<std::endl;
  file <<"ExperimentDescription " <<std::endl;
  file <<"TimeInterval " <<iTimeInterval <<std::endl;
  file <<"Objective " <<std::endl;
  file <<"VoxelSizeX " <<spacing[0] <<std::endl;
  file <<"VoxelSizeY " <<spacing[1] <<std::endl;
  file <<"VoxelSizeZ " <<spacing[2] <<std::endl;
  file <<"DimensionX " <<dim[0] <<std::endl;
  file <<"DimensionY " <<dim[1] <<std::endl;
  file <<"DimensionPL 1"<<std::endl;
  file <<"DimensionCO 1" <<std::endl;
  file <<"DimensionRO 1" <<std::endl;
  file <<"DimensionZT 1" <<std::endl;
  file <<"DimensionYT 1" <<std::endl;
  file <<"DimensionXT 1" <<std::endl;
  file <<"DimensionTM " <<iNumberOfTimePoints <<std::endl;
  file <<"DimensionZS " <<dim[2] <<std::endl;
  file <<"DimensionCH 3" <<std::endl;

  int red = 255;
  int green = 0;
  int blue = 0;
  file <<"ChannelColor00 " << red * 256 * 256 + green * 256 + blue <<std::endl;
  red = 0;
  green = 255;
  blue = 0;
  file <<"ChannelColor01 " << red * 256 * 256 + green * 256 + blue <<std::endl;
  red = 0;
  green = 0;
  blue = 255;
  file <<"ChannelColor02 " << red * 256 * 256 + green * 256 + blue <<std::endl;

  file <<"ChannelDepth 8" <<std::endl;
  file <<"FileType PNG" <<std::endl;
  file <<"</ImageSessionData>" <<std::endl;

  file.close();

  reader->Delete();

  return EXIT_SUCCESS;
}
