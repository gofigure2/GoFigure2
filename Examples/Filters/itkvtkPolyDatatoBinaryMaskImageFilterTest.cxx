#include "itkImage.h"
#include "itkImageFileReader.h"
#include "vtkSphereSource.h"
#include "itkvtkPolyDataToBinaryMaskImageFilter.h"

int main( int argc, char** argv )
{
  const unsigned int Dimension = 3;
  typedef double PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkSphereSource* sphere_source = vtkSphereSource::New();
  sphere_source->Update();

  typedef itk::vtkPolyDataToBinaryMaskImageFilter< ImageType, ImageType >
    BinarizerType;
  BinarizerType::Pointer binarize = BinarizerType::New();
  binarize->SetInput( reader->GetOutput() );
  binarize->SetPolyData( sphere_source->GetOutput() );
  binarize->Update();

  return EXIT_SUCCESS;
}
