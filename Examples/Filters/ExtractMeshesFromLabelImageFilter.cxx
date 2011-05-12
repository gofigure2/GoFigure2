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
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"
#include "itkExtractMeshesFromLabelImageFilter.h"
#include "itkNumericSeriesFileNames.h"
#include <fstream>

int main (int argc, char *argv[])
{
  if (argc < 3)
    {
    std::cout << "Usage: " << argv[0]
              << " InputVolume OutputMeshFormat numOfThreads"
              << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef itk::Image< unsigned int, Dimension > SegmentImageType;
  typedef itk::ImageFileReader< SegmentImageType > ReaderType;

  typedef itk::ExtractMeshesFromLabelImageFilter< SegmentImageType >
      MeshSourceType;

  typedef MeshSourceType::MeshType            MeshType;
  typedef itk::VTKPolyDataReader< MeshType >  MeshReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >  MeshWriterType;

  typedef itk::NumericSeriesFileNames         NameGeneratorType;
  typedef itk::VTKPolyDataWriter< MeshType >  MeshWriterType;

  // Define all of the variables
  unsigned int smoothingIterations = 8;
  double relaxationFactor = 0.75;
  unsigned int numberOfTriangles = 200;
  unsigned int numOfThreads = atoi( argv[3] );

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName ( argv[1] );
  reader->Update();
  SegmentImageType::Pointer input = reader->GetOutput();

  MeshSourceType::Pointer meshSource = MeshSourceType::New();
  meshSource->SetInput( input );
  meshSource->SetUseSmoothing( true );
  meshSource->SetUseDecimation( true );
  meshSource->SetNumberOfTrianglesPerMesh( numberOfTriangles );
  meshSource->SetNumberOfSmoothingIterations( smoothingIterations );
  meshSource->SetSmoothingRelaxationFactor( relaxationFactor );
  meshSource->SetDelaunayConforming( false );
  meshSource->SetNumberOfThreads( numOfThreads );
  meshSource->Update();

  NameGeneratorType::Pointer nameGeneratorOutput = NameGeneratorType::New();
  nameGeneratorOutput->SetSeriesFormat( argv[2] );
  nameGeneratorOutput->SetStartIndex( 1 );
  nameGeneratorOutput->SetEndIndex( meshSource->GetNumberOfMeshes() );
  nameGeneratorOutput->SetIncrementIndex( 1 );

  for( unsigned int i = 0; i < meshSource->GetNumberOfMeshes(); ++i )
  {
    std::stringstream ss;
    ss << nameGeneratorOutput->GetFileNames()[i];
//     std::cout << argv[0] << " writing " << ss.str() << std::endl;

    MeshWriterType::Pointer writer = MeshWriterType::New( );
    writer->SetInput( meshSource->GetOutputs()[i] );
    writer->SetFileName( ss.str().c_str() );
    writer->Update();
  }

  return EXIT_SUCCESS;
}
