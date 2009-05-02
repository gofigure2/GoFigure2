#include "itkMegaCaptureImport.h"
#include "itkMultiFileReader.h"

#include "vtkImageViewer2.h"
#include "vtkMetaImageWriter.h"

int main( int argc, char * argv[] ) 
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
  reader->SetTimePoint( 20 );
  reader->SetChannel( 0 );
  reader->Update();

  vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
  writer->SetFileName( "Whatever.mha" );
  writer->SetInput( reader->GetOutput() );
  writer->Write();

  vtkImageViewer2* viewer = vtkImageViewer2::New();
  viewer->SetInput( reader->GetOutput() );
  viewer->SetSlice( ( viewer->GetSliceMax() - viewer->GetSliceMin() ) / 2 );
  viewer->Render();

  return EXIT_SUCCESS;
}

