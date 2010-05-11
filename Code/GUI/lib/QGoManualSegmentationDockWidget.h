/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#ifndef __QGoManualSegmentationDockWidget_h
#define __QGoManualSegmentationDockWidget_h

#include <QDockWidget>
#include "ui_ManualSegmentationDockWidget.h"

class vtkProperty;
class QGoManualSegmentationSettingsDialog;

#include "QGoManualSegmentationSettingsDialog.h"
#include "QGoTraceManualEditingWidget.h"
#include "QGoIOConfigure.h"

class QGOGUILIB_EXPORT QGoManualSegmentationDockWidget :
  public QDockWidget,
  private Ui::ManualSegmentationDockWidget
{
  Q_OBJECT
public:
  QGoManualSegmentationDockWidget( QWidget* parent = 0 );
  ~QGoManualSegmentationDockWidget();

  QGoManualSegmentationSettingsDialog* m_SettingsDialog;

  void SetNumberOfChannels( int );

  // Methods to get useful informations
  int    GetMode();
  double GetRadius();
  int    GetChannel();
  int    GetNumberOfIterations();
  int    GetCurvatureWeight();

private:
  void ShowManual( bool iUse );
  void ShowSemiAuto( bool iUse );

  void InitializeLevelsetParameters();

signals:
  void ReinitializePressed();
  void ValidatePressed();
  void SettingsPressed();
  void UpdateContourRepresentationProperties();
  void ApplyFilterPressed();

  // Change Mode
  void UpdateInteractorBehaviour( bool );

public slots:
  void UpdateWidget( int );
  void AdvancedMode( bool );
  void ApplyFilterEmit();

};

#endif
