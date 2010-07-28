/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 1803 $  // Revision of last commit
  Date: $Date: 2010-07-15 13:48:08 -0400 (Thu, 15 Jul 2010) $  // Date of last commit
=========================================================================*/

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
