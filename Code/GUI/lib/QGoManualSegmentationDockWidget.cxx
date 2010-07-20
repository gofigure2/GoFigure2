/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#include "QGoManualSegmentationDockWidget.h"

#include <QColorDialog>
#include <cstdlib>

#include "QGoManualSegmentationSettingsDialog.h"

//---------------------------------------------------------------------------//
QGoManualSegmentationDockWidget::
QGoManualSegmentationDockWidget(QWidget* iParent) :
  QDockWidget(iParent)
  {
  this->setupUi(this);

  m_SettingsDialog = new QGoManualSegmentationSettingsDialog(this);

  QObject::connect(this->contourMode, SIGNAL(activated(int)),
                   this, SLOT(UpdateWidget(int)));

  QObject::connect(this->ReinitializeBtn, SIGNAL(pressed()),
                   this, SIGNAL(ReinitializePressed()));

  QObject::connect(this->ValidateBtn, SIGNAL(pressed()),
                   this, SIGNAL(ValidatePressed()));

  QObject::connect(this->SettingsBtn, SIGNAL(pressed()),
                   m_SettingsDialog, SLOT (exec()));

  QObject::connect(m_SettingsDialog, SIGNAL(accepted()),
                   this, SIGNAL(UpdateContourRepresentationProperties()));

  QObject::connect(this->applyFilterBtn, SIGNAL(pressed()),
                   this, SLOT(ApplyFilterEmit()));

  QObject::connect(this->advanceMode, SIGNAL(toggled(bool)),
                   this, SLOT(AdvancedMode(bool)));

  // Initialize visualization
  // 0 = manual segmentation
  // 1 = semi auto segmentation
  // 3 = auto
  UpdateWidget(0);
  AdvancedMode(false);

  // Initialize levelset parameters
  InitializeLevelsetParameters();
  }
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoManualSegmentationDockWidget::
~QGoManualSegmentationDockWidget()
  {
  }
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
InitializeLevelsetParameters()
{
  this->numberOfIterations->setValue(50);
  this->curvatureWeight->setValue(1);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
UpdateWidget(int iModeSelected)
{
  // 0 = manual segmentation
  // 1 = semi automatic segmentation
  // 2 = auto

  switch (iModeSelected)
    {
    case 0:
      // manual segmentation
      this->ShowManual(true);
      this->ShowSemiAuto(false);
      break;

    case 1:
      // semi automatic segmentation
      this->ShowManual(false);
      this->ShowSemiAuto(true);
      break;

    default:
      break;
    }
  emit this->UpdateInteractorBehavior(true);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
ShowManual(bool iUse)
{
  this->ValidateBtn->setVisible(iUse);
  this->ReinitializeBtn->setVisible(iUse);
  this->SettingsBtn->setVisible(iUse);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
ShowSemiAuto(bool iUse)
{
  this->LevelsetParametersLabel->setVisible(iUse);

  this->channelLabel->setVisible(iUse);
  this->channel->setVisible(iUse);

  this->radiusLabel->setVisible(iUse);
  this->radius->setVisible(iUse);

  this->advanceModeLabel->setVisible(iUse);
  this->advanceMode->setVisible(iUse);

  this->applyFilterBtn->setVisible(iUse);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
AdvancedMode(bool iAdvanced)
{
  this->numberOfIterationsLabel->setVisible(iAdvanced);
  this->numberOfIterations->setVisible(iAdvanced);

  this->curvatureWeightLabel->setVisible(iAdvanced);
  this->curvatureWeight->setVisible(iAdvanced);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
ApplyFilterEmit()
{
  emit ApplyFilterPressed();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoManualSegmentationDockWidget::
SetNumberOfChannels(int iNumberOfChannels)
{
  this->channel->setMaximum(iNumberOfChannels);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
double
QGoManualSegmentationDockWidget::
GetRadius()
{
  return this->radius->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoManualSegmentationDockWidget::
GetChannel()
{
  return this->channel->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoManualSegmentationDockWidget::
GetNumberOfIterations()
{
  return this->numberOfIterations->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoManualSegmentationDockWidget::
GetCurvatureWeight()
{
  return this->curvatureWeight->value();
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
int
QGoManualSegmentationDockWidget::
GetMode()
{
  return this->contourMode->currentIndex();
}
//---------------------------------------------------------------------------//
