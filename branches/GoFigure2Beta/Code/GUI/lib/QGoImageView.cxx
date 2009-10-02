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
QGoImageView::QGoImageView( QWidget* parent ) : QWidget( parent ),
  m_Image( 0 ),
  m_SnapshotId( 0 ),
  m_Pool( 0 )
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
  double avg = ( r + g + b ) / 3.;

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

  for( int i = 0; i < 3; i++ )
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

