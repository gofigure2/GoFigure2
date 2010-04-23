/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 1203 $  // Revision of last commit
  Date: $Date: 2010-04-09 10:03:07 -0400 (Fri, 09 Apr 2010) $  // Date of last commit
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

#include "QGoOneClickSegmentationDockWidget.h"

#include <QColorDialog>
#include <cstdlib>

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

  this->UseRadius( true );
  this->UseLevelSet( false );
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
ApplyFilterEmit()
{
  emit ApplyFilterPressed();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoOneClickSegmentationDockWidget::
GetFilter()
{
  // 0 = circle contours (sphere aspect)
  // 1 = sphere ( 3D volume - no contours)
  return this->filterType->currentIndex();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
double
QGoOneClickSegmentationDockWidget::
GetRadius()
{
  // 0 = circle contours (sphere aspect)
  // 1 = sphere ( 3D volume - no contours)
  return this->radiusSpinBox->value();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
FilterChanged( int filterSelected )
{
  // 0 = circle contours (sphere aspect)
  // 1 = sphere ( 3D volume - no contours)

  switch ( filterSelected )
    {
    case 0 :
    // circle contours creation (sphere aspect)
    this->UseRadius( true );
    this->UseLevelSet( false );
    break;

    case 1 :
    // sphere (3D volume creation)
    this->UseRadius( true );
    this->UseLevelSet( false );
    break;

    case 2 :
    // sphere (2d level set)
    this->UseRadius( false );
    this->UseLevelSet( true );
    break;

    case 3 :
    // sphere (3d level set)
    this->UseRadius( false );
    this->UseLevelSet( true );
    break;

    default :
    break;
    }
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
UseRadius( bool iUse )
{
  if( iUse )
    {
    this->radiusSpinBox->show();
    this->radiusLabel->show();

    }
  else
    {
    this->radiusSpinBox->hide();
    this->radiusLabel->hide();
    }
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoOneClickSegmentationDockWidget::
UseLevelSet( bool iUse )
{
  if( iUse )
    {
    this->label_3->show();
    this->label_4->show();
    this->label_5->show();
    this->doubleSpinBox->show();
    this->doubleSpinBox_2->show();
    this->doubleSpinBox_3->show();
    }
  else
    {
    this->label_3->hide();
    this->label_4->hide();
    this->label_5->hide();
    this->doubleSpinBox->hide();
    this->doubleSpinBox_2->hide();
    this->doubleSpinBox_3->hide();
    }
}
//---------------------------------------------------------------------------//
