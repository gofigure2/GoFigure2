#include "QGoTabImageViewElementBase.h"

#include <QColorDialog>
#include <QSettings>
#include <iostream>

#include "vtkImageData.h"
#include "vtkImageExtractComponents.h"

QGoTabImageViewElementBase::
QGoTabImageViewElementBase( QWidget* parent ) :
  QGoTabElementBase( parent ),
  m_Color( false ),
  m_BackgroundColor( Qt::black ),
  m_Image( 0 ),
  m_VisuDockWidget( 0 )
{}

QGoTabImageViewElementBase::~QGoTabImageViewElementBase()
{}

void QGoTabImageViewElementBase::SetColor( const bool& iColor )
{
  m_Color = iColor;
}

void QGoTabImageViewElementBase::WriteSettings()
{
  QSettings settings;
  settings.beginGroup( "QGoTabImageViewElementBase" );
  settings.setValue( "BackgroundColor", m_BackgroundColor );
  settings.endGroup();
}

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
void QGoTabImageViewElementBase::ChangeBackgroundColor()
{
  this->GetBackgroundColorFromImageViewer();

  QColor temp = QColorDialog::getColor( m_BackgroundColor,
    this, tr( "Choose Background Color" ) );

  if( temp != m_BackgroundColor )
    {
    m_BackgroundColor = temp;
    this->SetBackgroundColorToImageViewer();
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

  this->SetImageToImageViewer( m_Image );
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