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
#include "QGoMeshEditingWidgetManager.h"
#include "QGoAlgorithmWidget.h"
#include "QGoAlgoParameter.h"
#include <iostream>


QGoMeshEditingWidgetManager::QGoMeshEditingWidgetManager(
  QStringList iListChannels, 
  QStringList iListTimePoints, 
  vtkPoints* iSeeds, 
  std::vector< vtkSmartPointer< vtkImageData > >* iImages, 
  QWidget* iParent)
{
 this->m_MeshEditingWidget = new QGoTraceEditingWidget(
   "Mesh", iListChannels, iListTimePoints, iParent);
 this->m_Seeds = iSeeds;
 this->m_Images = iImages;

 this->SetLevelSetAlgo(iParent);
 this->SetShapeAlgo(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoMeshEditingWidgetManager::~QGoMeshEditingWidgetManager()
{
  delete m_Radius;
  delete m_Curvature;
  delete m_Iterations;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetLevelSetAlgo(QWidget* iParent)
{
  QGoAlgorithmWidget* LevelSetWidget = 
    new QGoAlgorithmWidget("LevelSet 3D", iParent);

  m_Radius = new QGoAlgoParameter<double>("Radius", false, 0.1, 99.99, 2, 3);
  LevelSetWidget->AddParameter(m_Radius);
  m_Curvature = new QGoAlgoParameter<int>("Curvature", true, 0, 1000, 20);
  LevelSetWidget->AddParameter(m_Curvature);
  m_Iterations = new QGoAlgoParameter<int> ("Iterations", true, 0, 1000, 100);
  LevelSetWidget->AddParameter(m_Iterations);

  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomatedMode(LevelSetWidget);

  QObject::connect(LevelSetWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(GetSignalLevelSet() ) );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::SetShapeAlgo(QWidget* iParent)
{
    QGoAlgorithmWidget* ShapeWidget = new QGoAlgorithmWidget("Shape 3D", iParent);
  QGoAlgoParameter<double> Radius("Radius", false, 0.1, 99.99, 2, 3);
  ShapeWidget->AddParameter(&Radius);
  QStringList ShapeList;
  ShapeList.append("Sphere");
  ShapeList.append("Cube");
  QGoAlgoParameter<std::string> Shape("Shape",true, ShapeList, "Sphere");
  ShapeWidget->AddParameter(&Shape);
  
  this->m_MeshEditingWidget->AddAlgoWidgetForSemiAutomatedMode(ShapeWidget);

  QObject::connect(ShapeWidget, SIGNAL(ApplyAlgo() ),
    this, SLOT(GetSignalShape() ) );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::GetSignalLevelSet()
{
  std::cout<<"level set signal caught"<<std::endl;
  std::cout<<"Radius:"<<m_Radius->GetValue()<<std::endl;
  std::cout<<"Curvature:"<<m_Curvature->GetValue()<<std::endl;
  std::cout<<"Iterations:"<<m_Iterations->GetValue()<<std::endl;
  /*LevelSetfilter = new QGoFilterChanAndVese(this, iSampling); // 3 i.e. 3D, to
                                                                // create a mesh
  filter = m_BaseAlgorithmSegmentationWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationWidget->AddFilter( m_LevelSetfilter->getName() );
  m_LevelSetfilter->getWidget()->setParent(m_BaseAlgorithmSegmentationWidget);
  m_LevelSetfilter->setPoints( getSeed() );
  m_LevelSetfilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationWidget->GetFrame()->addWidget( m_LevelSetfilter->getWidget() );
  m_LevelSetfilter->ConnectSignals(filter);*/
  emit UpdateSeeds();

  ClearAllSeeds();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::GetSignalShape()
{
  std::cout<<"Shape signal caught"<<std::endl;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoMeshEditingWidgetManager::showWidget()
{
  this->m_MeshEditingWidget->CheckDefaultModes();
  this->m_MeshEditingWidget->show();
}