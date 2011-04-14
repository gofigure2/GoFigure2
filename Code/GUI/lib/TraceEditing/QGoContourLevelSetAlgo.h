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
#ifndef __QGoContourLevelSetAlgo_h
#define __QGoContourLevelSetAlgo_h

#include "QGoLevelSetAlgo.h"
#include "QGoFilterChanAndVese.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"


/**
\class QGoContourLevelSetAlgo
\brief class to be the interface between the levelset algo for contours
and GoFigure
*/
class QGoContourLevelSetAlgo: public QGoLevelSetAlgo
{
public:
  QGoContourLevelSetAlgo(std::vector< vtkPoints* >* iSeeds, QWidget *iParent = 0);
  ~QGoContourLevelSetAlgo();

  std::vector<vtkPolyData*> ApplyAlgo(
    std::vector<vtkSmartPointer< vtkImageData > >* iImages,
    int iChannel);

protected:

  template < class PixelType, unsigned int VImageDimension >
  vtkPolyData * ApplyLevelSetFilter(std::vector<double> iCenter,
  std::vector<vtkSmartPointer< vtkImageData > >* iImages,
    int iChannel)
    {
    std::vector<double> Bounds = this->GetBounds(iCenter, this->m_Radius->GetValue());
    vtkImageData* ROI = ExtractROI(Bounds, ( *iImages )[iChannel]);

    typename itk::Image< unsigned char, VImageDimension >::Pointer ItkInput =
      this->ConvertVTK2ITK<unsigned char, VImageDimension>(ROI);

    QGoFilterChanAndVese Filter;
    typename itk::Image< float, VImageDimension >::Pointer ItkOutPut =
      Filter.Apply2DFilter<VImageDimension>(ItkInput,
      this->m_Curvature->GetValue(),  this->m_Iterations->GetValue() );

    vtkImageData * FilterOutPutToVTK = this->ConvertITK2VTK<float, VImageDimension>(ItkOutPut);
    return this->ExtractPolyData(FilterOutPutToVTK, 0);
    }
};

#endif
