#include "QGoTabImageViewElementBase.h"

#include <QColorDialog>
#include <QSettings>
#include <iostream>

#include "vtkImageData.h"
#include "vtkImageExtractComponents.h"
#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"

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
  m_Image( 0 ),
  m_VisuDockWidget( 0 )
{
  m_ManualSegmentationDockWidget = new QGoManualSegmentationDockWidget( this );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ValidatePressed() ),
    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget,
      SIGNAL( ActivateManualSegmentationToggled( bool ) ),
    this, SLOT( ActivateManualSegmentationEditor( bool ) ) );

  QObject::connect( m_ManualSegmentationDockWidget,
    SIGNAL( ContourRepresentationPropertiesChanged() ),
    this, SLOT( ChangeContourRepresentationProperty() ) );

  this->m_SegmentationActions.push_back(
    m_ManualSegmentationDockWidget->toggleViewAction() );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Destructor
 */
QGoTabImageViewElementBase::~QGoTabImageViewElementBase()
{
  for( unsigned int i = 0; i < m_ContourRepresentation.size(); i++ )
    {
    m_ContourRepresentation[i]->Delete();
    m_ContourWidget[i]->Delete();
    }

  ContourMeshStructureMultiIndexContainer::iterator it = m_ContourMeshContainer.begin();
  ContourMeshStructureMultiIndexContainer::iterator end = m_ContourMeshContainer.end();

  while( it != end )
    {
    it->Nodes->Delete();
    it->Actor->Delete();
    ++it;
    }
}
//--------------------------------------------------------------------------

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
 * \brief Set the image to be displaid
 * \param[in] iImage
 */
void QGoTabImageViewElementBase::
SetImage( vtkImageData* iImage )
{
  if( !iImage )
    {
    std::cerr <<"QGoTabImageViewElementBase::SetImage( 0x0 )" <<std::endl;
    return;
    }

  m_Image = iImage;

  int n = m_Image->GetNumberOfScalarComponents();
  m_VisuDockWidget->SetNumberOfChannels( n );

  if( n != 1 )
    {
    if( ( n == 3 ) || ( n == 4 ) )
      {
      m_VisuDockWidget->SetChannel( 0, tr( "Red" ) );
      m_VisuDockWidget->SetChannel( 1, tr( "Green" ) );
      m_VisuDockWidget->SetChannel( 2, tr( "Blue" ) );

      if( n == 4 )
        {
        m_VisuDockWidget->SetChannel( 3, tr( "Alpha" ) );
        }
      }
    else
      {
      for( int i = 0; i < n; i++ )
        {
        m_VisuDockWidget->SetChannel( i );
        }
      }
    }

  int extent[6];
  m_Image->GetExtent( extent );

  this->SetImageToImageViewer( m_Image );

  m_VisuDockWidget->SetXMinimumAndMaximum( extent[0], extent[1] );
  m_VisuDockWidget->SetXSlice( (extent[0]+extent[1])/2 );

  m_VisuDockWidget->SetYMinimumAndMaximum( extent[2], extent[3] );
  m_VisuDockWidget->SetYSlice( (extent[2]+extent[3])/2 );

  m_VisuDockWidget->SetZMinimumAndMaximum( extent[4], extent[5] );
  m_VisuDockWidget->SetZSlice( (extent[4]+extent[5])/2 );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iChecked
 */
void QGoTabImageViewElementBase::
ShowAllChannels( bool iChecked )
{
  if( iChecked )
    {
    this->SetImageToImageViewer( m_Image );
    }
  else
    {
    int ch = this->m_VisuDockWidget->GetCurrentChannel();
    if( ch != -1 )
      {
      vtkImageExtractComponents* extract = vtkImageExtractComponents::New();
      extract->SetInput( m_Image );
      extract->SetComponents( ch );
      extract->Update();

      this->SetImageToImageViewer( extract->GetOutput() );

      extract->Delete();
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iChannel
 */
void QGoTabImageViewElementBase::
ShowOneChannel( int iChannel )
{
  if( m_Image )
    {
    if( iChannel != -1 )
      {
      vtkImageExtractComponents* extract = vtkImageExtractComponents::New();
      extract->SetInput( m_Image );
      extract->SetComponents( iChannel );
      extract->Update();

      this->SetImageToImageViewer( extract->GetOutput() );

      extract->Delete();
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
  std::vector< vtkContourWidget* >::iterator it = m_ContourWidget.begin();
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
  QColor color = m_ManualSegmentationDockWidget->GetValidatedColor();
  color.getRgbF( &r, &g, &b );

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

  std::vector< vtkActor* > contour_actor =
    this->AddContour( iId, contour_copy,
      contour_property );

  contour_copy->Delete();
  contour_property->Delete();

  // get meshid from the dock widget (SpinBox)
  unsigned int meshid = m_ManualSegmentationDockWidget->GetMeshId();

  unsigned int timepoint = 0;
  bool highlighted = false;

  // fill the container
  for( unsigned int i = 0; i < contour_actor.size(); i++ )
    {
    ContourMeshStructure temp( m_ContourId, contour_actor[i], contour_nodes, meshid,
      timepoint, highlighted, r, g, b, alpha, i );
    m_ContourMeshContainer.insert( temp );
    }

  m_ContourId++;
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
 *
 * \return
 */
std::list< QDockWidget* >
QGoTabImageViewElementBase::
DockWidget()
{
  std::list< QDockWidget* > oList;
  oList.push_back( m_VisuDockWidget );
  oList.push_back( m_ManualSegmentationDockWidget );

  return oList;
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
  std::list< ContourMeshStructure* > c_list =
    FindContourGivenTraceID( this->m_ContourMeshContainer, iId );

  std::list< ContourMeshStructure* >::iterator c_it = c_list.begin();
  std::list< ContourMeshStructure* >::iterator c_end = c_list.end();

  int c_dir;
  vtkActor* c_actor;
  vtkPolyData* c_nodes;

  while( c_it != c_end )
    {
    c_dir = (*c_it)->Direction;
    c_actor = (*c_it)->Actor;
    c_nodes = (*c_it)->Nodes;

    this->RemoveActorFromViewer( c_dir, c_actor );

    // The contour actor is supposed to be deleted!!!
    c_actor->Delete();

    this->m_ContourWidget[c_dir]->On();
    this->m_ContourWidget[c_dir]->Initialize( c_nodes );

    ++c_it;
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



























