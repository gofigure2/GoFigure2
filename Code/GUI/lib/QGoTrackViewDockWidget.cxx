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
#include "QGoTrackViewDockWidget.h"

#include "ctkDoubleRangeSlider.h"

#include <iostream>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QLabel>

//-------------------------------------------------------------------------
QGoTrackViewDockWidget::QGoTrackViewDockWidget(
  QWidget *iParent) : QDockWidget(iParent)
{
  this->SetUpUi();
  QIcon trackicon;
  trackicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/TrackView.png") ),
                      QIcon::Normal, QIcon::Off);
  this->toggleViewAction()->setIcon(trackicon);
  this->toggleViewAction()->setToolTip("Track View");
  this->setWindowTitle("Track View");
  // appearance
  QObject::connect( this->m_glyph, SIGNAL( toggled(bool) ),
                    this, SLOT( Glyphs(bool) ) );
  QObject::connect( this->m_glyphSpinBox, SIGNAL( valueChanged(double) ),
                    this, SLOT( glyphValueChanged(double) ) );

  QObject::connect( this->m_tube, SIGNAL( toggled(bool) ),
                    this, SLOT( Tubes(bool) ) );
  QObject::connect( this->m_tubeSpinBox, SIGNAL( valueChanged(double) ),
                    this, SLOT( tubeValueChanged(double) ) );

  // color code
  QObject::connect( this->m_time, SIGNAL( toggled(bool) ),
                    this, SLOT( ColorCodeTracksByTime(bool) ) );
  QObject::connect( this->m_speed, SIGNAL( toggled(bool) ),
                    this, SLOT( ColorCodeTracksBySpeed(bool) ) );
  QObject::connect( this->m_real, SIGNAL( toggled(bool) ),
                    this, SLOT( ColorCodeTracksByOriginalColor(bool) ) );

  /*
  // double slider
  ctkDoubleRangeSlider *rangeSlider =
      new ctkDoubleRangeSlider(Qt::Horizontal , this->dockWidgetContents);
  gridLayout->addWidget(rangeSlider, 2, 0, 1, 1);
  */
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTrackViewDockWidget::
~QGoTrackViewDockWidget()
{
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::glyphValueChanged(double)
{
  //to avoid useless update
  if ( this->m_glyph->isChecked() )
    {
    Glyphs(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::Glyphs(bool iActivated)
{
  if ( iActivated )
    {
    emit UpdateTracksRepresentation( this->m_glyphSpinBox->value(), this->m_tubeSpinBox->value() * this->m_tube->isChecked() );
    }
  else
    {
    emit UpdateTracksRepresentation( 0, this->m_tubeSpinBox->value() * this->m_tube->isChecked() );
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::tubeValueChanged(double)
{
  //to avoid useless update
  if ( this->m_tube->isChecked() )
    {
    Tubes(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::Tubes(bool iActivated)
{
  if ( iActivated )
    {
    emit UpdateTracksRepresentation( this->m_glyphSpinBox->value() * this->m_glyph->isChecked(), this->m_tubeSpinBox->value() );
    }
  else
    {
    emit UpdateTracksRepresentation(this->m_glyphSpinBox->value() * this->m_glyph->isChecked(), 0);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::ColorCodeTracksByTime(bool iChecked)
{
  if ( iChecked )
    {
    emit ChangeColorCode("TemporalInformation");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::ColorCodeTracksBySpeed(bool iChecked)
{
  if ( iChecked )
    {
    emit ChangeColorCode("SpeedInformation");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::ColorCodeTracksByOriginalColor(bool iChecked)
{
  if ( iChecked )
    {
    emit ChangeColorCode("Original");
    }
}

//-------------------------------------------------------------------------
void QGoTrackViewDockWidget::SetUpUi()
{
  QWidget* TrackViewWidget = new QWidget;

  this->m_glyph = new QCheckBox(tr("Glyphs"), this );
  this->m_tube = new QCheckBox(tr ("Tubes"), this );
  this->m_glyphSpinBox = new QDoubleSpinBox(this);
  this->m_tubeSpinBox = new QDoubleSpinBox(this);
  this->SetDoubleSpinBox(this->m_glyphSpinBox);
  this->SetDoubleSpinBox(this->m_tubeSpinBox);

  QVBoxLayout* Vlayout = new QVBoxLayout;
  QLabel* Element = new QLabel(tr("Element:"), this );
  QLabel* Radius = new QLabel (tr("Radius:"), this );

  QGridLayout* GridLayout = new QGridLayout;
  GridLayout->addWidget(Element, 0, 0);
  GridLayout->addWidget(Radius, 0, 1);
  GridLayout->addWidget(this->m_glyph, 1, 0);
  GridLayout->addWidget(this->m_glyphSpinBox, 1, 1);
  GridLayout->addWidget(this->m_tube, 2, 0);
  GridLayout->addWidget(this->m_tubeSpinBox, 2, 1);
  
  Vlayout->addLayout(GridLayout);
  QLabel* Blank = new QLabel(tr("  ") );
  Vlayout->addWidget(Blank);

  this->m_real = new QRadioButton(tr("Original Color") );
  this->m_time = new QRadioButton(tr("Time Color Code") );
  this->m_speed = new QRadioButton(tr("Speed Color Code") );
  this->m_real->setChecked(true);

  QVBoxLayout* VColorCodeLayout = new QVBoxLayout;
  VColorCodeLayout->addWidget(this->m_real);
  VColorCodeLayout->addWidget(this->m_time);
  VColorCodeLayout->addWidget(this->m_speed);
  Vlayout->addLayout(VColorCodeLayout, 1);

  TrackViewWidget->setLayout(Vlayout);
  this->setWidget(TrackViewWidget);
  TrackViewWidget->setSizePolicy(
    QSizePolicy::Expanding, QSizePolicy::Fixed);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackViewDockWidget::SetDoubleSpinBox(QDoubleSpinBox* iSpinBox)
{
  iSpinBox->setDecimals(2);
  iSpinBox->setMaximum(99);
  iSpinBox->setMinimum(0);
  iSpinBox->setValue(3);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------