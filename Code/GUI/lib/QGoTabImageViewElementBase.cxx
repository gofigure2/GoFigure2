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

#include "QGoTabImageViewElementBase.h"

#include <QColorDialog>
#include <QSettings>
#include <iostream>

#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"

#include "ContourContainer.h"
#include "MeshContainer.h"

#include "QGoNavigationDockWidget.h"
#include "QGoContourManualSegmentationWidget.h"

//--------------------------------------------------------------------------
/**
 * \brief Default Constructor
 * \param iParent
 */
QGoTabImageViewElementBase::QGoTabImageViewElementBase(QWidget *iParent):
  QGoTabElementBase(iParent),
  m_Color(false),
  m_BackgroundColor(Qt::black),
  m_ContourId(0),
  m_ReEditContourMode(false),
  m_NavigationDockWidget(0)
{
  /// \todo fix this
  m_ContourContainer = new ContourContainer(this, NULL);
  m_MeshContainer = new MeshContainer(this, NULL);

  CreateManualSegmentationdockWidget();

  CreateToolsActions();

  /// \todo fix it is not a dockwidget anymore
  //m_DockWidgetList.push_back(
  //  std::pair<QGoDockWidgetStatus*, QDockWidget*>(
  //    new QGoDockWidgetStatus(m_ManualSegmentationWidget,
  // Qt::LeftDockWidgetArea, true, true),
  //   m_ManualSegmentationWidget));
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Destructor
 */
QGoTabImageViewElementBase::~QGoTabImageViewElementBase()
{}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief
 */
void
QGoTabImageViewElementBase::CreateManualSegmentationdockWidget()
{
  m_ManualSegmentationWidget = new QGoContourManualSegmentationWidget(this);

  QObject::connect( m_ManualSegmentationWidget, SIGNAL( ValidatePressed() ),
                    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationWidget, SIGNAL( ReinitializePressed() ),
                    this, SLOT( ReinitializeContour() ) );

  QObject::connect( m_ManualSegmentationWidget,
                    SIGNAL( UpdateContourRepresentationProperties() ),
                    this, SLOT( ChangeContourRepresentationProperty() ) );

//  QAction* tempaction = m_ManualSegmentationWidget->toggleViewAction();

//  this->m_SegmentationActions.push_back(tempaction);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageViewElementBase::ChangeContourRepresentationProperty()
{
  float  linewidth = static_cast< float >( m_LinesWidth );
  QColor linecolor = m_LinesColor;
  QColor nodecolor = m_NodesColor;
  QColor activenodecolor = m_ActiveNodesColor;

  double rl, gl, bl;

  linecolor.getRgbF(&rl, &gl, &bl);

  double rn, gn, bn;
  nodecolor.getRgbF(&rn, &gn, &bn);

  double ra, ga, ba;
  activenodecolor.getRgbF(&ra, &ga, &ba);

  for ( unsigned int i = 0; i < m_ContourRepresentation.size(); i++ )
    {
    m_ContourRepresentation[i]->GetLinesProperty()->SetLineWidth(linewidth);
    m_ContourRepresentation[i]->GetLinesProperty()->SetColor(rl, gl, bl);

    m_ContourRepresentation[i]->GetProperty()->SetColor(rn, gn, bn);
    m_ContourRepresentation[i]->GetActiveProperty()->SetColor(ra, ga, ba);
    }
}

//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iColor
 */
void QGoTabImageViewElementBase::SetColor(const bool & iColor)
{
  m_Color = iColor;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Write settings:
 * \li \c m_BackgroundColor
 */
void QGoTabImageViewElementBase::WriteSettings()
{
  QSettings settings;

  settings.beginGroup("QGoTabImageViewElementBase");
  settings.setValue("BackgroundColor", m_BackgroundColor);
  settings.endGroup();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Read settings:
 * \li \c m_BackgroundColor
 */
void QGoTabImageViewElementBase::ReadSettings()
{
  QSettings settings;

  settings.beginGroup("QGoTabImageViewElementBase");
  QVariant var = settings.value("BackgroundColor");
  m_BackgroundColor = var.value< QColor >();

  this->SetBackgroundColorToImageViewer();

  settings.endGroup();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Change background color
 */
void QGoTabImageViewElementBase::ChangeBackgroundColor()
{
  this->GetBackgroundColorFromImageViewer();

  QColor temp = QColorDialog::getColor( m_BackgroundColor,
                                        this, tr("Choose Background Color") );

  if ( temp.isValid() )
    {
    if ( temp != m_BackgroundColor )
      {
      m_BackgroundColor = temp;
      this->SetBackgroundColorToImageViewer();
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * \param[in] iActivate
 */
void QGoTabImageViewElementBase::ActivateManualSegmentationEditor(const bool & iActivate)
{
  std::vector< vtkSmartPointer< vtkContourWidget > >::iterator it =
    m_ContourWidget.begin();
  while ( it != m_ContourWidget.end() )
    {
    if ( iActivate )
      {
      ( *it )->On();
      }
    else
      {
      ( *it )->Off();
      }
    ++it;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param[in] iId
 */
void QGoTabImageViewElementBase::ValidateContour(const int & iId)
{
  vtkPolyData *contour =
    m_ContourRepresentation[iId]->GetContourRepresentationAsPolyData();

  // get color from the dock widget
  double r, g, b, a;

  r = 0.1;
  g = 0.5;
  b = 0.7;
  a = 1.;

  vtkProperty *contour_property = vtkProperty::New();
  contour_property->SetRepresentationToWireframe();
  contour_property->SetColor(r, g, b);
  contour_property->SetOpacity(a);

  // Compute Bounding Box
  double bounds[6];
  contour->GetBounds(bounds);

  // Extract Min and Max from bounds
  double Min[3], Max[3];
  int    k = 0;
  for ( int i = 0; i < 3; i++ )
    {
    Min[i] = bounds[k++];
    Max[i] = bounds[k++];
    }

// *** Get the Bounding Box ***
//   int* min_idx = this->GetImageCoordinatesFromWorldCoordinates( Min );
//   int* max_idx = this->GetImageCoordinatesFromWorldCoordinates( Max );
//   (void) min_idx;
//   (void) max_idx;
// ****************************

  vtkPolyData *contour_nodes = vtkPolyData::New();
  m_ContourRepresentation[iId]->GetNodePolyData(contour_nodes);

  // get corresponding actor from visualization
  vtkPolyData *contour_copy = vtkPolyData::New();
  contour_copy->ShallowCopy(contour);

//   std::vector< vtkQuadricLODActor* > contour_actor =
  std::vector< vtkActor * > contour_actor =
    this->AddContour(contour_copy,
                     contour_property);

  contour_copy->Delete();
  contour_property->Delete();

  /// \todo use m_ContourMeshContainer here!

//   // get meshid from the dock widget (SpinBox)
// //  unsigned int meshid = m_ManualSegmentationWidget->GetMeshId();
//   unsigned int meshid = 0;
//
//   if( this->m_NavigationDockWidget->GetCurrentCollectionID() != -1 )
//     {
//     meshid =
//       static_cast< unsigned int >(
// this->m_NavigationDockWidget->GetCurrentCollectionID() );
//     }
//
//   unsigned int timepoint = 0;
//   bool highlighted = false;
//
//   // fill the container
//
//   for( unsigned int i = 0; i < contour_actor.size(); i++ )
//     {
//     ContourMeshStructure temp( m_ContourId, contour_actor[i], contour_nodes,
// meshid,
//       timepoint, highlighted, r, g, b, alpha, i );
//     m_ContourMeshContainer.insert( temp );
//     }
//
//   m_ContourId++;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageViewElementBase::ValidateContour()
{
  for ( unsigned int i = 0; i < m_ContourWidget.size(); i++ )
    {
    ValidateContour(i);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Re-edit contour given by its id
 * \param iId
 */
void
QGoTabImageViewElementBase::ReEditContour(const unsigned int & iId)
{
  ContourMeshContainer::MultiIndexContainerTraceIDIterator
    it = m_ContourContainer->m_Container.get< TraceID >().find(iId);

  if ( it != m_ContourContainer->m_Container.get< TraceID >().end() )
    {
    vtkPolyData *c_nodes = NULL;

    /// \todo remove actor from the visualization and update!
//          c_dir = (*it).Direction;
//          c_actor = (*it).Actor;
//          c_nodes = (*it).Nodes;
//
//          RemoveActorFromViewer(c_dir, c_actor);

    m_ContourContainer->m_Container.erase(iId);

    if ( m_ContourWidget.size() > 1 )
      {
      int dir =
        ContourMeshContainer::ComputeDirectionFromContour(c_nodes);

      if ( dir != -1 )
        {
        m_ReEditContourMode = true;
        m_ContourId = iId;

        double p[3];
        c_nodes->GetPoint(0, p);
        int *idx = this->GetImageCoordinatesFromWorldCoordinates(p);
        this->SetSlice(dir, idx);

        delete[] idx;

        m_ContourWidget[dir]->Initialize(c_nodes);
        m_ManualSegmentationWidget->setEnabled(true);
        }
      }
    else
      {
      m_ContourWidget[0]->Initialize(c_nodes);
      m_ManualSegmentationWidget->setEnabled(true);
      }
    }
}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageViewElementBase::ReinitializeContour()
{
  for ( unsigned int i = 0; i < m_ContourWidget.size(); i++ )
    {
    /// \todo to be fully compliant with more recent version of vtk,
    /// we should rather use m_ContourWidget[i]->Initialize()
    m_ContourWidget[i]->Initialize(NULL);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageViewElementBase::CreateToolsActions()
{
  m_TakeSnapshotAction = new QAction(tr("Take Snapshot"), this);
  QIcon snapshoticon;
  snapshoticon.addPixmap(QPixmap( QString::fromUtf8(":/fig/camera-photo.png") ),
                         QIcon::Normal, QIcon::Off);
  m_TakeSnapshotAction->setIcon(snapshoticon);
  m_TakeSnapshotAction->setStatusTip( tr("You have to be in full screen view to use the snapshot") );
  m_TakeSnapshotAction->setEnabled(false);

  QObject::connect( m_TakeSnapshotAction, SIGNAL( triggered() ),
                    this, SLOT( TakeSnapshot() ) );

  this->m_ToolsActions.push_back(m_TakeSnapshotAction);
}

//-------------------------------------------------------------------------
