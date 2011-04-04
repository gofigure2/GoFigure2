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
#ifndef __QGoMeshEditingWidgetManager_h
#define __QGoMeshEditingWidgetManager_h

#include "QGoTraceEditingWidget.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "QGoMeshLevelSetAlgo.h"
#include "QGoMeshShapeAlgo.h"
#include "QGoMeshWaterShedAlgo.h"
#include "QGoSetOfContoursWaterShedAlgo.h"
#include "QGoSetOfContoursLevelSetAlgo.h"
#include "QGoSetOfContoursShapeAlgo.h"
#include "QGoMeshSplitDanielssonDistanceAlgo.h"
#include <QAction>
#include <QDockWidget>


/**
\class QGoMeshEditingWidgetManager handles the interactions between the user
and the algorithms for the meshes
\brief
*/
class QGOGUILIB_EXPORT QGoMeshEditingWidgetManager: public QObject
{
  Q_OBJECT
public:
  QGoMeshEditingWidgetManager(std::vector<QString> iVectChannels, 
    int iTimeMin, int iTimeMax, vtkPoints* iSeeds, 
    std::vector< vtkSmartPointer< vtkImageData > >* iImages, 
    int* iCurrentTimePoint,
    QWidget* iParent=0);
  ~QGoMeshEditingWidgetManager();

  QAction* GetToggleViewAction();

  /**
  \return the dockwidget for it to be integrated in the mainwindow
  */
  QDockWidget* GetDockWidget();

  /**
  \brief display only the current timepoint in the TSlice comboboxes
  of the qgoalgomanagerwidgets, disable them and enable the channel comboboxes
  */
  void SetTSliceForClassicView();

   /**
  \brief display the 3 timepoints chosen by the user in the TSlice comboboxes
  of the qgoalgomanagerwidgets, enable them, display only the channel tracked
  by the user and disable the channel comboboxes
  */
  void SetTSliceForDopplerView(QStringList iListTimePoints, int iChannelNumber);

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
  void MeshesCreatedFromAlgo(std::vector<vtkPolyData *> iVectPolydata, int iTCoord);
  void SetOfContoursFromAlgo(std::vector<std::vector<vtkPolyData*> > iVectVectPolydata, int iTCoord);

protected:
  QDockWidget*                m_MeshEditingDockWidget;
  QGoTraceEditingWidget*      m_MeshEditingWidget;
  QGoAlgorithmsManagerWidget* m_SetOfContoursWidget;
  vtkPoints*                                      m_Seeds;
  std::vector< vtkSmartPointer< vtkImageData > >* m_Images;
  int*                                            m_CurrentTimePoint;

  QGoMeshLevelSetAlgo*                            m_LevelSetAlgo;
  QGoMeshShapeAlgo*                               m_ShapeAlgo;
  QGoMeshWaterShedAlgo*                           m_WaterShedAlgo;
  QGoMeshSplitDanielssonDistanceAlgo*             m_DanielAlgo;

  QGoSetOfContoursWaterShedAlgo*                  m_SetOfContoursWaterShedAlgo;
  QGoSetOfContoursLevelSetAlgo*                   m_SetOfContoursLevelSetAlgo;
  QGoSetOfContoursShapeAlgo*                      m_SetOfContoursShapeAlgo;

  void SetTheMeshWidget(std::vector<QString> iVectChannels, int iTimeMin, 
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
  void SetSemiAutomaticAlgorithms(QWidget* iParent = 0);

  void SetSetOfContoursAlgorithms(
   std::vector<QString> iVectChannels, QStringList iListTime, QWidget* iParent = 0);

  void SetSplitMergeMode(QWidget* iParent);

  /**
  \brief get the vtkpolydata for the new created meshes by the chosen algo
  */
  template<typename T>
  void GetPolydatasFromAlgo(T* iAlgo)
    {
    emit UpdateSeeds();
    std::vector<vtkPolyData*> NewMeshes = iAlgo->ApplyAlgo(
      this->m_Images, this->m_MeshEditingWidget->GetChannelNumber() );
    emit MeshesCreatedFromAlgo(NewMeshes, this->GetSelectedTimePoint() );
    emit ClearAllSeeds();
    }

  /**
  \brief get the sets of vtkpolydata for the new created sets of contours
  by the chosen algo
  */
  template<typename T>
  void GetSetOfPolydatasFromAlgo(T* iAlgo)
    {
    emit UpdateSeeds(); 
    std::vector< std::vector<vtkPolyData*> > NewSetsOfContours = 
      iAlgo->ApplyAlgoSeveralSeeds(this->m_Images,
        this->m_MeshEditingWidget->GetChannelNumber() );
    emit SetOfContoursFromAlgo(NewSetsOfContours , 
      this->GetSelectedTimePoint() );
    emit ClearAllSeeds();
    }
  
signals:
  /**
  \brief emit true to get the seeds widget enabled and false to disable it
  */
  void SetSeedInteractorBehaviour(bool enable);

protected slots:
  void ApplyLevelSetAlgo();
  void ApplyShapeAlgo();
  void ApplyWaterShedAlgo();
  void ApplyDanielAlgo();
  void ApplySetOfContoursWaterShedAlgo();
  void ApplySetOfContoursLevelSetAlgo();
  void ApplySetOfContoursShapeAlgo();

};

#endif