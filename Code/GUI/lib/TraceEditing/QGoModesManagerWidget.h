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

#ifndef __QGoModesManagerWidget_h
#define __QGoModesManagerWidget_h

#include <QWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include "QGoAlgorithmsManagerWidget.h"

/**
 * \class QGoModesManagerWidget
 * \ingroup GUI
 * \brief widget that manages the different modes for the TraceEditingWidget,
 has a combobox with the mode names and a corresponging stackedWidgets which
 display the right widget according to the mode selected in the combobox,
 has semiautomatic and automatic default modes
*/
class QGoModesManagerWidget:
  public QWidget
{
  Q_OBJECT
public:
  explicit QGoModesManagerWidget(std::vector<QString> iVectChannels, 
    QStringList iListTimePoints, QWidget *iParent = 0);
  ~QGoModesManagerWidget();

  /**
  \brief add a widget in the StackedWidget with the mode name that will
  be added in the combobox.
  \param[in] iModeName name of the mode
  \param[in] iWidget widget that will be displayed when iModeName is selected
  \param[in] ModeNeedSeeds if true, a signal will be emitted everytime the mode name appear
  in the combobox
  */
  void AddWidgetWithModeName (std::string iModeName, QWidget* iWidget, 
    bool ModeNeedSeeds);

  /**
  \brief add a QGoAlgorithmsManagerWidget and set the default index of this algo widget
  to iDefaultIndex
  \param[in] iAlgoManagerWidget
  \param[in] iDefaultIndex default index for the algo widget
  */
  void AddAlgoManagerWidget(QGoAlgorithmsManagerWidget* iAlgoManagerWidget, 
    bool ModeNeedSeeds, int iDefaultIndex = 0);

  /**
  \brief add the iAlgoWidget directly in the QgoAlgomanager corresponding
  to the Semi Automatic mode
  \param[in] iAlgoWidget widget to be added for the parameters of the algo
  */
  void AddAlgoWidgetForSemiAutomaticMode(QGoAlgorithmWidget* iAlgoWidget);

  /**
  \brief add the iAlgoWidget directly in the QgoAlgomanager corresponding
  to the Automatic mode
  \param[in] iAlgoWidget widget to be added for the parameters of the algo
  */
  void AddAlgoWidgetForAutomaticMode(QGoAlgorithmWidget* iAlgoWidget);

  /**
  \brief create the "manual" mode and makes it correspond to the provided
  widget
  \param[in] iWidget widget to be added for the manual mode
  */
  void AddWidgetForManualMode(QWidget* iWidget, bool ModeNeedSeeds);

  /**
  \brief return the number of the selected channel
  */
  int GetChannelNumber();
  int GetSelectedTimePoint();

  void SetTSliceForClassicViewInAllAlgoModes(int iTimePoint);
  void SetTSliceForDopplerViewInAllAlgoModes(QStringList iListTimePoint, int iChannelNumber);

  /**
  \brief return the mode name currently selected in the combobox
  */
  std::string GetCurrentModeName();

public slots:
   /**
  \brief set the right mode according to the combobox if iIndex is different than
  -1 and emit a signal to enable/disable the seeds interactor mode based on the
  name of the selected mode
  */
  void SetTheRightMode(int iIndex = -1);

signals:
  void SetSeedInteractorBehaviour(bool enable);
  void ResetClicked();

protected:
  QVBoxLayout*                m_VBoxLayout;
  QComboBox*                  m_ModeComboBox;
  QStackedWidget*             m_ModeWidgets;  
  QGoAlgorithmsManagerWidget* m_SemiAutoAlgoManagerWidget;
  QGoAlgorithmsManagerWidget* m_AutoAlgoManagerWidget;
  bool                        m_ModeAlreadyCleaned;
  QStringList                 m_ModesWhoNeedSeeds;

  void Initialize(std::vector<QString> iVectChannels, QStringList iListTimePoints);

  /**
  \brief check that there is something in the default modes, if not, remove the mode
  name from the combobox
  */
  void CheckDefaultModes();

 
};
#endif
