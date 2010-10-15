/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 2037 $  // Revision of last commit
  Date: $Date: 2010-08-23 16:33:20 -0400 (Mon, 23 Aug 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#ifndef __QGoSeedSegmentation_h
#define __QGoSeedSegmentation_h

#include <QObject>
#include <QWidget>
#include "QGoGUILibConfigure.h"

#include "QGoSeedSegmentationBase.h"

class vtkPolyData;
class vtkPoints;

class vtkImageData;

class QGoBaseAlgorithmSegmentationDockWidget;

// segmentation filters
class QGoFilterChanAndVes;
class QGoFilterShape;

/**
 * \class QGoSeedShapeSegmentation
 * \ingroup QGoSeed
 * \brief Class to create pre defined meshes
*/
class QGOGUILIB_EXPORT QGoSeedSegmentation:public QGoSeedSegmentationBase
{
  Q_OBJECT
public:
  /**
   * \brief Constructor
   */
  explicit QGoSeedSegmentation(QObject *parentO = 0, QWidget *parentW = 0,
                               vtkPoints *seeds = 0,
                               std::vector< vtkImageData * > *iOriginalImage = 0);

  /**
   * \brief Destructor
   */
  ~QGoSeedSegmentation();

  /**
   * \brief Connect signals/slots to the segmentation dock widget
   * \param[in] iDockWidget Algorithm segmentation specific dock widget
   */
  void ConnectSignals(QGoBaseAlgorithmSegmentationDockWidget *iDockWidget);

  /**
   * \brief Specify on which one the segmentation will be applied since the input
   * data is multichannel.
   * \param[in] iChannel Channel on which want we want to apply the segmentation
   * algorithm
   */
  void SetChannel(int iChannel);

  // DOCKWIDGET
  virtual QWidget * getDockWidget();

public slots:
signals:
  /**
   * \brief Signal to be send to the dockwidget to add channel to the channel
   * QComboBox
   * \param[in] iQString Name of the channel.
   */
  void addChannel(QString);

  void MeshCreated(vtkPolyData *);

  void ContourCreated(vtkPolyData *);

  void ImageProcessed();

  void UpdateSeeds();

  void SegmentationFinished();

private:
  /**
   * \brief Reconstruct a 3D mesh from a vtkImageData using MarchingCubes.
   * Returns a vtkPolyData* representing the mesh.
   * \param[in] iOriginalImage Input 3D image resulted from segmentation.
   */
  vtkPolyData *  ReconstructMesh(vtkImageData *iInputImage);

  QGoBaseAlgorithmSegmentationDockWidget *m_BaseAlgorithmSegmentationDockWidget;

  QGoFilterChanAndVes *m_LevelSet2Dfilter;
  QGoFilterShape *     m_ShapeFilter;

  std::vector< vtkImageData * > *m_OriginalImage;
};
#endif
