#include "itkMegaCaptureImport.h"
#include "itkMultiFileReader.h"

#include "vtkImageData.h"

int main( int argc, char** argv )
{
  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();
  FileListType* listoffiles = importer->GetOutput();

  itk::MultiFileReader::Pointer reader = itk::MultiFileReader::New();
  reader->SetInput( listoffiles );
  reader->SetDimensionality( 2 );
  reader->SetFileType( itk::MultiFileReader::JPEG );
  reader->MultiChannelImagesOn();
  reader->SetTimeBased( true );
  reader->SetTimePoint( 0 );
  reader->Update();

  vtkImageData* image = reader->GetOutput();
  std::cout <<*image <<std::endl;

  return EXIT_SUCCESS;
}
