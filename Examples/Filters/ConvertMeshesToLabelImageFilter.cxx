/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "itkConvertMeshesToLabelImageFilter.h"
#include "itkVTKPolyDataReader.h"
#include "itkNumericSeriesFileNames.h"
#include <fstream>

int main (int argc, char *argv[])
{
  if (argc < 5)
    {
    std::cout << "Usage: " << argv[0] << " InfoVolume InputMeshFormat OutputVolume numOfThreads numOfMeshes" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned int, Dimension > SegmentImageType;
  typedef itk::ImageFileReader< SegmentImageType > ReaderType;

  typedef itk::ConvertMeshesToLabelImageFilter< SegmentImageType > ImageSourceType;

  typedef ImageSourceType::MeshType           MeshType;
  typedef itk::VTKPolyDataReader< MeshType >  MeshReaderType;

  typedef itk::NumericSeriesFileNames                  NameGeneratorType;
  typedef itk::ImageFileWriter< SegmentImageType >     WriterType;

  // Define all of the variables
  unsigned int numOfThreads = atoi( argv[4] );
  unsigned int numOfMeshes  = atoi( argv[5] );

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName ( argv[1] );
  reader->Update();
  SegmentImageType::Pointer input = reader->GetOutput();

  SegmentImageType::Pointer m_OutputImage = SegmentImageType::New();
  m_OutputImage->CopyInformation( input );
  m_OutputImage->SetRegions( input->GetLargestPossibleRegion() );
  m_OutputImage->Allocate();
  m_OutputImage->FillBuffer( 0 );

  NameGeneratorType::Pointer nameGeneratorOutput = NameGeneratorType::New();
  nameGeneratorOutput->SetSeriesFormat( argv[2] );
  nameGeneratorOutput->SetStartIndex( 1 );
  nameGeneratorOutput->SetEndIndex( numOfMeshes );
  nameGeneratorOutput->SetIncrementIndex( 1 );

  ImageSourceType::Pointer imageSource = ImageSourceType::New();

  ImageSourceType::MeshVectorType meshes( numOfMeshes );

  for( unsigned int i = 0; i < numOfMeshes; ++i )
    {
    std::stringstream ss;
    ss << nameGeneratorOutput->GetFileNames()[i];
    //     std::cout << argv[0] << " writing " << ss.str() << std::endl;

    MeshReaderType::Pointer meshReader = MeshReaderType::New( );
    meshReader->SetFileName( ss.str().c_str() );
    meshReader->Update();
    meshes[i] = meshReader->GetOutput();
    meshes[i]->DisconnectPipeline();
    }

  imageSource->SetInput( m_OutputImage );
  imageSource->SetNumberOfThreads( numOfThreads );
  imageSource->SetMeshes( meshes );
  imageSource->Update();

  WriterType::Pointer writer = WriterType::New( );
  writer->SetInput( m_OutputImage );
  writer->SetFileName( argv[3] );
  writer->Update();

  return EXIT_SUCCESS;
}
