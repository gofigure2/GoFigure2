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
#ifndef __QGoTraceEditingWidgetManager_h
#define __QGoTraceEditingWidgetManager_h

#include "QGoTraceEditingWidget.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include <QAction>
#include <QDockWidget>


/**
\class QGoTraceEditingWidgetManager abstract class handles the interactions
between the user and the algorithms for one kind of trace
\brief
*/
class QGOGUILIB_EXPORT QGoTraceEditingWidgetManager: public QObject
{
  Q_OBJECT
public:
  QGoTraceEditingWidgetManager(std::string iTraceName,
    std::vector<QString> iVectChannels,
    int iTimeMin, int iTimeMax,
    std::vector< vtkPoints* >* iSeeds,
    std::vector< vtkSmartPointer< vtkImageData > >* iImages,
    int* iCurrentTimePoint,
    QWidget* iParent=0);

  ~QGoTraceEditingWidgetManager();

  QAction* GetToggleViewAction();

  /**
  \return the dockwidget for it to be integrated in the mainwindow
  */
  QDockWidget* GetDockWidget();

  /**
  \brief display only the current timepoint in the TSlice comboboxes
  of the qgoalgomanagerwidgets, disable them and enable the channel comboboxes
  */
  virtual void SetTSliceForClassicView();

   /**
  \brief display the 3 timepoints chosen by the user in the TSlice comboboxes
  of the qgoalgomanagerwidgets, enable them, display only the channel tracked
  by the user and disable the channel comboboxes
  */
  virtual void SetTSliceForDopplerView(
    std::map<QString, QColor> iListTimePoints, int iChannelNumber);

public slots:
  /**
  \brief show or hide the dockwidget and check the current selected mode in order
  to emit a signal to get the seeds widget if show or to disable it of hide
  */
  void SetVisible(bool isVisible);

signals:

  void UpdateSeeds();
  void ClearAllSeeds();
  /**
  \brief emitted when new meshes need to be saved in database and rendered in the
  vizu, return the TSlice selected in the TSlice combobox
  */
  void TracesCreatedFromAlgo(std::vector<vtkPolyData *> iVectPolydata, int iTCoord);

protected:
  QDockWidget*                m_TraceEditingDockWidget;
  QGoTraceEditingWidget*      m_TraceEditingWidget;

  std::string                 m_TraceName;
  std::vector< vtkPoints* >*                      m_Seeds; //useful ???
  std::vector< vtkSmartPointer< vtkImageData > >* m_Images;
  int*                                            m_CurrentTimePoint;
  QStringList                 m_ListTimePoint;

  void SetTheTraceWidget(std::vector<QString> iVectChannels, int iTimeMin,
    int iTimeMax, QWidget* iParent);

  void SetTheDockWidget(QWidget* iParent);

  /**
  \brief return the selected timepoint in the TSlice combobox
  */
  int GetSelectedTimePoint();

  /**
  \brief add the algowidget of the different algo in the algomanagerwidget
  for the semi automatic mode and set the different SIGNAL/SLOTS connections
  */
  virtual void SetSemiAutomaticAlgorithms(QWidget* iParent = 0) = 0;

  /**
  \brief get the vtkpolydata for the new created meshes by the chosen algo
  */
  template<typename T>
  void GetPolydatasFromAlgo(T* iAlgo)
    {
    emit UpdateSeeds();
    std::vector<vtkPolyData*> NewTraces = iAlgo->ApplyAlgo(
      this->m_Images, this->m_TraceEditingWidget->GetChannelNumber() );
    emit TracesCreatedFromAlgo(NewTraces, this->GetSelectedTimePoint() );
    emit ClearAllSeeds();
    }

signals:
  /**
  \brief emit true to get the seeds widget enabled and false to disable it
  */
  void SetSeedInteractorBehaviour(bool enable);

protected slots:

};

#endif
