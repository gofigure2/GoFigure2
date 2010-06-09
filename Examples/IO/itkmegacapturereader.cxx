#include "itkMegaCaptureImport.h"
#include "itkMegaCaptureReader.h"
#include "vtkImageData.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage: ./itkmegacapturereader(.exe) filename(.png)" <<std::endl;
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
  reader->Update();

  unsigned int minch = reader->GetMinChannel();
  unsigned int maxch = reader->GetMaxChannel();

  for( unsigned int ch = minch; ch < maxch; ch++ )
    {
    vtkImageData* image = reader->GetOutput( ch );
    image->Delete();
    }

  return EXIT_SUCCESS;
}
