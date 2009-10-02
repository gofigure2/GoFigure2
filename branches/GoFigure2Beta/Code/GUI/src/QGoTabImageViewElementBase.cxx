#include "QGoTabImageViewElementBase.h"

#include <QColorDialog>
#include <QSettings>

QGoTabImageViewElementBase::
QGoTabImageViewElementBase( QWidget* parent ) :
  QGoTabElementBase( parent ),
  m_Color( false ),
  m_BackgroundColor( Qt::black )
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

  m_BackgroundColor = QColorDialog::getColor( m_BackgroundColor,
    this, tr( "Choose Background Color" ) );

  this->SetBackgroundColorToImageViewer();
}
//--------------------------------------------------------------------------