#include "QGoImageView.h"

#include "QVTKWidget.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkLookupTable.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"
#include "vtkViewImage2DCollection.h"
#include "vtkViewImage2D.h"

//--------------------------------------------------------------------------
QGoImageView::QGoImageView( QWidget* iParent ) : QWidget( iParent ),
  m_Pool( 0 ),
  m_Image( 0 ),
  m_SnapshotId( 0 )
{
  m_Pool = vtkViewImage2DCollection::New();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoImageView::~QGoImageView()
{
  if( m_Pool )
    {
    m_Pool->Delete();
    m_Pool = 0;
    }
}
//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView::GetBackgroundColor( double& r,
  double& g, double& b )
{
  double* rgb = this->GetBackgroundColor();
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double* QGoImageView::GetBackgroundColor()
{
  return m_Pool->GetItem( 0 )->GetBackground();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView::SetBackgroundColor( const double& r,
  const double& g,
  const double& b )
{
  double textcolor[3];

  if( ( r != 0.5 ) && ( g != 0.5 ) && ( b != 0.5 ) )
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

  double rgb[3] = {r, g, b };

  m_Pool->SyncSetBackground( rgb );

  for( int i = 0; i < m_Pool->GetNumberOfItems(); i++ )
    {
    vtkTextProperty* tproperty =
      m_Pool->GetItem( i )->GetTextProperty();
    tproperty->SetFontFamilyToArial();
    tproperty->SetFontSize( 14 );
    tproperty->SetColor( textcolor );
    }
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoImageView::SetBackgroundColor( double rgb[3] )
{
  this->SetBackgroundColor( rgb[0], rgb[1], rgb[2] );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoImageView::SetBackgroundColor( const QColor& iColor )
{
  double r, g, b;
  iColor.getRgbF( &r, &g, &b );

  this->SetBackgroundColor( r, g, b );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int* QGoImageView::
GetImageCoordinatesFromWorldCoordinates( double pos[3] )
{
  vtkViewImage2D* View = m_Pool->GetItem( 0 );
  return View->GetImageCoordinatesFromWorldCoordinates( pos );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkViewImage2D* QGoImageView::
GetImageViewer( const int& iId )
{
  return m_Pool->GetItem( iId );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoImageView::
GetNumberOfImageViewers()
{
  return m_Pool->GetNumberOfItems();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< vtkActor* >
QGoImageView::
AddContour( const int& iId, vtkPolyData* dataset, vtkProperty* property )
{
  int n = m_Pool->GetNumberOfItems();
  std::vector< vtkActor* > oActorVector;

  for( int i = 0; i < n; i++ )
    {
    vtkViewImage2D* viewer = m_Pool->GetItem( i );
    vtkActor* temp = viewer->AddDataSet( dataset, property,
      true, false );
    oActorVector.push_back( temp );
    }

  return oActorVector;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView::
HighlightContour( vtkProp3D* iProp, const bool& iToDo )
{
  int n = m_Pool->GetNumberOfItems();

  for( int i = 0; i < n; i++ )
    {
    vtkViewImage2D* viewer = m_Pool->GetItem( i );
    viewer->HighlightContour( iProp, iToDo );
    }
}
//--------------------------------------------------------------------------