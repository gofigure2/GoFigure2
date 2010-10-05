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

#include "QGoMeshSegmentationBaseDockWidget.h"

// base widgets
//#include "QGoContourManualSegmentation.h"
#include "QGoMeshSeedSegmentation.h"

//---------------------------------------------------------------------------//
QGoMeshSegmentationBaseDockWidget::
QGoMeshSegmentationBaseDockWidget(QWidget* iParent, vtkPoints* seeds,
    std::vector<vtkImageData*>* iOriginalImage) : QDockWidget(iParent)
{
  this->setupUi(this);

  // update interactor behavior
  QObject::connect(this->mode, SIGNAL(activated(int)),
                   this, SLOT(SegmentationMethod(int)));

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

  /// TODO add manual mesh segmentation capability

  //----------------------------------------------------------------
  // Semi auto segmentation ( i.e. algo with seed)
  //----------------------------------------------------------------

  m_MeshSemiAutoSegmentation =
      new QGoMeshSeedSegmentation( this, seeds, iOriginalImage);

  QWidget* semi_auto_widget = m_MeshSemiAutoSegmentation->getWidget();

  this->GetFrame()->layout()->addWidget( semi_auto_widget );
  semi_auto_widget->setVisible(false);

    // connect show/hide
    QObject::connect(this, SIGNAL(SemiAutoSegmentation(bool)),
                     semi_auto_widget, SLOT(setVisible(bool)));

    QObject::connect(this, SIGNAL(SemiAutoSegmentation(bool)),
                     semi_auto_widget, SLOT(setEnabled(bool)));

    // connect semi-automatic segmentation specific signals
    QObject::connect(m_MeshSemiAutoSegmentation, SIGNAL(UpdateSeeds()),
        this, SIGNAL(UpdateSeeds()));
    QObject::connect(m_MeshSemiAutoSegmentation, SIGNAL(MeshCreated(vtkPolyData*)),
        this, SIGNAL(SaveAndVisuMesh(vtkPolyData*)));
    QObject::connect(m_MeshSemiAutoSegmentation, SIGNAL(SegmentationFinished()),
        this, SIGNAL(ClearAllSeeds()));

}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoMeshSegmentationBaseDockWidget::
~QGoMeshSegmentationBaseDockWidget()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QFrame*
QGoMeshSegmentationBaseDockWidget::
GetFrame()
{
  return frame;
}

//---------------------------------------------------------------------------//
// one segmentation to another
//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::
SegmentationMethod(int iSegmentationMethod)
{
  emit ManualSegmentationActivated(false);
  emit SemiAutoSegmentationActivated(false);
  emit AutoSegmentationActivated(false);

  switch(iSegmentationMethod)
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
QGoMeshSegmentationBaseDockWidget::
interactorBehavior(bool iSegmentationMethod)
{
  if(iSegmentationMethod)
    {
    switch ( mode->currentIndex())
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

    this->setVisible( false );
    }
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::
SetChannel(int iChannel)
{
  m_MeshSemiAutoSegmentation->SetChannel(iChannel);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::
EnableAndShow( bool iEnable)
{
  if(iEnable)
    {
    //m_ContourManualSegmentation->getWidget()->show();
    //m_ContourManualSegmentation->getWidget()->setEnabled(iEnable);
    }
  else
    {
    //m_ContourManualSegmentation->getWidget()->hide();
    //m_ContourManualSegmentation->getWidget()->setEnabled(iEnable);
    }
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoMeshSegmentationBaseDockWidget::
Initialize()
{
  //m_ContourManualSegmentation->GenerateContourRepresentationProperties(true);
}
