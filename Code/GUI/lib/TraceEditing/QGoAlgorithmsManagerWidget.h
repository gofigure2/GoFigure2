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
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QHash>
#include "QGoAlgorithmWidget.h"

/**
 * \class QGoAlgorithmsManagerWidget
 * \ingroup GUI
 * \brief manages all the algorithms widget for a same mode,
 has a combobox with the name of the methods which display a different
 QGoAlgorithmWidget depending on the method selected in the combobox
*/
class QGoAlgorithmsManagerWidget:
  public QWidget
{
  Q_OBJECT
public:
  explicit QGoAlgorithmsManagerWidget(std::string iModeName,
    QWidget *iParent = 0,
    std::vector<QString> iVectChannels = std::vector<QString>(),
    QStringList iListTime = QStringList(),
    bool iOnlyOneMethod = false,
    bool NeedApplyResetButton = true);
  ~QGoAlgorithmsManagerWidget();

  /**
  \brief add a widget in the stacked layout with the name
  of the method associated in the combobox to display it,
  including the parameters and the advanced parameters
  displayed in an expandable box
  \param[in] iAlgoWidget widget with all the parameters for the
  algorithm
  */
  void AddMethod(QGoAlgorithmWidget* iAlgoWidget);

  /**
  \brief add the widget in the stacked_widgets and hide the methodcombobox
  as there will be only one method in this algomanagerwidget
  */
  void AddWidgetForOnlyOneMethod(QWidget* iWidget);

  /**
  \brief set the current index in the combobox to iIndex and
  get the corresponding widget to display
  \param[in] iIndex index to be displayed as the current one
  */
  void SetCurrentIndex(int iIndex);

  void SetCurrentChannel(QString iChannel);


  void SetTSliceForClassicView(QString iTimePoint);

  void SetTSliceForDopplerView(
    QHash<QString, QColor> iListTimePoints, int iIndexChannel);

  /**
  \brief return the name of the mode
  \return the name of the mode
  */
  std::string GetModeName();

  /**
  \brief return true if it has at least one QGoAlgorithmWidget in
  the stackedWidgets
  \return false if there is no algorithm
  */
  bool HasMethod();

  /**
  \brief return the number of the selected channel
  */
  std::string GetCurrentImageName();

  int GetSelectedTimePoint();

  bool IsInvertChecked();

signals:

  void ResetClicked();
  void InvertChecked(Qt::CheckState);

protected:
  QVBoxLayout*                 m_VBoxLayout;
  QComboBox*                   m_MethodComboBox;
  QStackedWidget*              m_MethodWidgets;
  std::string                  m_ModeName;
  QComboBox*                   m_ChannelComboBox;
  QComboBox*                   m_TimeComboBox;
  QStringList                  m_ListTimePoints;
  QLabel*                      m_MethodLabel;
  QCheckBox*                   m_InvertBox;

  /**
  \brief add the different widgets, buttons and fill the comboboxes
  for channel and timepoint
  \param[in] iListChannels list of the names of the channels
  \param[in] iListTime list of the timepoints
  */
  void Initialize(std::vector<QString> iVectChannels = std::vector<QString>(),
    QStringList iListTime = QStringList(), bool iOnlyOneMethod = false,
    bool NeedApplyResetButton = true);

 protected slots:
  /**
  \brief after button clicked signal is emitted, get the current widget
  of the stackedwidgets and call the method in QGoAlgoWidget to make
  it emit a signal
  */
  void EmitApplyAlgo();
};
#endif
