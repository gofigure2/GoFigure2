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

#ifndef __QGoContourSegmentationBaseDockWidget_h
#define __QGoContourSegmentationBaseDockWidget_h

#include <QDockWidget>
#include <QLayout>

#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"

// base widgets
class QGoContourManualSegmentation;
class QGoContourSeedSegmentation;

#include "ui_SegmentationDockWidgetBase.h"

/**
 * \class QGoContourSegmentationBaseDockWidget
 * \ingroup QGoContour
 * \brief Base dockwidget for the contours segmentation
*/

class QGoContourSegmentationBaseDockWidget:
  public QDockWidget,
  protected Ui::SegmentationDockWidgetBase
{
  Q_OBJECT
public:
  explicit QGoContourSegmentationBaseDockWidget(
    QWidget *iParent = 0,
    vtkPoints *seeds = 0,
    std::vector< vtkSmartPointer<vtkImageData> > *iOriginalImage = 0);

  ~QGoContourSegmentationBaseDockWidget();

  /**
   * \brief Specify on which one the segmentation will be applied
   * \param[in] iChannel Channel on which want we want to apply the segmentation
   * algorithm
   * \param[in] iText Name of the channel (not used yet)
   */
  void SetChannel(int iChannel, const QString & iText = QString());

  void SetCurrentChannel(unsigned int iChannel);

  void SetNumberOfChannels(int iNumberOfChannels);

  /**
   * \brief Get reedit mode. Provides a way to know if the reedit mode in on or of
   * \return true: reedit mode ON, false: reedit mode OFF
   */
  bool GetReeditMode();

  /**
   * \brief Set reedit mode. Provides a way to set the reedit mode to on or of
   * \param[in] iEnable true: reedit mode ON, false: reedit mode OFF
   */
  void SetReeditMode(bool iEnable);

  /**
   * \brief Initialize the dockwidget
   */
  void Initialize();

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
   * \brief Signal sent when a contour has been validated in manual segmentation
   */
  void ValidateContour();

  /**
   * \brief Signal sent when a contour has been reinitialized in manual segmentation
   */
  void ReinitializeContourWidget();

  /**
   * \brief Signal sent to update the contour properties in the visualization
   */
  void UpdateContourRepresentationProperties(float, QColor, QColor, QColor);

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
  void SaveAndVisuContour(vtkPolyData *);

  /**
   * \brief Signal sent after a seed segmentation to delete the seeds
   */
  void ClearAllSeeds();

protected:
  QGoContourManualSegmentation *m_ContourManualSegmentation;
  QGoContourSeedSegmentation *  m_ContourSemiAutoSegmentation;
private:
  Q_DISABLE_COPY(QGoContourSegmentationBaseDockWidget);
};

#endif
