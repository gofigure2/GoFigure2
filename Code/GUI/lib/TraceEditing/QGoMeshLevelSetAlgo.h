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
#ifndef __QGoMeshLevelSetAlgo_h
#define __QGoMeshLevelSetAlgo_h

#include "QGoLevelSetAlgo.h"
#include "QGoFilterChanAndVese.h"
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
\class QGoMeshLevelSetAlgo
\brief class to be the interface between the levelset algo for meshes
and GoFigure
*/
class QGoMeshLevelSetAlgo: public QGoLevelSetAlgo
{
public:
  QGoMeshLevelSetAlgo(std::vector< vtkPoints* >* iSeeds, QWidget *iParent = 0);
  ~QGoMeshLevelSetAlgo();

  std::vector<vtkPolyData*> ApplyAlgo(
    GoImageProcessor* iImages,
    std::string iChannel);

protected:

  template < class TPixel >
            // note this will work only in 3D, so we can remove the template
            // parameter on the image dimension
             //unsigned int VImageDimension >
  vtkPolyData * ApplyLevelSetFilter(
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
    // why no call to the filter itself...?
    QGoFilterChanAndVese Filter;
    Filter.Apply3DFilter< PixelType >( ITK_ROI_Image,
          iCenter,
          0, // we dont want to extract ROI from input since we already did
          this->m_Iterations->GetValue(),
          this->m_Curvature->GetValue());

    typename QGoFilterChanAndVese::Output3DPointer
        ItkOutPut = Filter.GetOutput3D();

    // Here it would be better if the mesh extraction would be performed directly
    // in ITK instead.
    vtkImageData * FilterOutPutToVTK =
        this->ConvertITK2VTK<
          typename QGoFilterChanAndVese::OutputPixelType,
          ImageDimension>( ItkOutPut );

    vtkPolyData* temp_output = this->ExtractPolyData(FilterOutPutToVTK, 0);
    FilterOutPutToVTK->Delete();

    double temp_bounds[6];
    temp_output->GetBounds( temp_bounds );

    double temp_center[3];
    temp_center[0] = ( temp_bounds[0] + temp_bounds[1] ) * 0.5;
    temp_center[1] = ( temp_bounds[2] + temp_bounds[3] ) * 0.5;
    temp_center[2] = ( temp_bounds[4] + temp_bounds[5] ) * 0.5;

    vtkSmartPointer< vtkTransform > translation =
        vtkSmartPointer< vtkTransform >::New();

    /// \todo fix it to get the real center!!!
    translation->Translate(iCenter[0] - temp_center[0],
                           iCenter[1] - temp_center[1],
                           iCenter[2] - temp_center[2] );

    vtkSmartPointer< vtkTransformPolyDataFilter > mesh_transform =
        vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    mesh_transform->SetTransform(translation);
    mesh_transform->SetInput( temp_output );
    mesh_transform->Update();
    temp_output->Delete();

    // MIGHT LEAK! CHECK IT  IS DELETED!
    vtkPolyData* mesh = vtkPolyData::New();
    mesh->DeepCopy( mesh_transform->GetOutput() );

    return mesh;
    }
};

#endif
