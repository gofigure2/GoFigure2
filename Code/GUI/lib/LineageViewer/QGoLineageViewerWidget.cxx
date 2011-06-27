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

#include <QApplication>

#include "ui_QGoLineageViewerWidget.h"
#include "QGoLineageViewerWidget.h"

// QT  general
#include <QGridLayout>

// Delete Lineage
#include <QInputDialog>
#include <QStringList>

// tab view
#include <vtkQtTreeView.h>

// graph view
#include "vtkGraphLayoutView.h"
#include "vtkRenderWindow.h"
#include "vtkTreeLayoutStrategy.h"
// color coding
#include "vtkLookupTable.h"
#include "vtkViewTheme.h"

// create the tree
#include "vtkAdjacentVertexIterator.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkTree.h"

// get data representation
#include "vtkDataRepresentation.h"
// get vertex data
#include "vtkDataSetAttributes.h"

//connect table and graph
#include "vtkAnnotationLink.h"
#include <vtkEventQtSlotConnect.h>

// back plane
#include "vtkDelaunay2D.h"
#include "vtkGraphToPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRendererCollection.h"

//reader
#include <QFileDialog>
#include <QString>
#include "vtkTreeReader.h"

//----------------------------------------------------------------------------
// Constructor
QGoLineageViewerWidget::
QGoLineageViewerWidget( QWidget* iParent) :
  QDockWidget( iParent )
{
  this->ui = new Ui_QGoLineageViewerWidget;
  this->ui->setupUi(this);

  // we nee a graph as input of the graph view
  m_Graph = vtkSmartPointer<vtkMutableDirectedGraph>::New();

  // we need a tree as input for the table
  m_Tree = vtkSmartPointer<vtkTree>::New();
  m_Tree->CheckedDeepCopy(m_Graph);

  //Create the table View
  this->m_treeTableView          = vtkSmartPointer<vtkQtTreeView>::New();
  this->ConfigureTableView();

  //Create the graph View
  this->m_treeGraphView = vtkSmartPointer<vtkGraphLayoutView>::New();
  this->ConfigureGraphView();

  this->FillQtComboBoxes();

  // add link table and graph annotations
  this->m_annotationLink = vtkSmartPointer<vtkAnnotationLink>::New();
  this->m_treeGraphView->GetRepresentation()->SetAnnotationLink(this->m_annotationLink);
  this->m_treeTableView->GetRepresentation()->SetAnnotationLink(this->m_annotationLink);

  // connect table and graph
  this->m_connect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  this->m_connect->Connect(this->m_treeTableView->GetRepresentation(),
    vtkCommand::SelectionChangedEvent,
    this, SLOT(selectionChanged(vtkObject*, unsigned long, void*, void*)));
  this->m_connect->Connect(this->m_treeGraphView->GetRepresentation(),
    vtkCommand::SelectionChangedEvent,
    this, SLOT(selectionChanged(vtkObject*, unsigned long, void*, void*)));

  this->ConnectQtButtons();

  /////// EXPERIMENTAL ///////

  // create the back plane
  //this->m_backPlane = vtkSmartPointer<vtkDelaunay2D>::New();
  //this->m_graphToPolyData = vtkSmartPointer<vtkGraphToPolyData>::New();
  //this->m_graphToPolyData->SetInput(graph);
  //this->m_graphToPolyData->Update();
  //this->m_planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  //this->m_planeActor = vtkSmartPointer<vtkActor>::New();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
QGoLineageViewerWidget::~QGoLineageViewerWidget()
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::ConfigureGraphView()
{
  this->m_treeGraphView->AddRepresentationFromInput(m_Graph);
  this->m_treeGraphView->SetEdgeSelection(false);
  this->m_treeGraphView->SetLayoutStrategyToTree();
  this->m_treeGraphView->ResetCamera();
  this->m_treeGraphView->SetInteractor(
      this->ui->graphViewWidget->GetInteractor() );
  this->ui->graphViewWidget->SetRenderWindow(
      this->m_treeGraphView->GetRenderWindow() );

  // create LUT
  this->m_lut = vtkSmartPointer<vtkLookupTable>::New();
  this->m_lut->SetHueRange(0.667, 0.0);
  this->m_lut->Build();

  // create theme
  vtkSmartPointer<vtkViewTheme> theme =
    vtkSmartPointer<vtkViewTheme>::New();
  theme->SetPointLookupTable(this->m_lut);
  theme->SetCellLookupTable(this->m_lut);

  this->m_treeGraphView->ApplyViewTheme(theme);

  // create the layout strategy
  this->m_treeLayoutStrategy    = vtkSmartPointer<vtkTreeLayoutStrategy>::New();
  this->m_treeLayoutStrategy->SetAngle(90);
  this->m_treeLayoutStrategy->SetRadial(false);
  this->m_treeLayoutStrategy->SetLogSpacingValue(1);
  this->m_treeGraphView->SetLayoutStrategy(this->m_treeLayoutStrategy);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::ConfigureTableView()
{
  QGridLayout* tableLayout = new QGridLayout(this->ui->tableFrame);
  tableLayout->addWidget(this->m_treeTableView->GetWidget());
  this->m_treeTableView->AddRepresentationFromInput(m_Tree);
  this->m_treeTableView->ColorByArrayOn();
  this->m_treeTableView->SetShowRootNode(false);
  this->m_treeTableView->Update();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::ConnectQtButtons()
{
  // add a lineage
  connect(this->ui->addLineagePushButton, SIGNAL(pressed()),
    this, SLOT(slotAddLineage()));

  connect(this->ui->deleteLineagePushButton, SIGNAL(pressed()),
    this, SLOT(slotDeleteLineage()));

  // color coding
  connect(this->ui->colorCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableColorCode(int)));
  connect(this->ui->colorComboBox, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeColorCode(QString)));

  // scaling
  connect(this->ui->scaleCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableScale(int)));
  connect(this->ui->scaleComboBox, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeScale(QString)));

  // labeling
  connect(this->ui->labelCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableLabel(int)));
  connect(this->ui->labelComboBox, SIGNAL(currentIndexChanged(QString)),
    this, SLOT(slotChangeLabel(QString)));

  // radial rendering
  connect(this->ui->radialCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableRadialLayout(int)));
  connect(this->ui->radialSlider, SIGNAL(valueChanged(int)),
    this, SLOT(slotChangeRadialLayout(int)));

  // log rendering
  connect(this->ui->logCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableLog(int)));
  connect(this->ui->logSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(slotChangeLog(double)));

  // back plane
  connect(this->ui->backCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(slotEnableBackPlane(int)));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::FillQtComboBoxes()
{
  // Fill combo boxes
  // Update combo boxes (fill content with arrays names)
  int numberOfArrays = m_Graph->GetVertexData()->GetNumberOfArrays();
  this->ui->colorComboBox->clear();
  this->ui->scaleComboBox->clear();
  this->ui->labelComboBox->clear();

  // fill comboxes according to the data
  for(int i=0;i<numberOfArrays; i++)
    {
    const char* name =
        m_Graph->GetVertexData()->GetArrayName(i);
    this->ui->labelComboBox->addItem(name);
    // if data array (i.e. numbers), add it
    if(m_Graph->GetVertexData()->GetArray(name))
      {
      this->ui->colorComboBox->addItem(name);
      this->ui->scaleComboBox->addItem(name);
      }
    }

  // requiered to properly initialize the view since the scaling changes
  // when we fill the combo box
  this->slotEnableScale(false);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::selectionChanged(vtkObject*,
                                      unsigned long,
                                      void* vtkNotUsed(clientData),
                                      void* callData)
{
  this->m_treeTableView->Update();
  this->m_treeGraphView->Render();
 }
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotAddLineage()
{
  QString file = QFileDialog::getOpenFileName(NULL, tr("Select a lineage"));

  vtkSmartPointer<vtkTreeReader> reader =
      vtkSmartPointer<vtkTreeReader>::New();
  reader->SetFileName(file.toLocal8Bit().data());
  reader->Update();

  vtkSmartPointer<vtkTree> tree =
      vtkSmartPointer<vtkTree>::New();
  tree->CheckedDeepCopy(reader->GetOutput());

  // update list of graphs
  std::pair<QString, vtkSmartPointer<vtkTree> > treePair;
  treePair.first = file;
  treePair.second = tree;
  m_ListOfTrees.push_back(treePair);

  UpdateGraph();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::UpdateGraph()
{
  // create New graph
  vtkSmartPointer<vtkMutableDirectedGraph> newGraph =
      vtkSmartPointer<vtkMutableDirectedGraph>::New();
  vtkIdType rootID = newGraph->AddVertex();

  // info
  vtkSmartPointer<vtkDoubleArray> id =
      vtkSmartPointer<vtkDoubleArray>::New();
  id->SetName("Track ID");
  id->InsertValue(rootID, 0);

  vtkSmartPointer<vtkDoubleArray> depth =
      vtkSmartPointer<vtkDoubleArray>::New();
  depth->SetName("Lineage Depth");
  depth->InsertValue(rootID, 0);

  // fill the new graph
  std::list<std::pair<QString, vtkSmartPointer<vtkTree> > >::iterator
          it = m_ListOfTrees.begin();

  while(it != m_ListOfTrees.end())
    {
    UpdateTree( rootID,                // new ID
                it->second->GetRoot(), // old ID
                it->second,            // old graph
                newGraph,              // new graph
                id,                    // Track ID array
                1, depth);             // original depth, depth array

    ++it;
    }

  newGraph->GetVertexData()->AddArray(id);
  newGraph->GetVertexData()->AddArray(depth);


  m_Graph->CheckedDeepCopy(newGraph);
  m_Tree->CheckedDeepCopy(newGraph);

  this->ConfigureTableView();
  this->ConfigureGraphView();

  this->FillQtComboBoxes();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::UpdateTree(vtkIdType iParentID,
                               vtkIdType iOldID,
                               vtkSmartPointer<vtkTree> iOldTree,
                               vtkSmartPointer<vtkMutableDirectedGraph> iNewGraph,
                               vtkDoubleArray* iTrackIDArray,
                               unsigned int iDepth, vtkDoubleArray* iDepthArray)
{
  // build new tree
  vtkIdType newRoot = iNewGraph->AddChild(iParentID);

  // update information array
  vtkDataArray* id = iOldTree->GetVertexData()->GetArray("Track ID");
  double value = id->GetTuple1(iOldID);
  iTrackIDArray->InsertValue( newRoot, value );

  iDepthArray->InsertValue(newRoot, iDepth);

  // go through tree
  vtkSmartPointer<vtkAdjacentVertexIterator> it =
      vtkSmartPointer<vtkAdjacentVertexIterator>::New();
  iOldTree->GetChildren(iOldID, it);

  while(it->HasNext())
    {
    UpdateTree(newRoot, it->Next() , iOldTree, iNewGraph,
               iTrackIDArray,
               iDepth+1, iDepthArray);
    }

}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotDeleteLineage()
{
  bool ok;
  QStringList lineages;

  std::list<std::pair<QString, vtkSmartPointer<vtkTree> > >::iterator
          it = m_ListOfTrees.begin();

  while(it != m_ListOfTrees.end())
    {
    lineages << it->first;
    ++it;
    }

  QString item =
      QInputDialog::getItem(this,
                            tr("Lineage selection"),
                            tr("Please select the lineage you want to delete"),
                            lineages, 0, false, &ok);

  // Remove from the list
  it = m_ListOfTrees.begin();
  while(it != m_ListOfTrees.end())
    {
    if( ! it->first.compare(item) ) // compare returns 0 if QStrings are equal
      {
      m_ListOfTrees.erase(it);
      break;
      }
    ++it;
    }

  // update the graph
  UpdateGraph();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotEnableColorCode(int state)
{
  this->m_treeGraphView->SetColorVertices(state);

  //update visu
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotChangeColorCode(QString array)
{
  this->m_treeGraphView->SetVertexColorArrayName(array.toLocal8Bit().data());
  this->m_treeGraphView->SetEdgeColorArrayName(array.toLocal8Bit().data());

  //update visu
   this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotEnableScale(int state)
{
  //scale
  this->m_treeLayoutStrategy->SetDistanceArrayName
  (state ? this->ui->scaleComboBox->currentText().toLocal8Bit().data() : NULL);

  //update visu
  this->m_treeGraphView->ResetCamera();
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotChangeScale(QString array)
{
  this->m_treeLayoutStrategy->SetDistanceArrayName(array.toLocal8Bit().data());

  //update visu
  this->m_treeGraphView->ResetCamera();
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotEnableLabel(int state)
{
  //scale
  this->m_treeGraphView->SetVertexLabelVisibility(state);

  //update visu
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotChangeLabel(QString array)
{
  this->m_treeGraphView->SetVertexLabelArrayName(array.toLocal8Bit().data());

  //update visu
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotEnableRadialLayout(int state)
{
  if(!state)
    {
    this->m_treeLayoutStrategy->SetAngle(90);
    }
  else
    {
    this->m_treeLayoutStrategy->SetAngle( this->ui->radialSlider->value() );
    }

  //radial layout
  this->m_treeLayoutStrategy->SetRadial(state);

  //update visu
  this->m_treeGraphView->ResetCamera();
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotChangeRadialLayout(int angle)
{
  if(this->ui->radialCheckBox->isChecked() )
    {
    // change the layout angle
    this->m_treeLayoutStrategy->SetAngle( angle );

    //update visu
    this->m_treeGraphView->ResetCamera();
    this->m_treeGraphView->Render();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotEnableLog(int state)
{
  if(!state)
    {
    this->m_treeLayoutStrategy->SetLogSpacingValue(1);
    }
  else
    {
    this->m_treeLayoutStrategy->SetLogSpacingValue( this->ui->logSpinBox->value() );
    }

  //update visu
  this->m_treeGraphView->ResetCamera();
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotChangeLog(double angle)
{
  if(this->ui->logCheckBox->isChecked() )
    {
    // change the layout angle
    this->m_treeLayoutStrategy->SetLogSpacingValue( angle );

    //update visu
    this->m_treeGraphView->ResetCamera();
    this->m_treeGraphView->Render();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void QGoLineageViewerWidget::slotEnableBackPlane(int state)
{
  if(state)
    {
    //this->m_backPlane->SetInput(
    //   this->m_graphToPolyData->GetOutput());
    this->m_planeMapper->SetInput(
    this->m_graphToPolyData->GetOutput());
    this->m_planeActor->SetMapper(this->m_planeMapper);
    this->ui->graphViewWidget->GetRenderWindow()->GetRenderers()
        ->GetFirstRenderer()->AddActor(this->m_planeActor);
    }
  else
    {
    this->ui->graphViewWidget->GetRenderWindow()->GetRenderers()
        ->GetFirstRenderer()->RemoveActor(this->m_planeActor);
    }

  //update visu
  this->m_treeGraphView->Render();
}
//----------------------------------------------------------------------------
