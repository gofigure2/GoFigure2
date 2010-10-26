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

#include "ui_SegmentationBaseDockWidget.h"

class QGoContourSegmentationBaseDockWidget:
  public QDockWidget,
  protected Ui::SegmentationBaseDockWidget
{
  Q_OBJECT
public:
  explicit QGoContourSegmentationBaseDockWidget(
    QWidget *iParent = 0,
    vtkPoints *seeds = 0,
    std::vector< vtkSmartPointer<vtkImageData> > *iOriginalImage = 0);

  ~QGoContourSegmentationBaseDockWidget();

  /**
   * \brief Specify on which one the segmentation will be applied since the input
   * data is multichannel.
   * \param[in] iChannel Channel on which want we want to apply the segmentation
   * algorithm
   */
  void SetChannel(int iChannel, const QString & iText = QString());

  bool GetReeditMode();

  void SetReeditMode(bool iEnable);

  void Initialize();

/*
protected:
  virtual void InitializeWidgetAppearance();
*/
public slots:
  void SegmentationMethod(int);

  void interactorBehavior(bool);

signals:
  void ManualSegmentationActivated(bool);

  void SemiAutoSegmentationActivated(bool);

  void AutoSegmentationActivated(bool);

  void ReinitializeInteractorActivated(bool);

  // manual segmentation specific signals
  void ValidateContour();

  void ReinitializeContourWidget();

  void UpdateContourRepresentationProperties(float, QColor, QColor, QColor);
  void ShowTraceDockWidgetForContour(bool);

  // semi automatic signals
  void UpdateSeeds();

  void SaveAndVisuContour(vtkPolyData *);

  void ClearAllSeeds();

protected:
  QGoContourManualSegmentation *m_ContourManualSegmentation;
  QGoContourSeedSegmentation *  m_ContourSemiAutoSegmentation;
private:
  Q_DISABLE_COPY(QGoContourSegmentationBaseDockWidget);
};

#endif
