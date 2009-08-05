#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCellPreprocess.h"

int main ( int argc, char* argv[] )
  {
  if ( argc < 2 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " InputImage OutputImage" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned char, Dimension > InputImage;
  typedef itk::ImageFileReader< InputImage > ReaderType;
  typedef itk::ImageFileWriter< InputImage > WriterType;

  typedef itk::CellPreprocess< InputImage, InputImage >
  FilterType;

  InputImage::Pointer input;

    {
    ReaderType::Pointer cellReader = ReaderType::New();
    cellReader->SetFileName ( argv[1] );
    cellReader->Update();
    input = cellReader->GetOutput();
    input->DisconnectPipeline();
    }

  InputImage::SpacingType spacing = input->GetSpacing();

  std::cout << "Spacing: " << spacing << std::endl;
//   spacing[0] = spacing[1] = 0.2; spacing[2] = 1.0;
//   input->SetSpacing( spacing );

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput ( input );
  filter->SetLargestCellRadius ( 4.0 ); // in real coordinates
  filter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput ( filter->GetOutput() );
  writer->SetFileName ( argv[2] );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject e )
    {
    std::cerr << "Error: " << e << std::endl;
    }

  return EXIT_SUCCESS;
  }
