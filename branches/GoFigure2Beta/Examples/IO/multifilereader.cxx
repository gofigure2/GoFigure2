#include "itkMegaCaptureImport.h"
#include "itkMultiFileReader.h"

#include "vtkImageData.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage: ./multifilereader(.exe) takes 2 arguments" <<std::endl;
    std::cout <<"1-filename (.jpg)" <<std::endl;
    std::cout <<"2-timebased (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();
  GoFigureFileInfoHelperMultiIndexContainer listoffiles = importer->GetOutput();

//   bool timebased = ( atoi( argv[2] ) == 0 );
//
//   itk::MultiFileReader::Pointer reader = itk::MultiFileReader::New();
//   reader->SetTimeBased( timebased );
//   reader->SetDimensionality( 2 );
//   reader->SetFileType( itk::MultiFileReader::JPEG );
//   reader->MultiChannelImagesOn();
//
//   if( timebased )
//     {
//     std::sort( listoffiles->begin(), listoffiles->end(),
//       GoFigureFileInfoHelperTimeBasedCompare() );
//     }
//   else
//     {
//     std::sort( listoffiles->begin(), listoffiles->end(),
//       GoFigureFileInfoHelperZCoordBasedCompare() );
//     }
//
//   reader->SetInput( listoffiles );
//
//   if( timebased )
//     {
//     reader->SetTimePoint( 0 );
//     }
//   else
//     {
//     reader->SetZDepth( 0 );
//     }
//
//   reader->Update();
//
//   vtkImageData* image = reader->GetOutput();
//   std::cout <<*image <<std::endl;
//
//   image->Delete();

  return EXIT_SUCCESS;
}
