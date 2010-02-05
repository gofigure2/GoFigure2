/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#include <set>

//--------------------------------------------------------------------------
/**
 * \brief Default Constructor
 * \param iParent
 */
QGoTabImageViewElementBase::
QGoTabImageViewElementBase( QWidget* iParent ) :
  QGoTabElementBase( iParent ),
  m_Color( false ),
  m_BackgroundColor( Qt::black ),
  m_ContourId( 0 ),
  m_ReEditContourMode( false ),
  m_VisuDockWidget( 0 )
#ifdef   ENABLEVIDEORECORD
  ,
  m_VideoRecorderWidget( 0 )
#endif
{
  CreateManualSegmentationdockWidget();

  m_DockWidgetList.push_back(
    std::pair< Qt::DockWidgetArea, QDockWidget* >( Qt::LeftDockWidgetArea,
      m_VisuDockWidget ) );

  m_DockWidgetList.push_back(
    std::pair< Qt::DockWidgetArea, QDockWidget* >( Qt::LeftDockWidgetArea,
      m_ManualSegmentationDockWidget ) );

#ifdef ENABLEVIDEORECORD
  m_DockWidgetList.push_back(
    std::pair< Qt::DockWidgetArea, QDockWidget* >( Qt::LeftDockWidgetArea,
      m_VideoRecorderWidget ) );
#endif
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Destructor
 */
QGoTabImageViewElementBase::~QGoTabImageViewElementBase()
{
  ContourMeshStructureMultiIndexContainer::iterator
    it = m_ContourMeshContainer.begin();
  ContourMeshStructureMultiIndexContainer::iterator
    end = m_ContourMeshContainer.end();

  std::set< vtkPolyData* > NodeSet;

  while( it != end )
    {
    NodeSet.insert( it->Nodes );
    it->Actor->Delete();
    ++it;
    }

  std::set< vtkPolyData* >::iterator NodeSetIt = NodeSet.begin();
  std::set< vtkPolyData* >::iterator NodeSetEnd = NodeSet.end();

  while( NodeSetIt != NodeSetEnd )
    {
    (*NodeSetIt)->Delete();
    ++NodeSetIt;
    }
}
//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief
 */
void
QGoTabImageViewElementBase::
CreateManualSegmentationdockWidget()
{
  m_ManualSegmentationDockWidget = new QGoManualSegmentationDockWidget( this );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ValidatePressed() ),
    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ReinitializePressed() ),
    this, SLOT( ReinitializeContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget,
      SIGNAL( ActivateManualSegmentationToggled( bool ) ),
    this, SLOT( ActivateManualSegmentationEditor( bool ) ) );

  QObject::connect( m_ManualSegmentationDockWidget,
    SIGNAL( ContourRepresentationPropertiesChanged() ),
    this, SLOT( ChangeContourRepresentationProperty() ) );

  this->m_SegmentationActions.push_back(
    m_ManualSegmentationDockWidget->toggleViewAction() );
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iColor
 */
void QGoTabImageViewElementBase::SetColor( const bool& iColor )
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
  settings.beginGroup( "QGoTabImageViewElementBase" );
  settings.setValue( "BackgroundColor", m_BackgroundColor );
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
  settings.beginGroup( "QGoTabImageViewElementBase" );
  QVariant var = settings.value( "BackgroundColor" );
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
    this, tr( "Choose Background Color" ) );

  if( temp.isValid() )
    {
    if( temp != m_BackgroundColor )
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
void QGoTabImageViewElementBase::
ActivateManualSegmentationEditor( const bool& iActivate )
{
  std::vector< vtkSmartPointer< vtkContourWidget > >::iterator it =
    m_ContourWidget.begin();
  while( it != m_ContourWidget.end() )
    {
    if( iActivate )
      {
      (*it)->On();
      }
    else
      {
      (*it)->Off();
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
void QGoTabImageViewElementBase::
ValidateContour( const int& iId )
{
  vtkPolyData* contour =
    m_ContourRepresentation[iId]->GetContourRepresentationAsPolyData();

  // get color from the dock widget
  double r, g ,b;
  r = 0.1;
  g = 0.5;
  b = 0.7;
  //   QColor color = m_ManualSegmentationDockWidget->GetValidatedColor();
  //   color.getRgbF( &r, &g, &b );

  /// \todo get alpha from QColor (it is not supposed to 255 all the time!)
  double alpha = 1.;

  vtkProperty* contour_property = vtkProperty::New();
  contour_property->SetRepresentationToWireframe();
  contour_property->SetColor( r, g, b );

  // Compute Bounding Box
  double bounds[6];
  contour->GetBounds( bounds );

  // Extract Min and Max from bounds
  double Min[3], Max[3];
  int k = 0;
  for( int i = 0; i < 3; i++ )
    {
    Min[i] = bounds[k++];
    Max[i] = bounds[k++];
    }


// *** Get the Bounding Box ***
//   int* min_idx = this->GetImageCoordinatesFromWorldCoordinates( Min );
//   int* max_idx = this->GetImageCoordinatesFromWorldCoordinates( Max );
//
//   (void) min_idx;
//   (void) max_idx;
// ****************************

  vtkPolyData* contour_nodes = vtkPolyData::New();
  m_ContourRepresentation[iId]->GetNodePolyData( contour_nodes );

  // get corresponding actor from visualization
  vtkPolyData* contour_copy = vtkPolyData::New();
  contour_copy->ShallowCopy( contour );

  std::vector< vtkQuadricLODActor* > contour_actor =
    this->AddContour( iId, contour_copy,
      contour_property );

  contour_copy->Delete();
  contour_property->Delete();

  /// \todo use m_ContourMeshContainer here!

//   // get meshid from the dock widget (SpinBox)
// //  unsigned int meshid = m_ManualSegmentationDockWidget->GetMeshId();
//   unsigned int meshid = 0;
//
//   if( this->m_VisuDockWidget->GetCurrentCollectionID() != -1 )
//     {
//     meshid =
//       static_cast< unsigned int >( this->m_VisuDockWidget->GetCurrentCollectionID() );
//     }
//
//   unsigned int timepoint = 0;
//   bool highlighted = false;
//
//   // fill the container
//
//   for( unsigned int i = 0; i < contour_actor.size(); i++ )
//     {
//     ContourMeshStructure temp( m_ContourId, contour_actor[i], contour_nodes, meshid,
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
void QGoTabImageViewElementBase::
ValidateContour( )
{
  for( unsigned int i = 0; i < m_ContourWidget.size(); i++ )
    {
    ValidateContour( i );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageViewElementBase::
ChangeContourRepresentationProperty()
{
  double linewidth = m_ManualSegmentationDockWidget->GetLinesWidth();
  QColor linecolor = m_ManualSegmentationDockWidget->GetLinesColor();
  QColor nodecolor = m_ManualSegmentationDockWidget->GetNodesColor();
  QColor activenodecolor = m_ManualSegmentationDockWidget->GetActiveNodesColor();

  double rl, gl, bl;
  linecolor.getRgbF( &rl, &gl, &bl );

  double rn, gn, bn;
  nodecolor.getRgbF( &rn, &gn, &bn );

  double ra, ga, ba;
  activenodecolor.getRgbF( &ra, &ga, &ba );

  for( unsigned int i = 0; i < m_ContourRepresentation.size(); i++ )
    {
    m_ContourRepresentation[i]->GetLinesProperty()->SetLineWidth( linewidth );
    m_ContourRepresentation[i]->GetLinesProperty()->SetColor( rl, gl, bl );

    m_ContourRepresentation[i]->GetProperty()->SetColor( rn, gn, bn );
    m_ContourRepresentation[i]->GetActiveProperty()->SetColor( ra, ga, ba );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Re-edit contour given by its id
 * \param iId
 */
void
QGoTabImageViewElementBase::
ReEditContour( const unsigned int& iId )
{
  if( !m_ContourMeshContainer.empty() )
    {
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = m_ContourMeshContainer.get< TraceID >().find( iId );

    if( it != m_ContourMeshContainer.get< TraceID >().end() )
      {
      int c_dir;
      vtkActor* c_actor;
      vtkPolyData* c_nodes;

      while( it != m_ContourMeshContainer.get< TraceID >().end() )
        {
        if( it->TraceID == iId )
          {
          c_dir = (*it).Direction;
          c_actor = (*it).Actor;
          c_nodes = (*it).Nodes;

          RemoveActorFromViewer( c_dir, c_actor );
          }
        else
          {
          break;
          }

        ++it;
        }

      m_ContourMeshContainer.erase( iId );

      if( m_ContourWidget.size() > 1 )
        {
        int dir = ComputeDirectionFromContour( c_nodes );

        if( dir != -1 )
          {
          m_ReEditContourMode = true;
          m_ContourId = iId;

          double p[3];
          c_nodes->GetPoint( 0, p );
          int* idx = this->GetImageCoordinatesFromWorldCoordinates( p );
          this->SetSlice( dir, idx );

          m_ContourWidget[dir]->Initialize( c_nodes );
          m_ManualSegmentationDockWidget->ActivateManualSegmentation( true );
          }
        }
      else
        {
        m_ContourWidget[0]->Initialize( c_nodes );
        m_ManualSegmentationDockWidget->ActivateManualSegmentation( true );
        }
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Remove the contour (vtkActor) from the viewer (iId).
 * Note that iActor is not deleted. It has to be deleted by the end-user.
 * \param iId
 * \param iActor
 */
// void
// QGoTabImageViewElementBase::
// RemoveContour( const int& iId, vtkActor* iActor )
// {
//   vtkViewImage2D* viewer = this->m_Pool->GetItem( iId );
//   viewer->GetRenderer()->RemoveActor( iActor );
// }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// void
// QGoTabImageViewElementBase::
// DisplayContour( const int& iId, vtkActor* iActor )
// {
//   vtkViewImage2D* viewer = this->m_Pool->GetItem( iId );
//   viewer->GetRenderer()->AddActor( iActor );
// }
//--------------------------------------------------------------------------



























