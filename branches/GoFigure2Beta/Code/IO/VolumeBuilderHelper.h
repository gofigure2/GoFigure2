#ifndef __VolumeBuilderHelper_h
#define __VolumeBuilderHelper_h

#include <string>

class vtkImageData;
class vtkImageAppend;

template< class TReader >
void AddToVolumeBuilder(
  const int& iId,
  const std::string& iFileName,
  vtkImageAppend* iBuilder )
{
  typedef TReader ReaderType;
  ReaderType* reader = ReaderType::New();
  reader->SetFileName( iFileName.c_str() );
  reader->SetFileDimensionality( 2 );
  reader->Update();

  iBuilder->SetInput( iId, reader->GetOutput( ) );
  reader->Delete();
}

#endif
