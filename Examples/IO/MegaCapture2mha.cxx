#include "itkMegaCaptureImport.h"
#include "itkMultiFileReader.h"

#include "vtkMetaImageWriter.h"

int main( int argc, char * argv[] ) 
{
  if( argc < 5 ) return EXIT_FAILURE;

  try
    {
    itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
    importFileInfoList->SetFileName( argv[1] );
    importFileInfoList->Update();

    //  itk::MultiFileReader* reader = itk::MultiFileReader::New();
    itk::MultiFileReader* reader = new itk::MultiFileReader;
    reader->SetInput( importFileInfoList->GetOutput( ) );
    reader->SetDimensionality( 2 );
    reader->SetFileType( JPEG );
    reader->SetMultiChannelImagesOFF();
    reader->SetTimePoint( atoi( argv[3] ) );
    reader->SetChannel(   atoi( argv[4] ) );
    reader->Update();

    vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
    writer->SetFileName( argv[2] );
    writer->SetInput( reader->GetOutput() );
    writer->Write();
    }   
  catch( const itk::ExceptionObject& e )
    {
    std::cerr << "MegaCapture2mha caught an ITK exception: " << std::endl;
    e.Print( std::cerr);
    return EXIT_FAILURE;
    }
  catch( const std::exception& e )
    {
	std::cerr << "MegaCapture2mha caught an std exception: " << std::endl;
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
  catch( ... )
    {
    std::cerr << "MegaCapture2mha caught an unknown exception!" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

