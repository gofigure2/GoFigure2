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

#include "QGoOneClickSegmentationDockWidget.h"

#include <QColorDialog>
#include <cstdlib>
#include <iostream>

//---------------------------------------------------------------------------//
QGoOneClickSegmentationDockWidget::
QGoOneClickSegmentationDockWidget( QWidget* iParent) : QDockWidget( iParent ),
m_Filter(0)
{
  this->setupUi( this );

  QObject::connect( this->applyFilterBtn, SIGNAL( pressed() ),
      this, SLOT( ApplyFilterEmit() ) );

  QObject::connect( this->filterType, SIGNAL( activated( int ) ),
        this, SLOT( FilterChanged( int ) ) );

  QObject::connect( this->advanceMode, SIGNAL( toggled( bool ) ),
          this, SLOT( AdvancedMode( bool ) ) );

  // Initialize visualization
  UpdateWidget( true );
  AdvancedMode( false );

  // Initialize parameters for levelset
  InitializeLevelsetParameters();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoOneClickSegmentationDockWidget::
~QGoOneClickSegmentationDockWidget()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
InitializeLevelsetParameters()
{
  this->numberOfIterations->setValue( 50 );
  this->curvatureWeight->setValue( 1 );
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
ApplyFilterEmit()
{
  emit ApplyFilterPressed();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
SetNumberOfChannels( int iNumberOfChannels )
{
  this->channel->setMaximum( iNumberOfChannels );
}

//---------------------------------------------------------------------------//
int
QGoOneClickSegmentationDockWidget::
GetFilter()
{
  // 0 = circle contours (sphere aspect)
  // 1 = sphere ( 3D volume - no contours)
  // 2 = 3D levelset

  return this->filterType->currentIndex();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
double
QGoOneClickSegmentationDockWidget::
GetRadius()
{
  return this->radiusSpinBox->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoOneClickSegmentationDockWidget::
GetChannel()
{
  return this->channel->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoOneClickSegmentationDockWidget::
GetNumberOfIterations()
{
  return this->numberOfIterations->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoOneClickSegmentationDockWidget::
GetCurvatureWeight()
{
  return this->curvatureWeight->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
FilterChanged( int filterSelected )
{
  // 0 = circle contours (sphere aspect)
  // 1 = sphere ( 3D volume - no contours)
  // 2 = 3D levelset

  switch ( filterSelected )
    {
    case 0 :
    // circle contours creation (sphere aspect)
    this->UpdateWidget( true );
    break;

    case 1 :
    // sphere (3D volume creation)
    this->UpdateWidget( true );
    break;

    case 2 :
    // sphere (3d level set)
    this->UpdateWidget( false );
    break;
/*
    case 3 :
    // sphere (3d level set)
    this->UpdateWidget( false );
    break;*/

    default :
    break;
    }
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
UpdateWidget( bool iUse )
{
  this->LevelsetParametersLabel->setVisible( !iUse );

  this->channelLabel->setVisible( !iUse );
  this->channel->setVisible( !iUse );

  this->advanceModeLabel->setVisible( !iUse );
  this->advanceMode->setVisible( !iUse );
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
AdvancedMode( bool iAdvanced )
{
  this->numberOfIterationsLabel->setVisible( iAdvanced );
  this->numberOfIterations->setVisible( iAdvanced );

  this->curvatureWeightLabel->setVisible( iAdvanced );
  this->curvatureWeight->setVisible( iAdvanced );
}
