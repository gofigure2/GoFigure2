#include "itkMegaCaptureImport.h"
#include "itkMegaCaptureReader.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage: ./itkmegacapturereader(.exe)" <<std::endl;
    return EXIT_FAILURE;
    }

  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();

  GoFigureFileInfoHelperMultiIndexContainer listoffiles = importer->GetOutput();

  itk::MegaCaptureReader::Pointer reader = itk::MegaCaptureReader::New();
  reader->SetInput( listoffiles );
  reader->SetFileType( GoFigure::PNG );
  reader->SetTimeBased( true );
  reader->SetTimePoint( 0 );
  reader->SetChannel( 0 );
  reader->Update();

  vtkImageData* image = reader->GetOutput();
  std::cout <<image <<std::endl;

  return EXIT_SUCCESS;
}
