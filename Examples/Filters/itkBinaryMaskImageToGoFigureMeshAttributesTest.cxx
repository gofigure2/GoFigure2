#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkBinaryMaskImageToGoFigureMeshAttributes.h"

int main(int argc, char** argv)
{
  if (argc != 3)
    {
    std::cerr << "./BinaryMaskImageToGoFigureMeshAttributesTest(.exe) takes 2 arguments" << std::endl;
    std::cerr << "1- input image filename" << std::endl;
    std::cerr << "2- input mask filename" << std::endl;
    return EXIT_FAILURE;
    }
  const unsigned int Dimension = 3;
  typedef double                                                             PixelType;
  typedef itk::Image<PixelType, Dimension>                                   ImageType;
  typedef itk::ImageFileReader<ImageType>                                    ReaderType;
  typedef itk::BinaryMaskImageToGoFigureMeshAttributes<ImageType, ImageType> AttributesFitlerType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName(argv[2]);
  reader2->Update();

  AttributesFitlerType::Pointer filter = AttributesFitlerType::New();
  filter->SetImage(reader->GetOutput());
  filter->SetMaskImage(reader2->GetOutput());
  filter->Update();

  std::cout << "size " << filter->GetSize() << std::endl;
  std::cout << "volume " << filter->GetPhysicalSize() << std::endl;
  std::cout << "mean intensity " << filter->GetMeanIntensity() << std::endl;
  std::cout << "total intensity " << filter->GetSumIntensity() << std::endl;
  return EXIT_SUCCESS;
}
