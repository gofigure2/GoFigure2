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
#ifndef __QGoSetOfContoursWaterShedAlgo_h
#define __QGoSetOfContoursWaterShedAlgo_h

// external files
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

// project files
#include "QGoWaterShedAlgo.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include "QGoGUILibConfigure.h"
#include "QGoFilterWatershed.h"

// temp for debug purpose
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "itkImageFileWriter.h"


#include "vtkPointData.h"

class GoImageProcessor;


/**
\class QGoSetOfContoursWaterShedAlgo
\brief class to be the interface between the watershed algo for set of contours
and GoFigure
*/
class QGoSetOfContoursWaterShedAlgo: public QGoWaterShedAlgo
{
public:
  QGoSetOfContoursWaterShedAlgo(std::vector< vtkPoints* >* iSeeds, QWidget* iParent = 0);
  ~QGoSetOfContoursWaterShedAlgo();

  // should not be virutal pure since we dont implement it....
  std::vector<vtkPolyData*> ApplyAlgo(
    GoImageProcessor* iImages,
    int iChannel, bool iIsInvertedOn = false);

  std::vector<std::vector<vtkPolyData*> > ApplyAlgoSeveralSeeds(
    GoImageProcessor* iImages, int iChannel);

protected:

  QGoAlgoParameter<int>*          m_Sampling;

  template < class TPixel >
            // note this will work only in 3D, so we can remove the template
            // parameter on the image dimension
             //unsigned int VImageDimension >
  std::vector<vtkPolyData *> ApplyWaterShedFilter(
    const std::vector<double>& iCenter,
    typename itk::Image< TPixel, 3 >::Pointer iImages,
    const unsigned int& iOrientation)
    {
    assert( iCenter.size() == 3);

    const unsigned int ImageDimension = 3;

    typedef TPixel                                  PixelType;
    typedef itk::Image< PixelType, ImageDimension > ImageType;
    typedef typename ImageType::Pointer             ImagePointer;
    typedef itk::Image< PixelType, 2 >              ImageType2D;
    typedef typename ImageType2D::Pointer           ImageType2DPointer;
    typedef typename ImageType::SpacingType         ImageSpacingType;

    std::vector<vtkPolyData*> output;

    ImageSpacingType spacing = iImages->GetSpacing();

    for(unsigned int i= 0; i<this->m_Sampling->GetValue(); ++i)
      {
      // let's compute the bounds of the region of interest
      double radius = this->m_Radius->GetValue();

      std::vector< double > bounds( 2 * ImageDimension, 0. );
      unsigned int k = 0;
      for( unsigned int dim = 0; dim < ImageDimension; dim++ )
        {
        bounds[k++] = iCenter[dim] - 2. * radius;
        bounds[k++] = iCenter[dim] + 2. * radius;
        }

      int pair = i+ i%2 -1;
      if(pair < 0)
        {
        pair = 0;
        }

      bounds[2*iOrientation]    =
          iCenter[iOrientation] + (pair*(pow(-1., static_cast<int>(i) ) )*spacing[iOrientation]);
      bounds[2*iOrientation +1] =
          iCenter[iOrientation] + (pair*(pow(-1., static_cast<int>(i) ) )*spacing[iOrientation]);

      // then let's extract the Slice of Interest
      ImageType2DPointer ITK_Slice_Image =
     this->ITKExtractSlice<PixelType>( bounds, iImages );

      // Compute the segmentation in 3D
      QGoFilterWatershed Filter;
      Filter.Apply2DFilter< PixelType >(
            ITK_Slice_Image,
            this->m_ThresMin->GetValue(),
            this->m_ThresMax->GetValue(),
            this->m_CorrThres->GetValue(),
            this->m_Alpha->GetValue(),
            this->m_Beta->GetValue());

      typename QGoFilterWatershed::Output2DPointer
          ItkOutPut = Filter.GetOutput2D();

      // Here it would be better if the mesh extraction would be performed directly
      // in ITK instead.
      vtkImageData * FilterOutPutToVTK =
          this->ConvertITK2VTK<
            typename QGoFilterWatershed::OutputPixelType,
            2>( ItkOutPut );

      // Nicolas- should be able to tune the parameter -0.5-
      vtkPolyData* temp_output = this->ExtractPolyData(FilterOutPutToVTK, 0.5);

      // translation transform -----------------------
      //------------------------------------------------------------------------
      double temp_bounds[6];
      FilterOutPutToVTK->GetBounds( temp_bounds );

      double temp_center[3];
      temp_center[0] = ( temp_bounds[0] + temp_bounds[1] ) * 0.5;
      temp_center[1] = ( temp_bounds[2] + temp_bounds[3] ) * 0.5;
      temp_center[2] = ( temp_bounds[4] + temp_bounds[5] ) * 0.5;

      vtkSmartPointer< vtkTransform > translation2 =
          vtkSmartPointer< vtkTransform >::New();

       translation2->Translate(-temp_center[0],
                               -temp_center[1],
                               -temp_center[2]);

       vtkSmartPointer< vtkTransformPolyDataFilter > mesh_transform2 =
           vtkSmartPointer< vtkTransformPolyDataFilter >::New();
       mesh_transform2->SetTransform(translation2);
       mesh_transform2->SetInput( temp_output );
       mesh_transform2->Update();

      // rotation transform -----------------------
      //------------------------------------------------------------------------
      vtkSmartPointer< vtkTransform > translation =
          vtkSmartPointer< vtkTransform >::New();
      // rotate polydata if necessary
      if(iOrientation == 0)
        {
        translation->RotateY(-90); // check if + or - 90
        }
      else if(iOrientation == 1)
        {
        translation->RotateX(-90); // check if + or - 90
        }
      else if(iOrientation == 2)
        {
        // no rotation, we are in the good plan
        }

      vtkSmartPointer< vtkTransformPolyDataFilter > mesh_transform =
          vtkSmartPointer< vtkTransformPolyDataFilter >::New();
      mesh_transform->SetTransform(translation);
      mesh_transform->SetInput( mesh_transform2->GetOutput() );
      mesh_transform->Update();

      // translate back -----------------------
      //------------------------------------------------------------------------
      double temp_center2[3];
      temp_center2[0] = ( bounds[0] + bounds[1] ) * 0.5;
      temp_center2[1] = ( bounds[2] + bounds[3] ) * 0.5;
      temp_center2[2] = ( bounds[4] + bounds[5] ) * 0.5;

      vtkSmartPointer< vtkTransform > translation23 =
          vtkSmartPointer< vtkTransform >::New();

       translation23->Translate(temp_center2[0],
                                temp_center2[1],
                                temp_center2[2]);

       vtkSmartPointer< vtkTransformPolyDataFilter > mesh_transform23 =
           vtkSmartPointer< vtkTransformPolyDataFilter >::New();
       mesh_transform23->SetTransform(translation23);
       mesh_transform23->SetInput( mesh_transform->GetOutput() );
       mesh_transform23->Update();

       //-----------------------------------------------------------------------

       temp_output->Delete();

      vtkPolyData* testt = vtkPolyData::New();
      testt->DeepCopy(mesh_transform23->GetOutput());

      output.push_back(testt);
      }

    return output;
    }
};

#endif
