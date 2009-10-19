#include "QGoChannelVisualizationDockWidget.h"

QGoChannelVisualizationDockWidget::
QGoChannelVisualizationDockWidget( QWidget* parent ) : QDockWidget( parent )
{
  this->setupUi( this );

  QObject::connect( this->buttonBox, SIGNAL( accepted() ),
    this, SLOT( Accept() ) );
}

QGoChannelVisualizationDockWidget::
~QGoChannelVisualizationDockWidget()
{
}

void
QGoChannelVisualizationDockWidget::
SetNumberOfChannels( const unsigned int& iN )
{
  this->ChannelComboBox->setMaxCount( iN );
}

void
QGoChannelVisualizationDockWidget::
SetChannel( const unsigned int& i, const QString& iText )
{
  QString input = QString( "Ch.%1" ).arg( i );

  if( !iText.isEmpty() )
    {
    input.append( iText );
    }
  this->ChannelComboBox->insertItem( i, input );
}

void
QGoChannelVisualizationDockWidget::
AddChannel( const QString& iText )
{
  int n = this->ChannelComboBox->count();

  QString input = QString( "Ch.%1" ).arg( n );

  if( !iText.isEmpty() )
    {
    input.append( iText );
    }
  this->ChannelComboBox->addItem( input );
}

void QGoChannelVisualizationDockWidget::
Accept()
{
  int idx = this->ChannelComboBox->currentIndex();
  if( idx == -1 )
    {
    return;
    }

  unsigned int temp = static_cast< unsigned int >( idx );

  if( this->AllChannelsBtn->isChecked() )
    {
    if( !m_AllChannels )
      {
      m_AllChannels = true;
      emit ChangeVisualizationToAllChannels();
      }
    }
  else
    {
    if( m_AllChannels )
      {
      m_AllChannels = false;
      m_ChannelId = temp;
      emit ChangeVisualizationToOneChannel( temp );
      }
    else
      {
      if( temp != m_ChannelId )
        {
        m_AllChannels = false;
        m_ChannelId = temp;
        emit ChangeVisualizationToOneChannel( temp );
        }
      }
    }
}


