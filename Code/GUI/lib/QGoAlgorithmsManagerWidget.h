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

#ifndef __QGoAlgorithmsManagerWidget_h
#define __QGoAlgorithmsManagerWidget_h

#include <QWidget>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QFormLayout.h>
#include <QComboBox>
#include "QGoAlgorithmWidget.h"

/**
 * \class QGoAlgorithmsManagerWidget
 * \ingroup GUI manages all the algorithms widget for a same result,
 has a combobox with the name of the methods which display a different
 QGoAlgorithmWidget depending on the method selected in the combobox
 * \brief 
*/
class QGoAlgorithmsManagerWidget:
  public QWidget
{
  Q_OBJECT
public:
  explicit QGoAlgorithmsManagerWidget(std::string iModeName, 
    QStringList iListChannels, QStringList iListTime, QWidget *iParent = 0);
  ~QGoAlgorithmsManagerWidget();

  /**
  \brief add a widget in the stacked layout with the name
  of the method associated in the combobox to display it,
  including the parameters and the advanced parameters
  displayed in an expandable box
  */
  void AddMethod(QGoAlgorithmWidget* iAlgoWidget);

  /**
  \brief set the current index in the combobox to iIndex and 
  get the corresponding widget to display
  \param[in] iIndex index to be displayed as the current one
  */
  void SetCurrentIndex(int iIndex);

  void SetCurrentTimePoint(QString iTimePoint);

  void SetCurrentChannel(QString iChannel);

  std::string GetModeName();

signals:

  void ApplyClicked();
  void ResetClicked();

protected:
  QVBoxLayout*                 m_VBoxLayout;
  QComboBox*                   m_MethodComboBox;
  QStackedWidget*              m_MethodWidgets; 
  std::string                  m_ModeName;
  QComboBox*                   m_ChannelComboBox;
  QComboBox*                   m_TimeComboBox;
  //QStackedLayout*              m_MethodsLayout;

  /**
  \brief add the different widgets, buttons and fill the comboboxes 
  for channel and timepoint
  */
  void Initialize(QStringList iListChannels, QStringList iListTime);
  
};
#endif
