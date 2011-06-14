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

#ifndef __QGoTraceEditingWidget_h
#define __QGoTraceEditingWidget_h

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QComboBox>
#include "QGoModesManagerWidget.h"
#include "QGoAlgorithmsManagerWidget.h"


class QGoTraceEditingWidget:
  public QWidget
{
  Q_OBJECT
public:
  explicit QGoTraceEditingWidget(std::string iTraceName , 
    std::vector<QString> iVectChannels, 
    QStringList iListTimePoints, QWidget *iParent = 0 );
  ~QGoTraceEditingWidget();

  /**
  \brief add iModeWidget as a widget to be displayed when iModeName is selected
  in the combobox of m_ModeEditingWidget 
  \param[in] iModeName name of the mode corresponding to iModeWidget
  \param[in] iModeWidget Widget corresponding to iModeName
  */
  void AddMode( std::string iModeName, QWidget* iModeWidget, bool ModeNeedSeeds);

  void AddMode(QGoAlgorithmsManagerWidget* iAlgoModeWidget, bool ModeNeedSeeds);

  /**
  \brief replace the existing m_ModeEditingWidget with iModeWidget: all the 
  previous mode will be erased
  \param[in] iModeWidget modes manager
  */
  void SetANewModesManager(QGoModesManagerWidget* iModeWidget);

  void AddAlgoWidgetForSemiAutomaticMode(QGoAlgorithmWidget* iAlgoWidget);

  void AddAlgoWidgetForAutomaticMode(QGoAlgorithmWidget* iAlgoWidget);

  void AddWidgetForManualMode(QWidget* iWidget,
    QStringList iListTimePoint, bool ModeNeedSeeds);

  std::string GetChannelNumber();
  int GetSelectedTimePoint();
 
  /**
  \brief check which mode is selected and if the seeds are needed or not and
  emit the corresponding signals based on the visibility of the widget
  */
  void CheckTheCurrentMode(bool IsVisible);

  void SetTSliceForClassicView(int iTimePoint);
  void SetTSliceForDopplerView(
    std::map<QString, QColor> iListTimePoints, int iChannelNumber);

  /**
  \brief return the name of the current mode in the combobox
  */
  std::string GetCurrentModeName();

signals:
  void SetSeedInteractorBehaviour(bool enable);

  void ResetClicked();

protected:
  void Initialize(std::vector<QString> iVectChannels, 
  QStringList iListTimePoints, QWidget *iParent = 0);
  QGoModesManagerWidget* m_ModeEditingWidget;
  QVBoxLayout*           m_VLayout;
  QStringList            m_ListTimePoints;

  void SetModesManager(QGoModesManagerWidget* iModeWidget);

};
#endif
