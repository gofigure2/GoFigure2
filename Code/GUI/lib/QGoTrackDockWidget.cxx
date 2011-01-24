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
#include "QGoTrackDockWidget.h"

#include "ctkDoubleRangeSlider.h"

//-------------------------------------------------------------------------
QGoTrackDockWidget::QGoTrackDockWidget(
  QWidget *iParent):QDockWidget(iParent)
{
  this->setupUi(this);
  QIcon trackicon;
  trackicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/BlankIcon.png") ),
                                QIcon::Normal, QIcon::Off);
  this->toggleViewAction()->setIcon(trackicon);

  // appearance
  QObject::connect( this->glyph, SIGNAL( toggled(bool)),
      this, SLOT( Glyphs(bool) ) );
  QObject::connect( this->glyphSpinBox, SIGNAL( valueChanged( int )),
      this, SLOT( glyphValueChanged( int ) ) );

  QObject::connect( this->tube, SIGNAL( toggled(bool)),
      this, SLOT( Tubes(bool) ) );
  QObject::connect( this->tubeSpinBox, SIGNAL( valueChanged( int )),
      this, SLOT( tubeValueChanged( int ) ) );

  // color code
  QObject::connect( this->time, SIGNAL( toggled(bool)),
      this, SIGNAL( ColorCodeTracksByTime(bool) ) );
  QObject::connect( this->speed, SIGNAL( toggled(bool)),
      this, SIGNAL( ColorCodeTracksBySpeed(bool) ) );
  QObject::connect( this->real, SIGNAL( toggled(bool)),
      this, SIGNAL( ColorCodeTracksByOriginalColor(bool) ) );


  /*
  // double slider
  ctkDoubleRangeSlider *rangeSlider =
      new ctkDoubleRangeSlider(Qt::Horizontal , this->dockWidgetContents);
  gridLayout->addWidget(rangeSlider, 2, 0, 1, 1);
  */
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTrackDockWidget::
~QGoTrackDockWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackDockWidget::
glyphValueChanged( int )
{
  //to avoid useless update
  if( this->glyph->isChecked() )
    {
    Glyphs( true );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackDockWidget::
Glyphs( bool iActivated )
{
  if( iActivated)
    {
    emit UpdateTrackGlyphs( this->glyphSpinBox->value() );
    }
  else
    {
    emit UpdateTrackGlyphs( 0 );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackDockWidget::
tubeValueChanged( int )
{
  //to avoid useless update
  if( this->tube->isChecked() )
    {
    Tubes( true );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackDockWidget::
Tubes( bool iActivated )
{
  if( iActivated)
    {
    emit UpdateTrackTubes( this->tubeSpinBox->value() );
    }
  else
    {
    emit UpdateTrackTubes( 0 );
    }
}
//-------------------------------------------------------------------------

/*
//-------------------------------------------------------------------------
//BUGGY AND USELESS SO COMMENTED
//-------------------------------------------------------------------------
void
QGoTrackDockWidget::
GlyphChanged( bool iState )
{
  //emit UpdateTracksAppearance( iState, this->tube->isChecked() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackDockWidget::
TubeChanged( bool iState )
{
  //emit UpdateTracksAppearance( this->glyph->isChecked(), iState );
}
//-------------------------------------------------------------------------
 */
