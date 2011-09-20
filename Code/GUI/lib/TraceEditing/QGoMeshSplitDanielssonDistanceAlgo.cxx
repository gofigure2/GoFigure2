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
#include "QGoMeshSplitDanielssonDistanceAlgo.h"

#include "itkvtkMeshSplitterDanielssonDistanceImageFilter.h"
#include "GoImageProcessor.h"

QGoMeshSplitDanielssonDistanceAlgo::QGoMeshSplitDanielssonDistanceAlgo(std::vector< vtkPoints* >* iSeeds, QWidget* iParent)
    :QGoSplitDanielssonDistanceAlgo(iSeeds, iParent)
{

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshSplitDanielssonDistanceAlgo::~QGoMeshSplitDanielssonDistanceAlgo()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoMeshSplitDanielssonDistanceAlgo::ApplyAlgo(
    GoImageProcessor* iImages,
    std::string iChannel,
    vtkPolyData* iPolyData,
    bool iIsInvertedOn)
{
  size_t nb_ch = iImages->getNumberOfChannels();

  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::vtkMeshSplitterDanielssonDistanceImageFilter< ImageType >
      SplitterType;
  SplitterType::Pointer filter = SplitterType::New();
  filter->SetNumberOfImages( nb_ch );


  filter->SetMesh( iPolyData );

  for( size_t i = 0; i < nb_ch; i++ )
    {
    filter->SetFeatureImage( i,
                             iImages->getImageITK<PixelType, Dimension>(
                                 iImages->getChannelName(i)));
    }

  typedef SplitterType::PointSetType PointSetType;
  PointSetType::Pointer seeds = PointSetType::New();

  ImageType::PointType itk_pt;
  double vtk_pt[3];

  for( vtkIdType id = 0; id < this->m_Seeds->size(); id++ )
    {
    for ( int i = 0; i < (*this->m_Seeds)[id]->GetNumberOfPoints(); i++ )
      {
      (*this->m_Seeds)[id]->GetPoint( i, vtk_pt );
      itk_pt[0] = vtk_pt[0];
      itk_pt[1] = vtk_pt[1];
      itk_pt[2] = vtk_pt[2];
      seeds->SetPoint( i, itk_pt );
      }
    }

  filter->SetSeeds( seeds );
  filter->Update();

  return filter->GetOutputs();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
