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
#include "QGoSeedSegmentation.h"

// DockWidget
#include "QGoBaseAlgorithmSegmentationDockWidget.h"

// To store the seeds
#include "vtkPoints.h"

// ALGOS
// LevelSet algorithm
#include "QGoFilterChanAndVes.h"
#include "QGoFilterShape.h"
//#include "Watershed.h"

// Reconstruct mesh
#include "vtkMarchingCubes.h"

//--------------------------------------------------------------------------
QGoSeedSegmentation::QGoSeedSegmentation(QObject *parentO, QWidget *parentW, vtkPoints *seeds,
                                         std::vector< vtkImageData * > *iOriginalImage):
  QGoSeedSegmentationBase(parentO, seeds)
{
  int filter = 0;

  m_OriginalImage = iOriginalImage;

//=============================================================================
//=============================================================================

  //Create associated dockwidget
  m_BaseAlgorithmSegmentationDockWidget = new QGoBaseAlgorithmSegmentationDockWidget(parentW);
  // connect with 3DwT to add the good number of channels
  QObject::connect( this, SIGNAL( addChannel(QString) ),
                    m_BaseAlgorithmSegmentationDockWidget, SLOT( AddChannel(QString) ) );
  // mesh has been created by a filter
  QObject::connect( m_BaseAlgorithmSegmentationDockWidget, SIGNAL( MeshCreated(vtkPolyData *) ),
                    this, SIGNAL( MeshCreated(vtkPolyData *) ) );
  // contour has been created by a filter
  QObject::connect( m_BaseAlgorithmSegmentationDockWidget, SIGNAL( ContourCreated(vtkPolyData *) ),
                    this, SIGNAL( ContourCreated(vtkPolyData *) ) );
  // image has been processed by a filter
  // should return the new image as argument
  QObject::connect( m_BaseAlgorithmSegmentationDockWidget, SIGNAL( ImageProcessed() ),
                    this, SIGNAL( ImageProcessed() ) );
  QObject::connect( m_BaseAlgorithmSegmentationDockWidget, SIGNAL( Radius(double) ),
                    this, SLOT( setRadius(double) ) );

  QObject::connect( m_BaseAlgorithmSegmentationDockWidget, SIGNAL( UpdateSeeds() ),
                    this, SIGNAL( UpdateSeeds() ) );
  QObject::connect( m_BaseAlgorithmSegmentationDockWidget, SIGNAL( SegmentationFinished() ),
                    this, SIGNAL( SegmentationFinished() ) );

//=============================================================================
//=============================================================================

  //Add new segmentation method
  m_LevelSet2Dfilter = new QGoFilterChanAndVes;
  filter = m_BaseAlgorithmSegmentationDockWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationDockWidget->AddFilter( m_LevelSet2Dfilter->getName() );
  m_LevelSet2Dfilter->getWidget()->setParent( m_BaseAlgorithmSegmentationDockWidget->GetFrame() );
  m_LevelSet2Dfilter->setPoints( getSeed() );
  m_LevelSet2Dfilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationDockWidget->GetFrame()->layout()->addWidget( m_LevelSet2Dfilter->getWidget() );
  m_LevelSet2Dfilter->ConnectSignals(filter);

//=============================================================================

  //Add new segmentation method

  m_ShapeFilter = new QGoFilterShape;
  filter = m_BaseAlgorithmSegmentationDockWidget->GetNumberOfFilters();
  m_BaseAlgorithmSegmentationDockWidget->AddFilter( m_ShapeFilter->getName() );
  m_ShapeFilter->getWidget()->setParent( m_BaseAlgorithmSegmentationDockWidget->GetFrame() );
  m_ShapeFilter->setPoints( getSeed() );
  m_ShapeFilter->setOriginalImageMC(m_OriginalImage);
  m_BaseAlgorithmSegmentationDockWidget->GetFrame()->layout()->addWidget( m_ShapeFilter->getWidget() );
  m_ShapeFilter->ConnectSignals(filter);

//=============================================================================
//=============================================================================

  m_BaseAlgorithmSegmentationDockWidget->Initialize();

//=============================================================================
//=============================================================================
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoSeedSegmentation::
~QGoSeedSegmentation()
{
  if ( m_LevelSet2Dfilter )
    {
    delete m_LevelSet2Dfilter;
    }

  if ( m_ShapeFilter )
    {
    delete m_ShapeFilter;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QWidget *
QGoSeedSegmentation::getDockWidget()
{
  return m_BaseAlgorithmSegmentationDockWidget;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSeedSegmentation::ConnectSignals(QGoBaseAlgorithmSegmentationDockWidget *iDockWidget)
{
  QObject::connect( this, SIGNAL( addChannel(QString) ),
                    iDockWidget, SLOT( AddChannel(QString) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSeedSegmentation::SetChannel(int i)
{
  emit addChannel( QString::number(i, 10) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoSeedSegmentation::ReconstructMesh(vtkImageData *iInputImage)
{
  // create iso-contours
  vtkMarchingCubes *contours = vtkMarchingCubes::New();

  contours->SetInput(iInputImage);
  contours->GenerateValues (1, 0, 0);
  contours->SetComputeGradients(0);
  contours->SetComputeNormals(0);
  contours->SetComputeScalars(0);
  contours->SetNumberOfContours(1);

  //Update required here!!
  contours->Update();

  return contours->GetOutput();
}

//--------------------------------------------------------------------------
