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

#include "QGoSeedBaseWidget.h"

#include <QShortcut>
#include <QDebug>

//---------------------------------------------------------------------------//
QGoSeedBaseWidget::QGoSeedBaseWidget(QWidget *iParent, int iSampling):QWidget(iParent)
{
  this->setupUi(this);

  // Connect signals
  QObject::connect( this->filter, SIGNAL( activated(int) ),
                    this, SIGNAL( Filter(int) ) );

  QObject::connect( this->apply, SIGNAL( pressed() ), this, SIGNAL( Apply() ) );

  QObject::connect( this->radius, SIGNAL( valueChanged(double) ),
                    this, SIGNAL( Radius(double) ) );

  QObject::connect( this->channel, SIGNAL( activated(int) ),
                    this, SIGNAL( Channel(int) ) );

  // shortcut to the "Apply" button
  (void) new QShortcut(QKeySequence(tr("A", "Apply")), this, SIGNAL(Apply()));

  // About the sampling button...
  if ( iSampling == 2 )
    {
    // add the button and connect it
    QLabel *samplingLabel = new QLabel(this);
    samplingLabel->setObjectName( QString::fromUtf8("samplingLabel") );
    samplingLabel->setText(
      QApplication::translate("SegmentationSeedBaseWidget", "Sampling",
                              0, QApplication::UnicodeUTF8) );
    gridLayout->addWidget(samplingLabel, 3, 0, 1, 2);
    // Add the spin box
    QSpinBox *sampling = new QSpinBox(this);
    sampling->setObjectName( QString::fromUtf8("sampling") );
    sampling->setMinimum(1);
    sampling->setValue(3);
    gridLayout->addWidget(sampling, 3, 2, 1, 1);

    QObject::connect( sampling, SIGNAL( valueChanged(int) ),
                      this, SIGNAL( Sampling(int) ) );
    }

  gridLayout->removeWidget(apply);
  gridLayout->addWidget(apply, 5, 0, 1, -1);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoSeedBaseWidget::
~QGoSeedBaseWidget()
{}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoSeedBaseWidget::AddChannel(QString iChannel)
{
 // qDebug() << "AddChannel(" <<iChannel << ")";
  channel->addItem(iChannel);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//

void
QGoSeedBaseWidget::setNumberOfChannels(int iNumberOfChannels)
{
  //qDebug() << "setNumberOfChannels" << iNumberOfChannels;
  channel->clear();
  channel->setMaxCount(iNumberOfChannels);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//

void
QGoSeedBaseWidget::AddFilter(QString iFilter)
{
  filter->addItem(iFilter);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
int
QGoSeedBaseWidget::GetNumberOfFilters()
{
  return filter->count();
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGridLayout *
QGoSeedBaseWidget::GetFrame()
{
  return gridLayout;
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoSeedBaseWidget::Initialize()
{
  filter->setCurrentIndex(0);
  emit Filter(0);
}

//---------------------------------------------------------------------------//
