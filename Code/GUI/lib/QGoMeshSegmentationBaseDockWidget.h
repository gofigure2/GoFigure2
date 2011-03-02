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

#ifndef __QGoMeshSegmentationBaseDockWidget_h
#define __QGoMeshSegmentationBaseDockWidget_h

#include <QDockWidget>
#include <QLayout>

#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"

// base widgets
class QGoMeshSeedSegmentation;

#include "ui_SegmentationBaseDockWidget.h"

/**
 * \class QGoMeshSegmentationBaseDockWidget
 * \ingroup QGoMesh
 * \brief Base dockwidget for the meshes segmentation
*/

class QGoMeshSegmentationBaseDockWidget:
  public QDockWidget,
  protected Ui::SegmentationBaseDockWidget
{
  Q_OBJECT
public:
  explicit QGoMeshSegmentationBaseDockWidget(
    QWidget *iParent = 0,
    vtkPoints *seeds = 0,
    std::vector< vtkSmartPointer<vtkImageData> > *iOriginalImage = 0);

  virtual ~QGoMeshSegmentationBaseDockWidget();

  /**
   * \brief Specify on which one the segmentation will be applied since the input
   * data is multichannel.
   * \param[in] iChannel Channel on which want we want to apply the segmentation
   * algorithm
   * \param[in] iText Name of the channels
   */
  void SetChannel(int iChannel, const QString & iText = QString());

  void SetCurrentChannel(unsigned int iChannel);

  void SetNumberOfChannels(int iNumberOfChannels);

public slots:
/**
 * \brief Slot to know if we are in Manual, Semi Automatic ou Automatic segmentation
 */
  void SegmentationMethod(int);

  /**
   * \brief Slot to update the interactor behavior, depending on if we are
   * in Manual, Semi Automatic ou Automatic segmentation
   */
  void interactorBehavior(bool);

signals:
/**
 * \brief Signal sent when we enter/leave the Manual segmentation mode
 * \param[in] iActivated true: enter, false: leave
 */
  void ManualSegmentationActivated(bool iActivated);

  /**
   * \brief Signal sent when we enter/leave the Semi Automatic segmentation mode
   * \param[in] iActivated true: enter, false: leave
   */
  void SemiAutoSegmentationActivated(bool iActivated);

  /**
   * \brief Signal sent when we enter/leave the Automatic segmentation mode
   * \param[in] iActivated true: enter, false: leave
   */
  void AutoSegmentationActivated(bool iActivated);

  /**
   * \brief Signal sent to go to the default interactor behavior
   */
  void ReinitializeInteractorActivated(bool);

  // manual segmentation specific signals
  /**
   * \brief Signal sent if we want to create a structure mesh composed of
   * multiple contours
   */
  void CreateCorrespondingMesh(int);

  /**
   * \brief Signal sent to add a contour to an mesh created with CreateCorrespondingMesh()
   */
  void AddContourForMeshToContours(vtkPolyData *);

  // semi automatic signals
  /**
   * \brief Signal sent before starting a "seed segmentation" to update the
   * value of the seeds
   */
  void UpdateSeeds();

  /**
   * \brief Signal sent after a seed segmentation to save the contour and
   * update the visualization
   */
  void SaveAndVisuMesh(vtkPolyData *, int);

  /**
   * \brief Signal sent after a seed segmentation to delete the seeds
   */
  void ClearAllSeeds();

private:
  QGoMeshSeedSegmentation *m_MeshManualSegmentation;
  QGoMeshSeedSegmentation *m_MeshSemiAutoSegmentation;

  Q_DISABLE_COPY(QGoMeshSegmentationBaseDockWidget);
};

#endif
