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
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageViewElementBase::~QGoTabImageViewElementBase()
{
  ContourStructureMultiIndexContainer::iterator it = m_ContourContainer.begin();
  ContourStructureMultiIndexContainer::iterator end = m_ContourContainer.begin();

  while( it != end )
    {
    it->Nodes->Delete();
    it->Actor->Delete();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageViewElementBase::SetColor( const bool& iColor )
{
  m_Color = iColor;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageViewElementBase::WriteSettings()
{
  QSettings settings;
  settings.beginGroup( "QGoTabImageViewElementBase" );
  settings.setValue( "BackgroundColor", m_BackgroundColor );
  settings.endGroup();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
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
void QGoTabImageViewElementBase::
ValidateContour( const int& iId )
{
  vtkPolyData* contour =
    m_ContourRepresentation[iId]->GetContourRepresentationAsPolyData();

  // get color from the dock widget
  double r, g ,b;
  QColor color = m_ManualSegmentationDockWidget->GetValidatedColor();
  color.getRgbF( &r, &g, &b );

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

  int* min_idx = this->GetImageCoordinatesFromWorldCoordinates( Min );
  int* max_idx = this->GetImageCoordinatesFromWorldCoordinates( Max );

  vtkPolyData* contour_nodes = vtkPolyData::New();
  m_ContourRepresentation[iId]->GetNodePolyData( contour_nodes );

  // get corresponding actor from visualization
  vtkPolyData* contour_copy = vtkPolyData::New();
  contour_copy->ShallowCopy( contour );

  std::vector< vtkActor* > contour_actor =
    this->AddContour( iId, contour_copy,
      contour_property );

  // get meshid from the dock widget (SpinBox)
  unsigned int meshid = m_ManualSegmentationDockWidget->GetMeshId();

  unsigned int timepoint = 0;
  bool highlighted = false;

  // fill the container
  for( int i = 0; i < contour_actor.size(); i++ )
    {
    ContourStructure temp( m_ContourId, contour_actor[i], contour_nodes, meshid,
      timepoint, highlighted, r, g, b, i );
    m_ContourContainer.insert( temp );
    }

  m_ContourId++;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageViewElementBase::
ValidateContour( )
{
  for( int i = 0; i < m_ContourWidget.size(); i++ )
    {
    ValidateContour( i );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
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

  for( int i = 0; i < m_ContourRepresentation.size(); i++ )
    {
    m_ContourRepresentation[i]->GetLinesProperty()->SetLineWidth( linewidth );
    m_ContourRepresentation[i]->GetLinesProperty()->SetColor( rl, gl, bl );

    m_ContourRepresentation[i]->GetProperty()->SetColor( rn, gn, bn );
    m_ContourRepresentation[i]->GetActiveProperty()->SetColor( ra, ga, ba );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
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
