/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "vtkSphereSource.h"
#include "itkvtkPolyDataToBinaryMaskImageFilter.h"

int main(int argc, char **argv)
{
  if ( argc != 3 )
    {
    std::cerr << "./itkvtkPolyDataToBinaryMaskImageFilter(.exe) takes 2 arguments" << std::endl;
    std::cerr << "1- input image filename" << std::endl;
    std::cerr << "2- output image filename" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef unsigned char                      PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType >  ReaderType;

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

  vtkSphereSource *sphere_source = vtkSphereSource::New();
  sphere_source->SetCenter(center[0], center[1], center[2]);
  sphere_source->SetRadius(0.25 * spacing[0] * size[0]);
  sphere_source->Update();

  typedef itk::vtkPolyDataToBinaryMaskImageFilter< ImageType, ImageType >
  BinarizerType;
  BinarizerType::Pointer binarize = BinarizerType::New();
  binarize->SetInput( reader->GetOutput() );
  binarize->SetPolyData( sphere_source->GetOutput() );
  binarize->Update();

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( binarize->GetOutput() );
  writer->SetFileName(argv[2]);
  writer->Write();

  sphere_source->Delete();

  return EXIT_SUCCESS;
}