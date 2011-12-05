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

#ifndef QGoLineageViewerWidget_H
#define QGoLineageViewerWidget_H

#include <QDockWidget>

#include "vtkSmartPointer.h"

class vtkObject;

// Forward Qt class declarations
class Ui_QGoLineageViewerWidget;
//table view
class vtkQtTreeView;
//graph view
class vtkGraphLayoutView;
class vtkTreeLayoutStrategy;
class vtkLookupTable;

// connection between graph and table
class vtkAnnotationLink;
class vtkEventQtSlotConnect;

class vtkMutableDirectedGraph;
class vtkTree;

class vtkDoubleArray;



// EXPERIMENTAL
// back plane
class vtkDelaunay2D;
class vtkGraphToPolyData;
class vtkPolyDataMapper;
class vtkActor;

class QGoLineageViewerWidget : public QDockWidget
{
  Q_OBJECT

public:

  QGoLineageViewerWidget( QWidget * iParent = 0 );
  ~QGoLineageViewerWidget();

private:

  /*
   *
   */
  void ConfigureGraphView();
  /*
   *
   */
  void ConfigureTableView();

  /*
   *
   */
  void ConnectQtButtons();
  /*
   *
   */
  void FillQtComboBoxes();

  void UpdateTree(vtkIdType iParentID,
                  vtkIdType iOldID,
                  vtkSmartPointer<vtkTree> iOldTree,
                  vtkSmartPointer<vtkMutableDirectedGraph> iNewGraph,
                  vtkDoubleArray* iTrackIDArray,
                  unsigned int iDepth, vtkDoubleArray* iDepthArray);

  void UpdateGraph();

  vtkSmartPointer<vtkTree> m_Tree;
  vtkSmartPointer<vtkMutableDirectedGraph> m_Graph;

  std::list< std::pair<
    QString,vtkSmartPointer<vtkTree> > > m_ListOfTrees;

  vtkSmartPointer<vtkQtTreeView>         m_treeTableView;
  vtkSmartPointer<vtkGraphLayoutView>    m_treeGraphView;
  vtkSmartPointer<vtkAnnotationLink>     m_annotationLink;
  vtkSmartPointer<vtkEventQtSlotConnect> m_connect;
  vtkSmartPointer<vtkLookupTable>        m_lut;
  vtkSmartPointer<vtkTreeLayoutStrategy> m_treeLayoutStrategy;

  // Experimental stuff...
  vtkSmartPointer<vtkDelaunay2D>         m_backPlane;
  vtkSmartPointer<vtkGraphToPolyData>    m_graphToPolyData;
  vtkSmartPointer<vtkPolyDataMapper>     m_planeMapper;
  vtkSmartPointer<vtkActor>              m_planeActor;

  Ui_QGoLineageViewerWidget *ui;

private slots:

  void slotAddLineage();
  void slotDeleteLineage();

  void selectionChanged(vtkObject*, unsigned long,void* ,void* );

  void slotEnableScale(int state);
  void slotChangeScale(QString array);

  void slotEnableColorCode(int state);
  void slotChangeColorCode(QString array);

  void slotEnableLabel(int state);
  void slotChangeLabel(QString array);

  void slotEnableRadialLayout(int state);
  void slotChangeRadialLayout(int angle);

  void slotEnableLog(int state);
  void slotChangeLog(double angle);

  void slotEnableBackPlane(int state);
};

#endif // QGoLineageViewerWidget_H
