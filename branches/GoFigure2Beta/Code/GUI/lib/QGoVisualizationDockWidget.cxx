#include "QGoVisualizationDockWidget.h"

QGoVisualizationDockWidget::
QGoVisualizationDockWidget( QWidget* iParent, const unsigned int& iDim ) :
  QDockWidget( iParent ),
  m_Dimension( iDim )
{
  this->setupUi( this );

  if( m_Dimension < 3 )
    {
    this->XSliceLbl->setVisible( false );
    this->XSliceSpinBox->setVisible( false );
    this->YSliceLbl->setVisible( false );
    this->YSliceSpinBox->setVisible( false );
    this->ZSliceLbl->setVisible( false );
    this->ZSliceSpinBox->setVisible( false );
    this->TSliceLbl->setVisible( false );
    this->TSliceSpinBox->setVisible( false );

    this->line->setVisible( false );
    }
  else
    {
    if( m_Dimension == 3 )
      {
      this->TSliceLbl->setVisible( false );
      this->TSliceSpinBox->setVisible( false );
      }
    }

    QObject::connect( this->XSliceSpinBox, SIGNAL( valueChanged( int ) ),
                      this, SIGNAL( XSliceChanged( int ) ) );

    QObject::connect( this->YSliceSpinBox, SIGNAL( valueChanged( int ) ),
                      this, SIGNAL( YSliceChanged( int ) ) );

    QObject::connect( this->ZSliceSpinBox, SIGNAL( valueChanged( int ) ),
                      this, SIGNAL( ZSliceChanged( int ) ) );

    QObject::connect( this->TSliceSpinBox, SIGNAL( valueChanged( int ) ),
                      this, SIGNAL( TSliceChanged( int ) ) );

    QObject::connect( this->AllChannelsBtn, SIGNAL( toggled( bool ) ),
                      this, SIGNAL( ShowAllChannelsChanged( bool ) ) );

    QObject::connect( this->ChannelComboBox, SIGNAL( currentIndexChanged( int ) ),
                      this, SIGNAL( ShowOneChannelChanged( int ) ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoVisualizationDockWidget::
~QGoVisualizationDockWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetXSlice( int iSlice )
{
  this->XSliceSpinBox->setValue( iSlice );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetYSlice( int iSlice )
{
  this->YSliceSpinBox->setValue( iSlice );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetZSlice( int iSlice )
{
  this->ZSliceSpinBox->setValue( iSlice );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetTSlice( int iSlice )
{
  this->TSliceSpinBox->setValue( iSlice );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVisualizationDockWidget::
SetNumberOfChannels( const unsigned int& iN )
{
  if( iN < 2 )
    {
    this->line->setVisible( false );
    this->AllChannelsBtn->setVisible( false );
    this->OneChannelBtn->setVisible( false );
    this->ChannelComboBox->setVisible( false );
    }
  else
    {
    this->line->setVisible( true );
    this->AllChannelsBtn->setVisible( true );
    this->OneChannelBtn->setVisible( true );
    this->ChannelComboBox->setMaxCount( iN );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoVisualizationDockWidget::
SetChannel( const unsigned int& i, const QString& iText )
{
  QString input = QString( "Ch.%1" ).arg( i );

  if( !iText.isEmpty() )
    {
    input.append( iText );
    }
  this->ChannelComboBox->insertItem( i, input );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetXMinimumAndMaximum( const int& iMin, const int& iMax )
{
  this->XSliceSpinBox->setMinimum( iMin );
  this->XSliceSpinBox->setMaximum( iMax );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetYMinimumAndMaximum( const int& iMin, const int& iMax )
{
  this->YSliceSpinBox->setMinimum( iMin );
  this->YSliceSpinBox->setMaximum( iMax );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetZMinimumAndMaximum( const int& iMin, const int& iMax )
{
  this->ZSliceSpinBox->setMinimum( iMin );
  this->ZSliceSpinBox->setMaximum( iMax );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::
SetTMinimumAndMaximum( const int& iMin, const int& iMax )
{
  this->TSliceSpinBox->setMinimum( iMin );
  this->TSliceSpinBox->setMaximum( iMax );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoVisualizationDockWidget::
GetCurrentChannel() const
{
  return this->ChannelComboBox->currentIndex();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoVisualizationDockWidget::GetCurrentCollectionID()
{
  return this->CollectionIDBox->currentText().toInt();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoVisualizationDockWidget::GetCurrentColor()
{
  return this->ColorCollectionBox->currentText().toStdString();
}