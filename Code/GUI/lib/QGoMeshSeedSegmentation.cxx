/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 2037 $  // Revision of last commit
  Date: $Date: 2010-08-23 16:33:20 -0400 (Mon, 23 Aug 2010) $  // Date of last commit
=========================================================================*/

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
//#include "Watershed.h"

//--------------------------------------------------------------------------
QGoMeshSeedSegmentation::
QGoMeshSeedSegmentation(QObject* parentO, QWidget * parentW, vtkPoints* seeds,
    std::vector<vtkImageData*>* iOriginalImage)
:QGoSeedSegmentationBase(parentO, parentW, seeds)
{
  int filter = 0;

  m_OriginalImage = iOriginalImage;

//=============================================================================
//=============================================================================

  //Add new segmentation method
  m_LevelSetfilter = new QGoFilterChanAndVes(3); // 3 i.e. 3D, to create a mesh
  filter = m_BaseAlgorithmSegmentationWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationWidget->AddFilter( m_LevelSetfilter->getName() );
  m_LevelSetfilter->getWidget()->setParent(m_BaseAlgorithmSegmentationWidget->GetFrame());
  m_LevelSetfilter->setPoints(getSeed());
  m_LevelSetfilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationWidget->GetFrame()->layout()->addWidget(m_LevelSetfilter->getWidget());
  m_LevelSetfilter->ConnectSignals(filter);

//=============================================================================

  //Add new segmentation method

  m_ShapeFilter = new QGoFilterShape(3); // 3 i.e. 3D, to create a mesh
  filter = m_BaseAlgorithmSegmentationWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationWidget->AddFilter( m_ShapeFilter->getName() );
  m_ShapeFilter->getWidget()->setParent(m_BaseAlgorithmSegmentationWidget->GetFrame());
  m_ShapeFilter->setPoints(getSeed());
  m_ShapeFilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationWidget->GetFrame()->layout()->addWidget(m_ShapeFilter->getWidget());
  m_ShapeFilter->ConnectSignals(filter);


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
{
  if(m_LevelSetfilter)
    {
    delete m_LevelSetfilter;
    }

  if(m_ShapeFilter)
    {
    delete m_ShapeFilter;
    }
}
//--------------------------------------------------------------------------
