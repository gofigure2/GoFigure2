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
#ifndef __QGoFilterSemiAutoBase_h
#define __QGoFilterSemiAutoBase_h

#include <QObject>

// QT
#include <QString>
#include <QWidget>

// VTK to ITK
#include "vtkImageExport.h"
#include "itkVTKImageImport.h"
#include "itkImage.h"

#include "vtkSmartPointer.h"

// for the signals
#include "vtkPolyData.h"

class vtkImageData;
class vtkPoints;
class vtkImageExport;
#include "QGoGUILibConfigure.h"

/**
 * \class QGoFilterSemiAutoBase Connects the common signals regarding the seeds segmentation
 * Provides methods to convert images from itk to vtk.
 * Provides methods to convert images from vtk to itk.
 * Provides methods to extract ROI from your image and to enhance your meshes.
 */

class QGOGUILIB_EXPORT QGoFilterSemiAutoBase:public QObject
{
  Q_OBJECT
public:
  /** \brief Constructor */
  explicit QGoFilterSemiAutoBase(QObject *iParent = NULL);

  /** \brief Destructor */
  virtual ~QGoFilterSemiAutoBase();

  void          setName(QString iName);

  QString       getName();

  void          setWidget(QWidget *iWidget);

  QWidget *      getWidget();

  vtkSmartPointer<vtkImageData> getInput();

  void          setOutput(vtkImageData *iOutput);

  vtkImageData * getOutput();

  void          setCenter(double *iCenter);

  double *       getCenter();

  double        getRadius();

  int           getSampling();

  int           getChannel();

  void          setPoints(vtkPoints *iPoints);

  vtkPoints *    getPoints();

  void          setOriginalImageMC(std::vector< vtkSmartPointer<vtkImageData> > *iOriginalImage);

  vtkImageData * extractOneSlice(vtkImageData *iOriginalImage, double *iOrigin, int iDirection);

  vtkPolyData *  ReconstructContour(vtkImageData *iInputImage,
                                    const double & iThreshold);

  vtkPolyData *  ReorganizeContour(vtkPolyData *iInputImage = NULL,
                                   bool iDecimate = false);

  vtkPolyData *  ReconstructMesh(vtkImageData *iInputImage,
                                 const double & iThreshold);

  // connect signals to dockwidget
  virtual void   ConnectSignals(int iFilterNumber);

  //CONVERT VTK 2 ITK
  template< class PixelType, unsigned int VImageDimension >
  typename itk::Image< PixelType, VImageDimension >::Pointer
  ConvertVTK2ITK(vtkImageData *iInput);

  //CONVERT ITK 2 VTK
  template< class PixelType, unsigned int VImageDimension >
  vtkImageData *
  ConvertITK2VTK(typename itk::Image< PixelType, VImageDimension >::Pointer);

  //EXTRACT ROI
  template< class PixelType, unsigned int VImageDimension >
  typename itk::Image< PixelType, VImageDimension >::Pointer
  ExtractROI(typename itk::Image< PixelType, VImageDimension >::Pointer,
             double *iCenter, double iRadius);

  int m_Dimension;
public slots:

  virtual vtkPolyData * Apply() = 0;

  void    UpdateVisibility(int iFilter);

  void    setRadius(double iRadius);

  void    setChannel(int iChannel = 0);

  void    setSampling(int iSampling);

  void UpdateAdvancedMode(bool);

signals:
  void MeshCreated(vtkPolyData *, int timePoint = 0);

  void ContourCreated(vtkPolyData *);

  void ImageProcessed();

  void UpdateSeeds();

  void SegmentationFinished();

  void CreateCorrespondingMesh(int);

  void AddContourForMeshToContours(vtkPolyData *);

private:
  vtkImageExport *                              m_vtk2itkImage;
  vtkImageData *                                m_Output;
  QString                                       m_Name;
  QWidget *                                     m_Widget;
  double                                        m_Center[3];
  double                                        m_Radius;
  int                                           m_Number;
  int                                           m_Channel;
  vtkPoints *                                   m_Points;
  std::vector< vtkSmartPointer<vtkImageData> > *m_OriginalImageMC;
  int                                           m_Sampling;
};

#include "QGoFilterSemiAutoBase.txx"

#endif
