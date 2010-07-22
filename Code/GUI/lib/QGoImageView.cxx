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
#include "QGoImageView.h"

#include "QVTKWidget.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkLookupTable.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"
#include "vtkViewImage2DCollection.h"
#include "vtkViewImage2DCollectionCommand.h"
#include "vtkViewImage2D.h"

//--------------------------------------------------------------------------
/**
 * \brief Default Constructor.
 * \param iParent
 */
QGoImageView::
QGoImageView(QWidget* iParent) : QWidget(iParent),
  m_Pool(0),
  m_Image(0),
  m_SnapshotId(0),
  m_ShowAnnotations(true),
  m_ShowSplinePlane(true)
  {
  m_Pool = vtkViewImage2DCollection::New();
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Default Destructor.
 */
QGoImageView::~QGoImageView()
  {
  if (m_Pool)
    {
    m_Pool->Delete();
    m_Pool = 0;
    }
  }
//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief Returns used background color by viewers.
 * \param[out] r red
 * \param[out] g green
 * \param[out] b blue
 */
void QGoImageView::GetBackgroundColor(double& r,
                                      double& g, double& b)
{
  double* rgb = this->GetBackgroundColor();
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \overload
 */
double* QGoImageView::GetBackgroundColor()
{
  return m_Pool->GetItem(0)->GetBackground();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief Set background color to be used by viewers.
 * \param[in] r
 * \param[in] g
 * \param[in] b
 */
void QGoImageView::SetBackgroundColor(const double& r,
                                      const double& g,
                                      const double& b)
{
  double textcolor[3];

  if ((r != 0.5) && (g != 0.5) && (b != 0.5))
    {
    textcolor[0] = 1. - r;
    textcolor[1] = 1. - g;
    textcolor[2] = 1. - b;
    }
  else
    {
    textcolor[0] = 1.;
    textcolor[1] = 1.;
    textcolor[2] = 0.;
    }

  double rgb[3] = { r, g, b };

  m_Pool->SyncSetBackground(rgb);
  int n = m_Pool->GetNumberOfItems();

  for (int i = 0; i < n; i++)
    {
    vtkTextProperty* tproperty =
      m_Pool->GetItem(i)->GetTextProperty();
    tproperty->SetFontFamilyToArial();
    tproperty->SetFontSize(14);
    tproperty->SetColor(textcolor);
    }

  m_Pool->SyncRender();
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \overload
 */
void QGoImageView::SetBackgroundColor(double rgb[3])
{
  this->SetBackgroundColor(rgb[0], rgb[1], rgb[2]);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \overload
 */
void QGoImageView::SetBackgroundColor(const QColor& iColor)
{
  double r, g, b;
  iColor.getRgbF(&r, &g, &b);

  this->SetBackgroundColor(r, g, b);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * \param[in] pos
 * \return
 */
int*
QGoImageView::
GetImageCoordinatesFromWorldCoordinates(double iPos[3])
{
  vtkViewImage2D* View = m_Pool->GetItem(0);
  return View->GetImageCoordinatesFromWorldCoordinates(iPos);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iId
 * \return
 */
vtkViewImage2D*
QGoImageView::
GetImageViewer(const int& iId)
{
  return m_Pool->GetItem(iId);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief
 * \return
 */
int
QGoImageView::
GetNumberOfImageViewers()
{
  return m_Pool->GetNumberOfItems();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Add contour with given property into the visualization.
 * \param[in] iId direction
 * \param[in] iDataset contour
 * \param[in] iProperty
 * \return vector of vtkActor rendered in each 2D viewer.
 * \todo check the utility of iId.
 */
// std::vector< vtkQuadricLODActor* >
std::vector<vtkActor*>
QGoImageView::
AddContour(const int& iId, vtkPolyData* iDataset, vtkProperty* iProperty)
{
  (void) iId;

  int n = m_Pool->GetNumberOfItems();

  std::vector<vtkActor*> oActorVector(n);
//   std::vector< vtkQuadricLODActor* > oActorVector( n );

  for (int i = 0; i < n; i++)
    {
    vtkViewImage2D* viewer = m_Pool->GetItem(i);
    //viewer-
//     vtkQuadricLODActor* temp =
    vtkActor* temp = viewer->AddDataSet(iDataset, iProperty);
    viewer->Render();
    oActorVector[i] = temp;
    }

  return oActorVector;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Highlight contour (or not).
 * \param[in] iProp contour
 * \param[in] iToDo to be highlighted
 */
void
QGoImageView::
ChangeActorProperty(vtkProp3D* iActor,
                    vtkProperty* iProperty)
{
  int n = m_Pool->GetNumberOfItems();

  for (int i = 0; i < n; i++)
    {
    vtkViewImage2D* viewer = m_Pool->GetItem(i);
    viewer->ChangeActorProperty(iActor, iProperty);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView::
RemoveActor(const int& iId, vtkActor* iActor)
{
  if ((iId >= 0) && (iId < m_Pool->GetNumberOfItems()))
    {
    vtkViewImage2D* viewer = m_Pool->GetItem(iId);
    viewer->GetRenderer()->RemoveActor(iActor);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView::
AddActor(const int& iId, vtkActor* iActor)
{
  if ((iId >= 0) && (iId < m_Pool->GetNumberOfItems()))
    {
    vtkViewImage2D* viewer = m_Pool->GetItem(iId);
    viewer->GetRenderer()->AddActor(iActor);
    }
}

//--------------------------------------------------------------------------
void
QGoImageView::
DefaultMode()
{
  // Change mode in the collection
  m_Pool->EnableDefaultInteractionMode();
}

//--------------------------------------------------------------------------
void
QGoImageView::
ZoomMode()
{
  // Change mode in the collection
  m_Pool->EnableZoomInteractionMode();
}

//--------------------------------------------------------------------------
void
QGoImageView::
PanMode()
{
  // Change mode in the collection
  m_Pool->EnablePanInteractionMode();
}

//-------------------------------------------------------------------------
void
QGoImageView::
ResetWindowLevel()
{
  m_Pool->SyncResetWindowLevel();
}

//-------------------------------------------------------------------------
void
QGoImageView::
SetLookupTable(vtkLookupTable* iLut)
{
  if (this->m_Image->GetNumberOfScalarComponents() == 1)
    {
    m_Pool->SyncSetLookupTable(iLut);
    m_Pool->SyncResetWindowLevel();
    m_Pool->SyncRender();
    }
}

//-------------------------------------------------------------------------
void
QGoImageView::
ShowScalarBar(const bool& iShow)
{
  if (this->m_Image->GetNumberOfScalarComponents() == 1)
    {
    m_Pool->SyncSetShowScalarBar(iShow);
    m_Pool->SyncRender();
    }
}

//-------------------------------------------------------------------------
void
QGoImageView::
UpdateRenderWindows()
{
  // Update Visualization
  // Update() not used because just want to update the renderWindow)
  vtkRenderWindow* ren;

  for (int i = 0; i < this->m_Pool->GetNumberOfItems(); ++i)
    {
    ren = this->m_Pool->GetItem(i)->GetRenderWindow();
    ren->Render();
    }
}

//-------------------------------------------------------------------------
void
QGoImageView::
ShowAnnotations()
{
  if (m_ShowAnnotations)
    {
    // remove annotations in 2d views
    m_ShowAnnotations = false;
    this->m_Pool->SyncSetShowAnnotations(m_ShowAnnotations);
    }
  else
    {
    m_ShowAnnotations = true;
    this->m_Pool->SyncSetShowAnnotations(m_ShowAnnotations);
    }

  UpdateRenderWindows();
}

//--------------------------------------------------------------------------
std::list<vtkProp3D*>
QGoImageView::
GetListOfPickedActors()
{
  return m_Pool->GetCommand()->GetListOfPickedActors();
}

//--------------------------------------------------------------------------
std::list<vtkProp3D*>
QGoImageView::
GetListOfUnPickedActors()
{
  return m_Pool->GetCommand()->GetListOfUnPickedActors();
}

//-------------------------------------------------------------------------
vtkImageActor*
QGoImageView::
GetImageActor(const int& iId)
{
  if ((iId < 0) || (iId > 2))
    {
    return NULL;
    }
  else
    {
    return m_Pool->GetItem(iId)->GetImageActor();
    }
}

//-------------------------------------------------------------------------
void
QGoImageView::
ChangeActorProperty(int iDir, vtkProp3D* iActor, vtkProperty* iProperty)
{
  m_Pool->GetItem(iDir)->ChangeActorProperty(iActor, iProperty);
}

//--------------------------------------------------------------------------
void
QGoImageView::
ShowSplinePlane()
{
  // Invert state of m_ShowPlane
  m_ShowSplinePlane = !m_ShowSplinePlane;
  this->m_Pool->SetSplinePlaneActorsVisibility(m_ShowSplinePlane);
}
