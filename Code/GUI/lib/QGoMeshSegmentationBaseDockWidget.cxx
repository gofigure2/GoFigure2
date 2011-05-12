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

#include "QGoMeshSegmentationBaseDockWidget.h"

// base widgets
#include "QGoMeshSeedSegmentation.h"

//---------------------------------------------------------------------------//
QGoMeshSegmentationBaseDockWidget::QGoMeshSegmentationBaseDockWidget(
  QWidget *iParent,
  vtkPoints *seeds,
  std::vector< vtkSmartPointer< vtkImageData > > *
  iOriginalImage) :
  QGoDockWidget(iParent)
{
  this->setupUi(this);

  this->setWindowTitle( QString::fromUtf8("Mesh Editing") );

  QIcon MeshSegmentationIcon;
  //MeshSegmentationIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/meshOneClick.png")),
  //                          QIcon::Normal, QIcon::Off);
  MeshSegmentationIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/MeshEditing.png") ),
                                 QIcon::Normal, QIcon::Off);

  this->m_ToggleAction->setIcon(MeshSegmentationIcon);
  this->m_ToggleAction->setToolTip( tr("Mesh Editing") );
  this->m_ToggleAction->setStatusTip( tr("Create meshes manually, semi-automatically or automatically") );

  // update interactor behavior
  QObject::connect( this->mode, SIGNAL( activated(int) ),
                    this, SLOT( SegmentationMethod(int) ) );

  // ADD BASE WIDGETS FOR EACH SEGMENTATION MODE

  //----------------------------------------------------------------
  //
  //  NOW WE HAVE TO FILL THE BASE DOCK WIDGET WITH WIDGETS
  //  1 widget for each segmentation type
  //  Create a method do the basic connections with a dockwidget
  //----------------------------------------------------------------

  //----------------------------------------------------------------
  // Manual segmentation
  //----------------------------------------------------------------

  m_MeshManualSegmentation =
    // 0 2D
    // 1 3D
    // 2 2D+3D
    new QGoMeshSeedSegmentation(this, seeds, iOriginalImage, 2);

  QWidget *manual_widget = m_MeshManualSegmentation->getWidget();

  gridLayout->addWidget(manual_widget, 1, 0, 1, -1);
  manual_widget->setVisible(false);

  // connect show/hide
  QObject::connect( this, SIGNAL( ManualSegmentationActivated(bool) ),
                    manual_widget, SLOT( setVisible(bool) ) );

  // connect semi-automatic segmentation specific signals
  QObject::connect( m_MeshManualSegmentation, SIGNAL( UpdateSeeds() ),
                    this, SIGNAL( UpdateSeeds() ) );

  QObject::connect( m_MeshManualSegmentation, SIGNAL( CreateCorrespondingMesh(int) ),
                    this, SIGNAL( CreateCorrespondingMesh(int) ) );

  QObject::connect( m_MeshManualSegmentation, SIGNAL( AddContourForMeshToContours(vtkPolyData *) ),
                    this, SIGNAL( AddContourForMeshToContours(vtkPolyData *) ) );
  QObject::connect( m_MeshManualSegmentation, SIGNAL( SegmentationFinished() ),
                    this, SIGNAL( ClearAllSeeds() ) );

  //----------------------------------------------------------------
  // Semi auto segmentation ( i.e. algo with seed)
  //----------------------------------------------------------------

  m_MeshSemiAutoSegmentation =
    new QGoMeshSeedSegmentation(this, seeds, iOriginalImage, 1);

  QWidget *semi_auto_widget = m_MeshSemiAutoSegmentation->getWidget();

  gridLayout->addWidget(semi_auto_widget, 1, 0, 1, -1);
  semi_auto_widget->setVisible(false);

  // connect show/hide
  QObject::connect( this, SIGNAL( SemiAutoSegmentationActivated(bool) ),
                    semi_auto_widget, SLOT( setVisible(bool) ) );

  // connect semi-automatic segmentation specific signals
  QObject::connect( m_MeshSemiAutoSegmentation, SIGNAL( UpdateSeeds() ),
                    this, SIGNAL( UpdateSeeds() ) );
  QObject::connect( m_MeshSemiAutoSegmentation, SIGNAL( MeshCreated(vtkPolyData *, int) ),
                    this, SIGNAL( SaveAndVisuMesh(vtkPolyData *, int) ) );
  QObject::connect( m_MeshSemiAutoSegmentation, SIGNAL( SegmentationFinished() ),
                    this, SIGNAL( ClearAllSeeds() ) );

  // set default segmentation as semi auto segmentation
  this->mode->setCurrentIndex(1);
  gridLayout->setSizeConstraint(QLayout::SetFixedSize);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoMeshSegmentationBaseDockWidget::
~QGoMeshSegmentationBaseDockWidget()
{
}

//---------------------------------------------------------------------------//
// one segmentation to another
//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::SegmentationMethod(int iSegmentationMethod)
{
  emit ManualSegmentationActivated(false);
  emit SemiAutoSegmentationActivated(false);
  emit AutoSegmentationActivated(false);

  switch ( iSegmentationMethod )
    {
    case 0:
      emit ManualSegmentationActivated(true);
      break;
    case 1:
      emit SemiAutoSegmentationActivated(true);
      break;
    case 2:
      emit AutoSegmentationActivated(true);
      break;
    default:
      emit ReinitializeInteractorActivated(true);
      break;
    }
}

//---------------------------------------------------------------------------//
// show hide widget
//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::interactorBehavior(bool iSegmentationMethod)
{
  if ( iSegmentationMethod )
    {
    switch ( mode->currentIndex() )
      {
      case 0:
        emit ManualSegmentationActivated(iSegmentationMethod);
        break;
      case 1:
        emit SemiAutoSegmentationActivated(iSegmentationMethod);
        break;
      case 2:
        emit AutoSegmentationActivated(iSegmentationMethod);
        break;
      default:
        break;
      }
    }
  else
    {
    emit ManualSegmentationActivated(false);
    emit SemiAutoSegmentationActivated(false);
    emit AutoSegmentationActivated(false);
    emit ReinitializeInteractorActivated(true);

    this->setVisible(false);
    }
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::SetChannel(int iChannel, const QString & iText)
{
  QString input;

  if ( !iText.isEmpty() )
    {
    input = iText;
    }
  else
    {
    input = QString("Channel %1").arg(iChannel);
    }

 // m_MeshManualSegmentation->SetChannel(iChannel, input);
 // m_MeshSemiAutoSegmentation->SetChannel(iChannel, input);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::SetCurrentChannel(unsigned int iChannel)
{
  m_MeshManualSegmentation->SetCurrentChannel(iChannel);
  m_MeshSemiAutoSegmentation->SetCurrentChannel(iChannel);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::SetNumberOfChannels(int iNumberOfChannels)
{
  m_MeshManualSegmentation->SetNumberOfChannels(iNumberOfChannels);
  m_MeshSemiAutoSegmentation->SetNumberOfChannels(iNumberOfChannels);
}

//---------------------------------------------------------------------------//
/*void
QGoMeshSegmentationBaseDockWidget::Initialize()
{
  //m_ContourManualSegmentation->GenerateContourRepresentationProperties(true);
}
*/
