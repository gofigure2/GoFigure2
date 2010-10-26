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

#include "QGoSegmentationBaseDockWidget.h"

// base widgets
#include "QGoContourManualSegmentation.h"
#include "QGoSeedSegmentation.h"

//---------------------------------------------------------------------------//
QGoSegmentationBaseDockWidget::QGoSegmentationBaseDockWidget(QWidget *iParent, vtkPoints *seeds,
                                                             std::vector< vtkImageData * > *iOriginalImage):QDockWidget(
    iParent)
{
  this->setupUi(this);

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

  m_ContourManualSegmentation = new QGoContourManualSegmentation(this, this);
  this->GetFrame()->layout()->addWidget( m_ContourManualSegmentation->getDockWidget() );
  m_ContourManualSegmentation->getDockWidget()->setEnabled(false);
  m_ContourManualSegmentation->getDockWidget()->setVisible(false);

  // connect show/hide
  QObject::connect( this, SIGNAL( ManualSegmentation(bool) ),
                    m_ContourManualSegmentation->getDockWidget(), SLOT( setVisible(bool) ) );
  QObject::connect( this, SIGNAL( ManualSegmentation(bool) ),
                    m_ContourManualSegmentation->getDockWidget(), SLOT( setEnabled(bool) ) );

  // connect specific signals
  QObject::connect( m_ContourManualSegmentation, SIGNAL( validateContour() ),
                    this, SIGNAL( ValidateContour() ) );
  QObject::connect( m_ContourManualSegmentation, SIGNAL( reinitializeContour() ),
                    this, SIGNAL( ReinitializeContourWidget() ) );
  QObject::connect( m_ContourManualSegmentation,
                    SIGNAL( changeContourRepresentationProperty(float, QColor, QColor, QColor) ),
                    this, SIGNAL( UpdateContourRepresentationProperties(float, QColor, QColor, QColor) ) );

  //----------------------------------------------------------------
  // Semi auto segmentation ( i.e. algo with seed)
  //----------------------------------------------------------------

  m_ContourSemiAutoSegmentation =
    new QGoSeedSegmentation(this, this, seeds, iOriginalImage);
  this->GetFrame()->layout()->addWidget( m_ContourSemiAutoSegmentation->getDockWidget() );
  m_ContourSemiAutoSegmentation->getDockWidget()->setEnabled(false);
  m_ContourSemiAutoSegmentation->getDockWidget()->setVisible(false);

  // connect show/hide
  QObject::connect( this, SIGNAL( SemiAutoSegmentation(bool) ),
                    m_ContourSemiAutoSegmentation->getDockWidget(), SLOT( setVisible(bool) ) );
  QObject::connect( this, SIGNAL( SemiAutoSegmentation(bool) ),
                    m_ContourSemiAutoSegmentation->getDockWidget(), SLOT( setEnabled(bool) ) );

  // connect semi-automatic segmentation specific signals
  QObject::connect( m_ContourSemiAutoSegmentation, SIGNAL( UpdateSeeds() ),
                    this, SIGNAL( UpdateSeeds() ) );
  QObject::connect( m_ContourSemiAutoSegmentation, SIGNAL( ContourCreated(vtkPolyData *) ),
                    this, SIGNAL( SaveAndVisuContour(vtkPolyData *) ) );
  QObject::connect( m_ContourSemiAutoSegmentation, SIGNAL( SegmentationFinished() ),
                    this, SIGNAL( ClearAllSeeds() ) );
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoSegmentationBaseDockWidget::
~QGoSegmentationBaseDockWidget()
{}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QFrame *
QGoSegmentationBaseDockWidget::GetFrame()
{
  return frame;
}

//---------------------------------------------------------------------------//
// one segmentation to another
//---------------------------------------------------------------------------//
void
QGoSegmentationBaseDockWidget::SegmentationMethod(int iSegmentationMethod)
{
  emit ManualSegmentation(false);
  emit SemiAutoSegmentation(false);
  emit AutoSegmentation(false);

  switch ( iSegmentationMethod )
    {
    case 0:
      emit ManualSegmentation(true);
      break;
    case 1:
      emit SemiAutoSegmentation(true);
      break;
    case 2:
      emit AutoSegmentation(true);
      break;
    default:
      emit ReinitializeInteractor(true);
      break;
    }
}

//---------------------------------------------------------------------------//
// show hide widget
//---------------------------------------------------------------------------//
void
QGoSegmentationBaseDockWidget::interactorBehavior(bool iSegmentationMethod)
{
  std::cout << "interactor behavior" << std::endl;

  if ( iSegmentationMethod )
    {
    switch ( mode->currentIndex() )
      {
      case 0:
        emit ManualSegmentation(iSegmentationMethod);
        break;
      case 1:
        emit SemiAutoSegmentation(iSegmentationMethod);
      case 2:
        emit AutoSegmentation(iSegmentationMethod);
        break;
      default:
        break;
      }
    }
  else
    {
    emit ManualSegmentation(false);
    emit SemiAutoSegmentation(false);
    emit AutoSegmentation(false);
    emit ReinitializeInteractor(true);
    }
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoSegmentationBaseDockWidget::SetChannel(int iChannel)
{
  m_ContourSemiAutoSegmentation->SetChannel(iChannel);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
bool
QGoSegmentationBaseDockWidget::GetReeditMode()
{
  return m_ContourManualSegmentation->GetReeditMode();
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoSegmentationBaseDockWidget::SetReeditMode(bool iEnable)
{
  m_ContourManualSegmentation->SetReeditMode(iEnable);
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoSegmentationBaseDockWidget::EnableAndShow(bool iEnable)
{
  if ( iEnable )
    {
    m_ContourManualSegmentation->getDockWidget()->show();
    m_ContourManualSegmentation->getDockWidget()->setEnabled(iEnable);
    }
  else
    {
    m_ContourManualSegmentation->getDockWidget()->hide();
    m_ContourManualSegmentation->getDockWidget()->setEnabled(iEnable);
    }
}

//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoSegmentationBaseDockWidget::Initialize()
{
  m_ContourManualSegmentation->GenerateContourRepresentationProperties(true);
}
