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

#include "QGoNavigationDockWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>

#include <QCheckBox>
#include <QPushButton>

#include <QtDebug>

QGoNavigationDockWidget::
QGoNavigationDockWidget( QWidget *iParent,
                         const GoFigure::TabDimensionType & iDim ) :
  QGoDockWidget(iParent),
  m_Dimension(iDim)
{
  this->setupUi(this);
  QIcon Navigation;
  Navigation.addPixmap(QPixmap( QString::fromUtf8(":/fig/navigation.png") ),
                       QIcon::Normal, QIcon::Off);
  //temp->setIcon(Navigation);
  //this->toggleViewAction()->setIcon(Navigation);
  this->m_ToggleAction->setIcon(Navigation);

  switch ( m_Dimension )
    {
    case GoFigure::TWO_D:
      {
      this->XSliceLbl->setVisible(false);
      this->XSliceSpinBox->setVisible(false);
      this->YSliceLbl->setVisible(false);
      this->YSliceSpinBox->setVisible(false);
      this->ZSliceLbl->setVisible(false);
      this->ZSliceSpinBox->setVisible(false);
      this->TSliceLbl->setVisible(false);
      this->TSliceSpinBox->setVisible(false);

      break;
      }
    case GoFigure::TWO_D_WITH_T:
      {
      this->XSliceLbl->setVisible(false);
      this->XSliceSpinBox->setVisible(false);
      this->YSliceLbl->setVisible(false);
      this->YSliceSpinBox->setVisible(false);
      this->ZSliceLbl->setVisible(false);
      this->ZSliceSpinBox->setVisible(false);

      break;
      }
    case GoFigure::THREE_D:
      {
      this->TSliceLbl->setVisible(false);
      this->TSliceSpinBox->setVisible(false);

      break;
      }
    default:
    case GoFigure::THREE_D_WITH_T:
      {
      break;
      }
    }

  QObject::connect( this->XSliceSpinBox, SIGNAL( valueChanged(int) ),
                    this, SIGNAL( XSliceChanged(int) ) );

  QObject::connect( this->YSliceSpinBox, SIGNAL( valueChanged(int) ),
                    this, SIGNAL( YSliceChanged(int) ) );

  QObject::connect( this->ZSliceSpinBox, SIGNAL( valueChanged(int) ),
                    this, SIGNAL( ZSliceChanged(int) ) );

  QObject::connect( this->TSliceSpinBox, SIGNAL( valueChanged(int) ),
                    this, SIGNAL( TSliceChanged(int) ) );

  // doppler view specific widgets
  this->channelLabel->hide();
  this->channelName->hide();
  this->stepLabel->hide();
  this->step->hide();
  this->tLabel->hide();
  this->t->hide();

}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoNavigationDockWidget::
~QGoNavigationDockWidget()
{
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetXSlice(int iSlice)
{
  this->XSliceSpinBox->setValue(iSlice);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetYSlice(int iSlice)
{
  this->YSliceSpinBox->setValue(iSlice);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetZSlice(int iSlice)
{
  this->ZSliceSpinBox->setValue(iSlice);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetTSlice(int iSlice)
{
  this->TSliceSpinBox->setValue(iSlice);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetXMinimumAndMaximum(const int & iMin, const int & iMax)
{
  if( iMin != iMax )
    {
    this->XSliceSpinBox->setMinimum(iMin);
    this->XSliceSpinBox->setMaximum(iMax);
    this->XSliceSlider->setMinimum(iMin);
    this->XSliceSlider->setMaximum(iMax);
    this->MinXSlicelbl->setText( tr("%1").arg(iMin) );
    this->MaxXSlicelbl->setText( tr("%1").arg(iMax) );
    }
  else
    {
    this->YSliceLbl->hide();
    this->YSliceSpinBox->hide();
    this->YSliceSlider->hide();
    this->MinYSlicelbl->hide();
    this->MaxYSlicelbl->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetYMinimumAndMaximum(const int & iMin, const int & iMax)
{
  if( iMin != iMax )
    {
    this->YSliceSpinBox->setMinimum(iMin);
    this->YSliceSpinBox->setMaximum(iMax);
    this->YSliceSlider->setMinimum (iMin);
    this->YSliceSlider->setMaximum(iMax);
    this->MinYSlicelbl->setText( tr("%1").arg(iMin) );
    this->MaxYSlicelbl->setText( tr("%1").arg(iMax) );
    }
  else
    {
    this->YSliceLbl->hide();
    this->YSliceSpinBox->hide();
    this->YSliceSlider->hide();
    this->MinYSlicelbl->hide();
    this->MaxYSlicelbl->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetZMinimumAndMaximum(const int & iMin, const int & iMax)
{
  if( iMin != iMax )
    {
    this->ZSliceSpinBox->setMinimum(iMin);
    this->ZSliceSpinBox->setMaximum(iMax);
    this->ZSliceSlider->setMinimum (iMin);
    this->ZSliceSlider->setMaximum(iMax);
    this->MinZSlicelbl->setText( tr("%1").arg(iMin) );
    this->MaxZSlicelbl->setText( tr("%1").arg(iMax) );
    }
  else
    {
    this->ZSliceLbl->hide();
    this->ZSliceSpinBox->hide();
    this->ZSliceSlider->hide();
    this->MinZSlicelbl->hide();
    this->MaxZSlicelbl->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::SetTMinimumAndMaximum(const int & iMin, const int & iMax)
{
  if( iMin != iMax )
    {
    this->TSliceSpinBox->setMinimum(iMin);
    this->TSliceSpinBox->setMaximum(iMax);
    this->TSliceSlider->setMinimum (iMin);
    this->TSliceSlider->setMaximum(iMax);
    this->MinTSlicelbl->setText( tr("%1").arg(iMin) );
    this->MaxTSlicelbl->setText( tr("%1").arg(iMax) );

    QObject::connect( this->ModeComboBox, SIGNAL( activated(int) ),
                      this, SIGNAL( ModeChanged(int) ) );

    QObject::connect( this->ModeComboBox, SIGNAL( activated(int) ),
                      this, SLOT( UpdateWidget(int) ) );

    QObject::connect( this->step, SIGNAL( valueChanged(int) ),
                      this, SIGNAL( StepChanged(int) ) );


    // shortcuts to move through time
    (void)new QShortcut( QKeySequence( tr("Ctrl+Z", "Move to previous") ),
                        this, SLOT( MoveToPreviousTimePoint() ) );
    (void)new QShortcut( QKeySequence( tr("Ctrl+C", "Move to next") ),
                        this, SLOT( MoveToNextTimePoint() ) );
    }
  else
    {
    this->TSliceLbl->hide();
    this->TSliceSpinBox->hide();
    this->TSliceSlider->hide();
    this->MinTSlicelbl->hide();
    this->MaxTSlicelbl->hide();
    this->ModeComboBox->hide();
    this->channelLabel->hide();
    this->channelName->hide();
    this->stepLabel->hide();
    this->step->hide();
    this->tLabel->hide();
    this->t->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::MoveToPreviousTimePoint()
{
  if ( TSliceSpinBox->value() > TSliceSpinBox->minimum() )
    {
    emit TSliceChanged(TSliceSpinBox->value() - 1);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::MoveToNextTimePoint()
{
  if ( TSliceSpinBox->value() < TSliceSpinBox->maximum() )
    {
    emit TSliceChanged(TSliceSpinBox->value() + 1);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNavigationDockWidget::UpdateWidget(int iStep)
{
  if ( iStep == 0 )
    {
    this->channelLabel->hide();
    this->channelName->hide();
    this->stepLabel->hide();
    this->step->hide();
    this->tLabel->hide();
    this->t->hide();
    }
  else
    {
    this->channelLabel->show();
    this->channelName->show();
    this->stepLabel->show();
    this->step->show();
    this->tLabel->show();
    this->t->show();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::
AddChannel(const QString& iName, const QColor& iColor, const unsigned int& iNumber,
           const bool& iChecked)
{
  // create check box + colored push button
  QCheckBox *checkBox1 = new QCheckBox(iName, this);
  checkBox1->setObjectName(iName);
  checkBox1->setChecked(iChecked);
  QPushButton *pushButton = new QPushButton(this);
  pushButton->setObjectName(iName);
  QString style = "background: rgb(%1, %2, %3);";
  pushButton->setStyleSheet(
        style.arg(iColor.red()).arg(iColor .green()).arg(iColor.blue()));
 // pushButton->setStyleSheet("QPushButton { background: red } ");
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(checkBox1);
  layout->addWidget(pushButton);
  // to be modified - 7
  this->gridLayout_2->addLayout(layout, 7+iNumber, 0, 0);
  //create signals connections
  QObject::connect( checkBox1, SIGNAL( clicked(bool) ),
                    this, SLOT( visibilityChanged(bool) ) );
  // more signals for modify LUT

  // vector of widget so we can remove it from layout efficiently
  m_ListChannels.push_back(checkBox1);
  m_ListChannels.push_back(pushButton);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::
visibilityChanged(bool iVisibility)
{
  emit visibilityChanged(QObject::sender()->objectName(), iVisibility);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::
VisibilityListChannels(const bool& iVisibility)
{
  QList<QWidget*>::iterator it = m_ListChannels.begin();
  while(it != m_ListChannels.end())
  {
    (*it)->setVisible(iVisibility);
    ++it;
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::
AddDoppler(const QString& iName, const QColor& iColor, const unsigned int& iNumber,
           const bool& iChecked)
{
  // create check box + colored push button
  QCheckBox *checkBox1 = new QCheckBox(iName, this);
  checkBox1->setObjectName(iName);
  checkBox1->setChecked(iChecked);
  QPushButton *pushButton = new QPushButton(this);
  pushButton->setObjectName(iName);
  QString style = "background: rgb(%1, %2, %3);";
  pushButton->setStyleSheet(
        style.arg(iColor.red()).arg(iColor .green()).arg(iColor.blue()));
 // pushButton->setStyleSheet("QPushButton { background: red } ");
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(checkBox1);
  layout->addWidget(pushButton);
  // to be modified - 7
  this->gridLayout_2->addLayout(layout, 7+iNumber, 0, 0);
  //create signals connections
  QObject::connect( checkBox1, SIGNAL( clicked(bool) ),
                    this, SLOT( visibilityChanged(bool) ) );
  // more signals for modify LUT

  // vector of widget so we can remove it from layout efficiently
  m_ListDoppler.push_back(checkBox1);
  m_ListDoppler.push_back(pushButton);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::
VisibilityListDoppler(const bool& iVisibility)
{
  QList<QWidget*>::iterator it = m_ListDoppler.begin();
  while(it != m_ListDoppler.end())
  {
    (*it)->setVisible(iVisibility);
    ++it;
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::
setChannelName(QString iChannelName)
{
  this->channelName->setText(iChannelName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget:: DeleteDopplerWidgets()
{
  qDeleteAll(m_ListDoppler.begin(), m_ListDoppler.end());
  m_ListDoppler.clear();
}
