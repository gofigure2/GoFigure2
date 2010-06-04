/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#include "QGoComparer3D.h"

#include "QGoComparer3DSync.h"
#include "vtkImageData.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2D.h"
#include "vtkEventQtSlotConnect.h"
#include "QGoImageView3D.h"
#include "QGoCompareOrchestra.h"
#include "SnapshotHelper.h"

//--------------------------------------------------------------------------
QGoComparer3D::QGoComparer3D( QString iComparerName, QWidget *iParent )
 : QGoComparer( iComparerName, iParent )
 {
 }


//--------------------------------------------------------------------------
QGoComparer3D::
  ~QGoComparer3D()
{
  // remove the comparer from the orchestra
  if (m_currentOrchestra != NULL)
    {
    m_currentOrchestra->removeComparer3D(this);
    m_currentOrchestra = NULL ;
    }

  // delete the view if any
  if (HasViewer())
    {
    // we delete the viewer
    delete(static_cast<QGoImageView3D*>(m_currentView));
    m_currentView = NULL;
    }
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
PrintOs(ostream &os)
{
  // if we have an imageview, the we print its image information
  if (m_currentImage != NULL)
    {
    os << "Comparer 3D " << this << " contains :" << std::endl;
    m_currentImage->Print(os);
    }
else
    {
    os << "Comparer 3D " << this << " contains no Image :"<< std::endl;
    }
}


//--------------------------------------------------------------------------
/** \brief returns the type of comparer (2 for 2D, 3 for 3D)
*/
int
QGoComparer3D::
GetComparerType()
{
  return 3;
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetImage(vtkImageData* iImage)
{
  if (iImage == NULL)
    {
    return;
    }
  else
    {
    // if there is no viewer, we create one
    if (static_cast<QGoImageView3D*>(m_currentView) == NULL)
      {
      createViewer();
      }
    // set the image to the current view
    static_cast<QGoImageView3D*>(m_currentView)->SetImage(iImage);
    // update current image
    m_currentImage = iImage;
    this->Update();
    }
}



//--------------------------------------------------------------------------
void
QGoComparer3D::
Render(const int& iId)
{
  if (static_cast<QGoImageView3D*>(m_currentView) == NULL)
    {
    return;
    }
  else
    {
    if (iId <= 2) // if we want to render one of the 2D view
      {
      static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer(iId)->Render();
      }
    else // if we want to render the 3D view
      {
      static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer3D()->Render();
      }
    }
}

//--------------------------------------------------------------------------
// get the camera of the current viewer;
vtkCamera*
QGoComparer3D::
GetCamera(const int& iId)
{
  if (static_cast<QGoImageView3D*>(m_currentView) == NULL)
    {
    return NULL;
    }
  else
    {
    if (iId <= 2)
      {
      return static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer(iId)
                                                        ->GetRenderer()
                                                        ->GetActiveCamera();
      }
    else
      {
      return static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer3D()
                                                        ->GetRenderer()
                                                        ->GetActiveCamera();
      }
    }
}


//--------------------------------------------------------------------------
int
QGoComparer3D::
GetFullScreenView()
{
if ( HasViewer() )
  {
  return static_cast<QGoImageView3D*>(m_currentView)->GetFullScreenView();
  }
else
  {
  return -1;
  }
}


//--------------------------------------------------------------------------
QGoImageView3D*
QGoComparer3D::
GetImageView()
{
  if ( HasViewer() )
    {
    return static_cast<QGoImageView3D*>(m_currentView);
    }
  else
    {
    return NULL;
    }
}


//--------------------------------------------------------------------------
QString
QGoComparer3D::
SnapshotViewXY( const GoFigure::FileType& iType, const QString& iBaseName )
{
  return GetImageView()->SnapshotViewXY( iType, iBaseName);
}


//--------------------------------------------------------------------------
QString
QGoComparer3D::
SnapshotView2( const GoFigure::FileType& iType, const QString& iBaseName)
{
  return GetImageView()->SnapshotView2( iType, iBaseName);
}


//--------------------------------------------------------------------------
QString
QGoComparer3D::
SnapshotView3( const GoFigure::FileType& iType, const QString& iBaseName)
{
  return GetImageView()->SnapshotView3(iType,iBaseName);
}


//--------------------------------------------------------------------------
QString
QGoComparer3D::
SnapshotViewXYZ( const GoFigure::FileType& iType, const QString& iBaseName)
{
  return GetImageView()->SnapshotViewXYZ(iType,iBaseName);
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullScreenView(const int& iId)
{
  if ( HasViewer() )
    {
    static_cast<QGoImageView3D*>(m_currentView)->SetFullScreenView(iId);
    }
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullXYScreenView()
{
  SetFullScreenView(1);
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullXZScreenView()
{
  SetFullScreenView(2);
}

//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullYZScreenView()
{
  SetFullScreenView(3);
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullXYZScreenView()
{
  SetFullScreenView(4);
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetQuadView()
{
  SetFullScreenView(0);
}


/*
  Private
*/


//--------------------------------------------------------------------------
/** Create the viewer in the widget
*/
void
QGoComparer3D::
createViewer()
{
  // if there is already a viewer
  if (static_cast<QGoImageView3D*>(m_currentView) != NULL)
    {
    return;
    }
  else
    {
    // else we create one
    m_currentView = new QGoImageView3D(this);
    static_cast<QGoImageView3D*>
      (m_currentView)->setContentsMargins( 1, 1, 1, 1 );
    // setup position of the widget
    gridLayout->addWidget(static_cast<QGoImageView3D*>(m_currentView) );
    }
}
