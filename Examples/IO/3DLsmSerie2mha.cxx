#include "itkLsm3DSerieImport.h"
#include "itkMultiFileReader.h"

#include "vtkMetaImageWriter.h"


// usage <LsmSerie2Mha> in_file out_file timepoint channel
int main( int argc, char * argv[] ) 
{
  if( argc < 4 ) return EXIT_FAILURE;

  try
    {
    itk::Lsm3DSerieImport::Pointer  importFileInfoList = itk::Lsm3DSerieImport::New();
    importFileInfoList->SetFileName( argv[1] );
    importFileInfoList->SetGroupId( 1 );
    importFileInfoList->Update();

    //  itk::MultiFileReader* reader = itk::MultiFileReader::New();
    itk::MultiFileReader* reader = new itk::MultiFileReader;
    reader->SetInput( importFileInfoList->GetOutput( ) );
    reader->SetDimensionality( 3 );
    reader->SetFileType( LSM );
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
    std::cerr << "3DLsmSerieTest caught an ITK exception: " << std::endl;
    e.Print( std::cerr);
    return EXIT_FAILURE;
    }
  catch( const std::exception& e )
    {
	std::cerr << "3DLsmSerieTest caught an std exception: " << std::endl;
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
  catch( ... )
    {
    std::cerr << "3DLsmSerieTest caught an unknown exception!" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

