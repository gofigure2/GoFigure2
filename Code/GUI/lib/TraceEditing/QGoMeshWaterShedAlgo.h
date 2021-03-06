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
#ifndef __QGoMeshWaterShedAlgo_h
#define __QGoMeshWaterShedAlgo_h

#include "QGoWaterShedAlgo.h"
#include "QGoFilterWatershed.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "GoImageProcessor.h"

/**
\class QGoMeshWaterShedAlgo
\brief class to be the interface between the watershed algo for meshes
and GoFigure
*/
class QGoMeshWaterShedAlgo: public QGoWaterShedAlgo
{
public:
  QGoMeshWaterShedAlgo(std::vector< vtkPoints* >* iSeeds,
                       int iMaxThreshold,
                       QWidget* iParent = 0);
  ~QGoMeshWaterShedAlgo();

  std::vector<vtkPolyData*> ApplyAlgo(
    GoImageProcessor* iImages,
    std::string iChannel,
    bool iIsInvertedOn = false);

protected:

  template < class TPixel >
            // note this will work only in 3D, so we can remove the template
            // parameter on the image dimension
             //unsigned int VImageDimension >
  vtkPolyData * ApplyWaterShedFilter(
    const std::vector<double>& iCenter,
    typename itk::Image< TPixel, 3 >::Pointer iImages)
    {
    assert( iCenter.size() == 3);

    const unsigned int ImageDimension = 3;

    typedef TPixel PixelType;

    typedef itk::Image< PixelType, ImageDimension >  ImageType;
    typedef typename ImageType::Pointer               ImagePointer;

    // let's compute the bounds of the region of interest
    double radius = this->m_Radius->GetValue();

    std::vector< double > bounds( 2 * ImageDimension, 0. );
    unsigned int k = 0;
    for( unsigned int dim = 0; dim < ImageDimension; dim++ )
      {
      bounds[k++] = iCenter[dim] - 2. * radius;
      bounds[k++] = iCenter[dim] + 2. * radius;
      }

    // then let's extract the Region of Interest
    ImagePointer ITK_ROI_Image =
        this->ITKExtractROI< PixelType, ImageDimension >( bounds, iImages );

    // Compute the segmentation in 3D
    QGoFilterWatershed Filter;
    Filter.Apply3DFilter< PixelType >(
          ITK_ROI_Image,
          this->m_ThresMin->GetValue(),
          this->m_ThresMax->GetValue(),
          this->m_CorrThres->GetValue(),
          this->m_Alpha->GetValue(),
          this->m_Beta->GetValue());

    typename QGoFilterWatershed::Output3DPointer
        ItkOutPut = Filter.GetOutput3D();

    // Here it would be better if the mesh extraction would be performed directly
    // in ITK instead.
    vtkImageData * FilterOutPutToVTK =
        this->ConvertITK2VTK<
          typename QGoFilterWatershed::OutputPixelType,
          ImageDimension>( ItkOutPut );

    // Nicolas- should be able to tune the parameter -0.5-
    vtkPolyData* temp_output = this->ExtractPolyData(FilterOutPutToVTK, 0.5);
    FilterOutPutToVTK->Delete();

    // MIGHT LEAK! CHECK IT  IS DELETED!
    vtkPolyData* mesh = vtkPolyData::New();
    mesh->DeepCopy( temp_output );

    return mesh;
    }
};

#endif
