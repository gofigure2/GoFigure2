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
#ifndef __QGoSegmentationAlgo_h
#define __QGoSegmentationAlgo_h

#include "QGoAlgorithmWidget.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "QGoGUILibConfigure.h"

/**
\class QGoSegmentationAlgo
\brief abstract class to be the interface between the algorithms for meshes 
and contours and GoFigure
*/
class QGOGUILIB_EXPORT QGoSegmentationAlgo:public QObject
{
  Q_OBJECT
public:
  QGoSegmentationAlgo(QWidget *iParent = 0);
  ~QGoSegmentationAlgo();

  /**
  \brief return the algowidget
  */
  QGoAlgorithmWidget* GetAlgoWidget();

  /**
  \brief return the vtkpolydata created by the algorithm
  */
  virtual std::vector<vtkPolyData*> ApplyAlgo(
    //vtkPoints* iSeeds, std::vector<vtkSmartPointer< vtkImageData > >* iImages,
    std::vector<vtkSmartPointer< vtkImageData > >* iImages,
    int iChannel) = 0;

protected:
  QGoAlgorithmWidget*             m_AlgoWidget;

  /**
  \brief construct the algowidget with the different parameters
  */
  virtual void SetAlgoWidget(QWidget* iParent = 0) = 0;

  /**
  \brief delete the different parameters
  */
  virtual void DeleteParameters() = 0;

  /*
   * \brief Extract region of interest, given a bounding box and a list of vtk images
   * \param[in] iBounds bounding box (xmin, xmax, ymin, ymax, zmin, zmax)
   * \param[in] iImages vector of vtkimagedata
   * \return list of roi
   */
  std::vector<vtkImageData*> ExtractROI(double* iBounds, std::vector<vtkImageData*> iImages);
  /*
   * \brief Extract region of interest, given a bounding box and a vtk image
   * \param[in] iBounds bounding box (xmin, xmax, ymin, ymax, zmin, zmax)
   * \param[in] iImage vtkimagedata
   * \return roi
   */
  vtkImageData* ExtractROI(double* iBounds, vtkImageData* iImage);

  // convert vtk to itk
  //template< class PixelType, unsigned int VImageDimension >
  //typename itk::Image< PixelType, VImageDimension >::Pointer
  //ConvertVTK2ITK(vtkImageData *iInput);

  //add a method std::vector<PolyData*> ConvertITKImagesToPolyData(std::vector<itk::Image> iImages)
  //add a method std::vector<TraceAttribut> GetAttribut(std::vector<vtkPolyData*> iNewTraces)
};

#endif
