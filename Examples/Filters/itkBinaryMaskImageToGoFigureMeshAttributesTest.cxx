#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkBinaryMaskImageToGoFigureMeshAttributes.h"

int main( int argc, char** argv )
{
  const unsigned int Dimension = 3;
  typedef double PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::BinaryMaskImageToGoFigureMeshAttributes< ImageType, ImageType > AttributesFitlerType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );
  reader2->Update();

  AttributesFitlerType::Pointer filter = AttributesFitlerType::New();
  filter->SetImage( reader->GetOutput() );
  filter->SetMaskImage( reader2->GetOutput() );
  filter->Update();
  
  return EXIT_SUCCESS;
}
