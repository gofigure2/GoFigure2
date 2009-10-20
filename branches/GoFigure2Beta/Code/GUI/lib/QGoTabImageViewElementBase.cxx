#include "QGoTabImageViewElementBase.h"

#include <QColorDialog>
#include <QSettings>

QGoTabImageViewElementBase::
QGoTabImageViewElementBase( QWidget* parent ) :
  QGoTabElementBase( parent ),
  m_Color( false ),
  m_BackgroundColor( Qt::black ),
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