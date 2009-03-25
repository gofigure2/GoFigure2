#include <qapplication.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <vtkImageData.h>
#include <itkRGBPixel.h>

#include <vtkImageImport.h>

#include "itkImageToVTKImageFilter.h"
#include "QColorImagePageView.h"

int main( int argc, char** argv )
{
  const unsigned int Dimension = 3;
  typedef itk::RGBPixel< unsigned char >     PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType >  ImageReaderType;
  typedef ImageReaderType::Pointer           ImageReaderPointer;

  ImageReaderPointer reader = ImageReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update( );

  typedef itk::ImageToVTKImageFilter< ImageType > ITKVTKConnectorType;
  ITKVTKConnectorType::Pointer connector = ITKVTKConnectorType::New();
  connector->SetInput( reader->GetOutput() );
  connector->Update();

  vtkImageData* image = connector->GetOutput();

  QApplication app(argc, argv);
  QColorImagePageView form;
  form.SetImage( image );
  form.SetBackgroundColor( 0., 0., 0. );
  app.setMainWidget( &form );
  form.show( );

  return app.exec();
}
