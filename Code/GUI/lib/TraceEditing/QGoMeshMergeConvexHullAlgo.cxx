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
#include "QGoMeshMergeConvexHullAlgo.h"

#include "GoImageProcessor.h"

#include "itkImage.h"
#include "itkvtkMeshMergeConvexHullFilter.h"

QGoMeshMergeConvexHullAlgo::QGoMeshMergeConvexHullAlgo(std::vector< vtkPoints* >* iSeeds, QWidget* iParent)
    :QGoMergeConvexHullAlgo(iSeeds, iParent)
{
  this->setObjectName("Merge");
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshMergeConvexHullAlgo::~QGoMeshMergeConvexHullAlgo()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkPolyData*> QGoMeshMergeConvexHullAlgo::ApplyAlgo(
    GoImageProcessor* iImages,
    std::string iChannel,
    std::vector<vtkPolyData*> iPolyData,
    bool iIsInvertedOn)
{
  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef std::list< vtkPolyData* > PolyDataListType;
  PolyDataListType listOfPolydatas;

  // init bounding box
  std::vector< double > bounds(Dimension*2);
  for(int i = 0; i<Dimension; ++i)
    {
    bounds[i*2] = std::numeric_limits<int>::max();
    bounds[i*2+1] = std::numeric_limits<int>::min();
    }

  // get bounding box over all selected polydatas
  std::vector<vtkPolyData*>::iterator iterator = iPolyData.begin();
  while(iterator != iPolyData.end())
    {
    double* boundsPointer = (*iterator)->GetBounds();
    for(int i = 0; i<Dimension; ++i)
      {
      if(boundsPointer[i*2] < bounds[i*2])
          bounds[i*2] = static_cast<int>(boundsPointer[i*2]);
      if(boundsPointer[i*2 + 1] > bounds[i*2 +1])
          bounds[i*2+1] = static_cast<int>(boundsPointer[i*2+1]);
      }
    listOfPolydatas.push_back(*iterator);
    ++iterator;
  }

  ImageType::Pointer ITK_Full_Image = iImages->getImageITK<PixelType, Dimension>(
           iImages->getChannelName(0));

  itk::Vector<double> spacing = ITK_Full_Image->GetSpacing();

  // work on smaller region
  // increase size of bounding box by 20*spacing... bug itk?
  for(int i = 0; i<Dimension; ++i)
    {
    bounds[i*2] = bounds[i*2] - 10*spacing[i];
    bounds[i*2+1] = bounds[i*2+1] + 10*spacing[i];
    }

  // then let's extract the Region of Interest
  // on 1 image as for now - channel 0
  ImageType::Pointer ITK_ROI_Image =
      this->ITKExtractROI< PixelType, Dimension >( bounds, ITK_Full_Image);

  typedef itk::vtkMeshMergeConvexHullFilter< ImageType, PolyDataListType > MergerType;
  MergerType::Pointer filter = MergerType::New();
  filter->SetInputs( listOfPolydatas );
  filter->SetNumberOfImages( 1 );
  filter->SetFeatureImage( 0, ITK_ROI_Image );
  filter->Update();

  std::vector<vtkPolyData*> oVector;
  oVector.push_back(filter->GetOutput());
  return oVector;
}
//-------------------------------------------------------------------------
