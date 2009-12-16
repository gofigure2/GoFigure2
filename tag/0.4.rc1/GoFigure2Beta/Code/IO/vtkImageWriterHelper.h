#ifndef __vtkImageWriterHelper_h
#define __vtkImageWriterHelper_h

#include <QString>

class vtkImageData;

//-------------------------------------------------------------------------
template< class TWriter >
void vtkWriteImage( vtkImageData* iImage, const std::string& iFileName )
{
  typedef TWriter WriterType;
  WriterType* writer = WriterType::New();
  writer->SetInput( iImage );
  writer->SetFileName( iFileName.c_str( ) );
  writer->Write();
  writer->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TWriter >
void vtkWriteImage( vtkImageData* iImage, const QString& iFileName )
{
  vtkWriteImage< TWriter >( iImage, iFileName.toStdString() );
}
//-------------------------------------------------------------------------


#endif
