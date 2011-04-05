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

#ifndef __QGoContourManualSegmentationWidget_h
#define __QGoContourManualSegmentationWidget_h

#include <QWidget>
#include "ui_ContourManualSegmentationWidget.h"

class vtkProperty;
class QGoManualSegmentationSettingsDialog;

#include "QGoManualSegmentationSettingsDialog.h"
#include "QGoTraceSettingsWidget.h"
#include "QGoIOConfigure.h"

/**
 * \class QGoContourManualSegmentationWidget
 * \ingroup QGoContourManual
 * \brief Define the manual segmentation widget connections
*/

class QGOGUILIB_EXPORT QGoContourManualSegmentationWidget:
  public QWidget,
  private Ui::ContourManualSegmentationWidget
{
  Q_OBJECT
public:
  explicit QGoContourManualSegmentationWidget(QWidget *parent = 0);
  ~QGoContourManualSegmentationWidget();

  /*
   * \brief Pop up dialog to choose the contour properties
   */
  QGoManualSegmentationSettingsDialog *m_SettingsDialog;

  virtual void setVisible(bool iVisible);

signals:
/*
 * \brief Signal sent if "Reinitialize" button clicked
 */
  void ReinitializePressed();

  /*
   * \brief Signal sent if "Validate" button clicked
   */
  void ValidatePressed();

  /*
   * \brief Signal sent if we "Setting" button clicked
   */
  void SettingsPressed();

  /*
   * \brief Signal sent if leave the m_SettingsDialog by clicking "OK"
   */
  void UpdateContourRepresentationProperties();

  void ManualSegmentationActivated(bool);

private:
  Q_DISABLE_COPY(QGoContourManualSegmentationWidget);
};

#endif
