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
#include "QGoLineageDockWidget.h"

#include "ctkDoubleRangeSlider.h"

#include <iostream>

//-------------------------------------------------------------------------
QGoLineageDockWidget::QGoLineageDockWidget(
  QWidget *iParent) : QDockWidget(iParent)
{
  this->setupUi(this);
  QIcon Lineageicon;
  Lineageicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/LineageView.png") ),
                      QIcon::Normal, QIcon::Off);
  this->toggleViewAction()->setIcon(Lineageicon);
  this->toggleViewAction()->setToolTip("Lineage View");
  this->setWindowTitle("Lineage View");
  // appearance
  QObject::connect( this->glyph, SIGNAL( toggled(bool) ),
                    this, SLOT( Glyphs(bool) ) );
  QObject::connect( this->glyphSpinBox, SIGNAL( valueChanged(double) ),
                    this, SLOT( glyphValueChanged(double) ) );

  QObject::connect( this->tube, SIGNAL( toggled(bool) ),
                    this, SLOT( Tubes(bool) ) );
  QObject::connect( this->tubeSpinBox, SIGNAL( valueChanged(double) ),
                    this, SLOT( tubeValueChanged(double) ) );

  // color code
  QObject::connect( this->time, SIGNAL( toggled(bool) ),
                    this, SLOT( ColorCodeLineagesByTime(bool) ) );
  QObject::connect( this->speed, SIGNAL( toggled(bool) ),
                    this, SLOT( ColorCodeLineagesBySpeed(bool) ) );
  QObject::connect( this->real, SIGNAL( toggled(bool) ),
                    this, SLOT( ColorCodeLineagesByOriginalColor(bool) ) );

  /*
  // double slider
  ctkDoubleRangeSlider *rangeSlider =
      new ctkDoubleRangeSlider(Qt::Horizontal , this->dockWidgetContents);
  gridLayout->addWidget(rangeSlider, 2, 0, 1, 1);
  */
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoLineageDockWidget::
~QGoLineageDockWidget()
{
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::glyphValueChanged(double)
{
  //to avoid useless update
  if ( this->glyph->isChecked() )
    {
    Glyphs(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::Glyphs(bool iActivated)
{
  if ( iActivated )
    {
    emit UpdateLineagesRepresentation( this->glyphSpinBox->value(), this->tubeSpinBox->value() * this->tube->isChecked() );
    }
  else
    {
    emit UpdateLineagesRepresentation( 0, this->tubeSpinBox->value() * this->tube->isChecked() );
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::tubeValueChanged(double)
{
  //to avoid useless update
  if ( this->tube->isChecked() )
    {
    Tubes(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::Tubes(bool iActivated)
{
  if ( iActivated )
    {
    emit UpdateLineagesRepresentation( this->glyphSpinBox->value() * this->glyph->isChecked(), this->tubeSpinBox->value() );
    }
  else
    {
    emit UpdateLineagesRepresentation(this->glyphSpinBox->value() * this->glyph->isChecked(), 0);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::ColorCodeLineagesByTime(bool iChecked)
{
  if ( iChecked )
    {
    emit ChangeColorCode("TemporalInformation");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::ColorCodeLineagesBySpeed(bool iChecked)
{
  if ( iChecked )
    {
    emit ChangeColorCode("SpeedInformation");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoLineageDockWidget::ColorCodeLineagesByOriginalColor(bool iChecked)
{
  if ( iChecked )
    {
    emit ChangeColorCode("Original");
    }
}

//-------------------------------------------------------------------------
