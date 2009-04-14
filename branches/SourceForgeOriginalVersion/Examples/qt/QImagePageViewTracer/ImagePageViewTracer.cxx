#include <qapplication.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <vtkImageData.h>

#include <vtkImageImport.h>

#include "itkImageToVTKImageFilter.h"
#include "QImagePageViewTracer.h"

int main( int argc, char** argv )
{
  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef ImageReaderType::Pointer ImageReaderPointer;

  ImageReaderPointer reader = ImageReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update( );

  typedef itk::ImageToVTKImageFilter< ImageType > ITKVTKConnectorType;
  ITKVTKConnectorType::Pointer connector = ITKVTKConnectorType::New();
  connector->SetInput( reader->GetOutput() );
  connector->Update();

  vtkImageData* image = connector->GetOutput();
  //   image->SetExtent( 0, size[0], 0, size[1], 0, size[2] );

  QApplication app(argc, argv);
  QImagePageViewTracer form;
  form.SetImage( image );
  form.SetBackgroundColor( 0., 0., 0. );
//   form.FullScreenView1();
  app.setMainWidget( &form );
  form.show( );
  //form.SetView4ToVolumeRenderingMode();

  return app.exec();
}
