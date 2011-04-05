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
#ifndef __QGoContourEditingWidgetManager_h
#define __QGoContourEditingWidgetManager_h

#include "QGoTraceEditingWidgetManager.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include <QAction>
#include <QDockWidget>


/**
\class QGoContourEditingWidgetManager handles the interactions between the user
and the algorithms for the contours
\brief
*/
class QGOGUILIB_EXPORT QGoContourEditingWidgetManager: public QGoTraceEditingWidgetManager
{
  Q_OBJECT
public:

  QGoContourEditingWidgetManager(std::vector<QString> iVectChannels, 
    int iTimeMin, int iTimeMax, vtkPoints* iSeeds, 
    std::vector< vtkSmartPointer< vtkImageData > >* iImages, 
    int* iCurrentTimePoint,
    QWidget* iParent=0);

  ~QGoContourEditingWidgetManager();

  /**
  \brief display only the current timepoint in the TSlice comboboxes
  of the qgoalgomanagerwidgets, disable them and enable the channel comboboxes
  */
  //void SetTSliceForClassicView();

   /**
  \brief display the 3 timepoints chosen by the user in the TSlice comboboxes
  of the qgoalgomanagerwidgets, enable them, display only the channel tracked
  by the user and disable the channel comboboxes
  */
  //void SetTSliceForDopplerView(QStringList iListTimePoints, int iChannelNumber);

public slots:
  
signals:

protected:

  /*QGoMeshLevelSetAlgo*                            m_LevelSetAlgo;
  QGoMeshShapeAlgo*                               m_ShapeAlgo;
  QGoMeshWaterShedAlgo*                           m_WaterShedAlgo;
  QGoMeshSplitDanielssonDistanceAlgo*             m_DanielAlgo;*/

  /**
  \brief add the algowidget of the different algo in the algomanagerwidget
  for the semi automatic mode and set the different SIGNAL/SLOTS connections
  */
  virtual void SetSemiAutomaticAlgorithms(QWidget* iParent = 0);

  
signals:
  
protected slots:
  /*void ApplyLevelSetAlgo();
  void ApplyShapeAlgo();
  void ApplyWaterShedAlgo();*/

};

#endif