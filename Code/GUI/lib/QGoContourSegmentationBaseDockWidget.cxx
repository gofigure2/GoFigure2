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

#include "QGoContourSegmentationBaseDockWidget.h"

// base widgets
#include "QGoContourManualSegmentation.h"
#include "QGoContourSeedSegmentation.h"

//---------------------------------------------------------------------------//
QGoContourSegmentationBaseDockWidget::
QGoContourSegmentationBaseDockWidget(
    QWidget* iParent,
    vtkPoints* seeds,
    std::vector<vtkImageData*>* iOriginalImage) : QDockWidget(iParent)
{
  this->setupUi(this);

  this->setWindowTitle(QString::fromUtf8("Contour Segmentation"));

  QIcon ContourSegmentationIcon;
  ContourSegmentationIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/contourManual.png")),
                              QIcon::Normal, QIcon::Off);
  this->toggleViewAction()->setIcon(ContourSegmentationIcon);

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

  m_ContourManualSegmentation = new QGoContourManualSegmentation( this );

  QWidget* manual_widget = m_ContourManualSegmentation->getWidget();

  this->GetFrame()->layout()->addWidget( manual_widget );
//  manual_widget->setEnabled(false);
  manual_widget->setVisible(true);

  // connect show/hide
  QObject::connect(this, SIGNAL(ManualSegmentationActivated(bool)),
      manual_widget, SLOT(setVisible(bool)));
//  QObject::connect(this, SIGNAL(ManualSegmentation(bool)),
//      manual_widget, SLOT(setEnabled(bool)));

  // connect specific signals
  QObject::connect(m_ContourManualSegmentation, SIGNAL(validateContour()),
                     this, SIGNAL(ValidateContour()));
  QObject::connect(m_ContourManualSegmentation, SIGNAL(reinitializeContour()),
                     this, SIGNAL(ReinitializeContourWidget()));
  QObject::connect(m_ContourManualSegmentation, SIGNAL(changeContourRepresentationProperty(float, QColor, QColor, QColor)),
      this, SIGNAL(UpdateContourRepresentationProperties(float, QColor, QColor, QColor)));

  //----------------------------------------------------------------
  // Semi auto segmentation ( i.e. algo with seed)
  //----------------------------------------------------------------

  m_ContourSemiAutoSegmentation =
      // 0 2D
      // 1 3D
      // 2 2D+3D
      new QGoContourSeedSegmentation( this, seeds, iOriginalImage, 0);

  QWidget* semi_auto_widget = m_ContourSemiAutoSegmentation->getWidget();

  this->GetFrame()->layout()->addWidget( semi_auto_widget );
//  semi_auto_widget->setEnabled(false);
  semi_auto_widget->setVisible(false);

  // connect show/hide
  QObject::connect(this, SIGNAL(SemiAutoSegmentationActivated(bool)),
        semi_auto_widget, SLOT(setVisible(bool)));
//    QObject::connect(this, SIGNAL(SemiAutoSegmentation(bool)),
//        semi_auto_widget, SLOT(setEnabled(bool)));

    // connect semi-automatic segmentation specific signals
    QObject::connect(m_ContourSemiAutoSegmentation, SIGNAL(UpdateSeeds()),
        this, SIGNAL(UpdateSeeds()));
    QObject::connect(m_ContourSemiAutoSegmentation, SIGNAL(ContourCreated(vtkPolyData*)),
        this, SIGNAL(SaveAndVisuContour(vtkPolyData*)));
    QObject::connect(m_ContourSemiAutoSegmentation, SIGNAL(SegmentationFinished()),
        this, SIGNAL(ClearAllSeeds()));

}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QGoContourSegmentationBaseDockWidget::
~QGoContourSegmentationBaseDockWidget()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
QFrame*
QGoContourSegmentationBaseDockWidget::
GetFrame()
{
  return frame;
}

//---------------------------------------------------------------------------//
// one segmentation to another
//---------------------------------------------------------------------------//
void
QGoContourSegmentationBaseDockWidget::
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
QGoContourSegmentationBaseDockWidget::
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
QGoContourSegmentationBaseDockWidget::
SetChannel(int iChannel)
{
  m_ContourSemiAutoSegmentation->SetChannel(iChannel);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
bool
QGoContourSegmentationBaseDockWidget::
GetReeditMode()
{
  return m_ContourManualSegmentation->GetReeditMode();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoContourSegmentationBaseDockWidget::
SetReeditMode( bool iEnable)
{
  m_ContourManualSegmentation->SetReeditMode(iEnable);
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void
QGoContourSegmentationBaseDockWidget::
Initialize()
{
  m_ContourManualSegmentation->GenerateContourRepresentationProperties(true);
}
