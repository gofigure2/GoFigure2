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

QGoNavigationDockWidget::
QGoNavigationDockWidget( QWidget *iParent,
                         const GoFigure::TabDimensionType & iDim ) :
  QDockWidget(iParent),
  m_Dimension(iDim)
{
  this->setupUi(this);
  QIcon Navigation;
  Navigation.addPixmap(QPixmap( QString::fromUtf8(":/fig/navigation.png") ),
                       QIcon::Normal, QIcon::Off);
  //temp->setIcon(Navigation);
  this->toggleViewAction()->setIcon(Navigation);

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

      this->line->setVisible(false);

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

  QObject::connect( this->AllChannelsBtn, SIGNAL( toggled(bool) ),
                    this, SIGNAL( ShowAllChannelsChanged(bool) ) );

  QObject::connect( this->ChannelComboBox, SIGNAL( currentIndexChanged(int) ),
                    this, SIGNAL( ShowOneChannelChanged(int) ) );

  this->stepLabel->hide();
  this->step->hide();
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
void
QGoNavigationDockWidget::SetNumberOfChannels(const unsigned int & iN)
{
  if ( iN < 2 )
    {
    this->line->setVisible(false);
    this->AllChannelsBtn->setVisible(false);
    this->OneChannelBtn->setVisible(false);
    this->ChannelComboBox->setVisible(false);
    }
  else
    {
    this->line->setVisible(true);
    this->AllChannelsBtn->setVisible(true);
    this->OneChannelBtn->setVisible(true);
    this->ChannelComboBox->setMaxCount(iN);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::SetChannel(const unsigned int & i, const QString & iText)
{
  QString input;

  if ( !iText.isEmpty() )
    {
    input = iText;
    }
  else
    {
    input = QString("Channel %1").arg(i);
    }
  this->ChannelComboBox->insertItem(i, input);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoNavigationDockWidget::SetCurrentChannel(unsigned int iChannel)
{
  this->ChannelComboBox->setCurrentIndex(iChannel);
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
    //this->MinXSlicelbl->setText(ConvertToString<int>(iMin).c_str());
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
                      this, SLOT( StepVisibility(int) ) );

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
    this->line_2->hide();
    this->TSliceLbl->hide();
    this->TSliceSpinBox->hide();
    this->TSliceSlider->hide();
    this->MinTSlicelbl->hide();
    this->MaxTSlicelbl->hide();
    this->ModeComboBox->hide();
    this->stepLabel->hide();
    this->step->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoNavigationDockWidget::GetCurrentChannel() const
{
  return this->ChannelComboBox->currentIndex();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoNavigationDockWidget::ShowAllChannels() const
{
  return this->AllChannelsBtn->isChecked();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoNavigationDockWidget::GetChannelName(const int & iIdx)
{
  return this->ChannelComboBox->itemText(iIdx);
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
void QGoNavigationDockWidget::StepVisibility(int iStep)
{
  if ( iStep == 0 )
    {
    this->step->hide();
    this->stepLabel->hide();
    }
  else
    {
    this->step->show();
    this->stepLabel->show();
    }
}
