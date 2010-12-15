/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSingleCellSplitImageFilter.h"
#include <list>
#include <fstream>

int main ( int argc, char* argv[] )
  {
  if ( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " input output" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;
  typedef itk::Image< unsigned char, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType;
  typedef itk::SingleCellSplitImageFilter< ImageType > FilterType;

  ImageType::Pointer input;
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName ( argv[1] );
    reader->Update();
    input = reader->GetOutput();
    input->DisconnectPipeline();
    }

  ImageType::IndexType index1, index2;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput ( input );
  filter->SetForegroundValue( 1 );

  index1[0] = 40;
  index1[1] = 40;
  filter->m_Seeds.push_back( index1 );

  index2[0] = 60;
  index2[1] = 60;
  filter->m_Seeds.push_back( index2 );

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
