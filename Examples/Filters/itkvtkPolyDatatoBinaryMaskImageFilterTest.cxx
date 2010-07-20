#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "vtkSphereSource.h"
#include "itkvtkPolyDataToBinaryMaskImageFilter.h"

int main(int argc, char** argv)
{
  if (argc != 3)
    {
    std::cerr << "./itkvtkPolyDataToBinaryMaskImageFilter(.exe) takes 2 arguments" << std::endl;
    std::cerr << "1- input image filename" << std::endl;
    std::cerr << "2- output image filename" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef unsigned char                    PixelType;
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef itk::ImageFileReader<ImageType>  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  ImageType::Pointer image = reader->GetOutput();

  ImageType::SizeType    size = image->GetLargestPossibleRegion().GetSize();
  ImageType::PointType   org = image->GetOrigin();
  ImageType::SpacingType spacing = image->GetSpacing();
  ImageType::PointType   center;
  center[0] = org[0] + 0.5 * spacing[0] * size[0];
  center[1] = org[1] + 0.5 * spacing[1] * size[1];
  center[2] = org[2] + 0.5 * spacing[2] * size[2];

  vtkSphereSource* sphere_source = vtkSphereSource::New();
  sphere_source->SetCenter(center[0], center[1], center[2]);
  sphere_source->SetRadius(0.25 * spacing[0] * size[0]);
  sphere_source->Update();

  typedef itk::vtkPolyDataToBinaryMaskImageFilter<ImageType, ImageType>
  BinarizerType;
  BinarizerType::Pointer binarize = BinarizerType::New();
  binarize->SetInput(reader->GetOutput());
  binarize->SetPolyData(sphere_source->GetOutput());
  binarize->Update();

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(binarize->GetOutput());
  writer->SetFileName(argv[2]);
  writer->Write();

  sphere_source->Delete();

  return EXIT_SUCCESS;
}
