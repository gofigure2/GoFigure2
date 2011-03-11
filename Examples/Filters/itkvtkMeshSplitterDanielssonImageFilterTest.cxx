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


#include "itkvtkMeshSplitterDanielssonDistanceImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataWriter.h"

#include "vcl_algorithm.h"

int main( int argc, char* argv[] )
{
  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;

  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;

  ImageType::Pointer input;
    {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName ( argv[1] );
    reader->Update();
    input = reader->GetOutput();
    input->DisconnectPipeline();
    }

  ImageType::PointType im_org = input->GetOrigin();
  ImageType::SizeType im_size = input->GetLargestPossibleRegion().GetSize();

  ImageType::IndexType idx;
  idx[0] = im_size[0] - 1;
  idx[1] = im_size[1] - 1;
  idx[2] = im_size[2] - 1;

  ImageType::PointType p;
  input->TransformIndexToPhysicalPoint( idx, p );

  ImageType::PointType center;
  center.SetToMidPoint( im_org, p );

  ImageType::PointType::CoordRepType radius;
  radius = vcl_min( p[0] - im_org[0], p[1] - im_org[1] );
  radius = vcl_min( radius, p[2] - im_org[2] );

  radius *= 0.25;

  vtkSmartPointer< vtkSphereSource > sphere_source =
      vtkSmartPointer< vtkSphereSource >::New();
  sphere_source->SetCenter( center[0], center[1], center[2] );
  sphere_source->SetRadius( radius );
  sphere_source->Update();

  typedef itk::vtkMeshSplitterDanielssonDistanceImageFilter< ImageType >
      SplitterType;
  SplitterType::Pointer filter = SplitterType::New();
  filter->SetMesh( sphere_source->GetOutput() );
  filter->SetImage( input );

  typedef SplitterType::PointSetType PointSetType;
  PointSetType::Pointer seeds = PointSetType::New();

  p[0] = center[0] - 0.1 * radius;
  p[1] = center[1];
  p[2] = center[2];
  seeds->SetPoint( 0, p );

  p[0] = center[0] + 0.1 * radius;
  seeds->SetPoint( 1, p );

  filter->SetSeeds( seeds );
  filter->Update();

  std::vector< vtkPolyData* > outputs = filter->GetOutputs();

  std::vector< vtkPolyData* >::const_iterator it = outputs.begin();
  size_t i = 0;

  while( it != outputs.end() )
    {
    vtkSmartPointer< vtkPolyDataWriter > writer =
        vtkSmartPointer< vtkPolyDataWriter >::New();
    writer->SetInput( *it );

    std::stringstream os;
    os << i;

    std::string filename = "mesh";
    filename += os.str();
    filename += ".vtk";
    writer->SetFileName( filename.c_str() );
    writer->Write();

    ++i;
    ++it;
    }

  return EXIT_SUCCESS;
}
