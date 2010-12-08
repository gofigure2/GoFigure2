/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include "QGoMeshSeedSegmentation.h"

// To store the seeds
#include "vtkPoints.h"

// usefull to access internal methods
#include "QGoSeedBaseWidget.h"

// ALGOS
// LevelSet algorithm
#include "QGoFilterChanAndVes.h"
#include "QGoFilterShape.h"
#include "QGoFilterWatershed.h"

//--------------------------------------------------------------------------
QGoMeshSeedSegmentation::QGoMeshSeedSegmentation(QWidget *parentW,
                                                 vtkPoints *seeds,
                                                 std::vector< vtkSmartPointer<vtkImageData> > *iOriginalImage,
                                                 int iSampling):
  QGoSeedSegmentationBase(parentW, seeds, iSampling),
  m_OriginalImage(iOriginalImage)
{
  // ADD ARG IN CONSTRUCTORS TO ENABLE SAMPLING

  int filter = 0;

//=============================================================================
//=============================================================================

  //Add new segmentation method

  m_ShapeFilter = new QGoFilterShape(this, iSampling); // 3 i.e. 3D, to create a
                                                       // mesh
  filter = m_BaseAlgorithmSegmentationWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationWidget->AddFilter( m_ShapeFilter->getName() );
  m_ShapeFilter->getWidget()->setParent(m_BaseAlgorithmSegmentationWidget);
  m_ShapeFilter->setPoints( getSeed() );
  m_ShapeFilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationWidget->GetFrame()->addWidget(m_ShapeFilter->getWidget());
  m_ShapeFilter->ConnectSignals(filter);

  //=============================================================================

  //Add new segmentation method
  m_LevelSetfilter = new QGoFilterChanAndVes(this, iSampling); // 3 i.e. 3D, to
                                                               // create a mesh
  filter = m_BaseAlgorithmSegmentationWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationWidget->AddFilter( m_LevelSetfilter->getName() );
  m_LevelSetfilter->getWidget()->setParent(m_BaseAlgorithmSegmentationWidget);
  m_LevelSetfilter->setPoints( getSeed() );
  m_LevelSetfilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationWidget->GetFrame()->addWidget(m_LevelSetfilter->getWidget());
  m_LevelSetfilter->ConnectSignals(filter);

//=============================================================================

  //Add new segmentation method
  m_Watershed = new QGoFilterWatershed(this, iSampling);   // 3 i.e. 3D, to
                                                           // create a mesh
  filter = m_BaseAlgorithmSegmentationWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationWidget->AddFilter( m_Watershed->getName() );
  m_Watershed->getWidget()->setParent(m_BaseAlgorithmSegmentationWidget);
  m_Watershed->setPoints( getSeed() );
  m_Watershed->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationWidget->GetFrame()->addWidget(m_Watershed->getWidget());
  m_Watershed->ConnectSignals(filter);

  //m_BaseAlgorithmSegmentationWidget has what we want
  std::cout << "add1: " << m_BaseAlgorithmSegmentationWidget << std::endl;
  std::cout << "add2: " << m_BaseAlgorithmSegmentationWidget->GetFrame() << std::endl;
  std::cout << "add3: " << m_Watershed->getWidget() << std::endl;
  std::cout << "add4: " << m_Watershed->getWidget()->parentWidget() << std::endl;

//=============================================================================
//=============================================================================

  m_BaseAlgorithmSegmentationWidget->Initialize();

//=============================================================================
//=============================================================================
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoMeshSeedSegmentation::
~QGoMeshSeedSegmentation()
{}

//--------------------------------------------------------------------------
