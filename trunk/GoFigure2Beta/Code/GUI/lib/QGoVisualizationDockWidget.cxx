#include "QGoVisualizationDockWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

QGoVisualizationDockWidget::
QGoVisualizationDockWidget( QWidget* iParent, const unsigned int& iDim ) :
  QDockWidget( iParent ),
  m_Dimension( iDim )
{
  this->setupUi( this );
  SetColorTraceComboBox();
  SetColorIDCollectionComboBox();
  //this->verticalLayout_2->addLayout(this->VLayoutForCollection);

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
  std::string temp = this->ColorIDCollectionComboBox->GetCurrentColorData().first;
  if( temp.empty() )
    {
    return -1;
    }
  else
    {
    return atoi( temp.c_str() );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::SetCollectionID(
  std::list<std::pair<std::string,QColor> > iListExistingID)
{
  std::list<std::pair<std::string,QColor> >::iterator iter =
    iListExistingID.begin();
  while (iter != iListExistingID.end())
    {
    QColor color = iter->second;
    QString name = iter->first.c_str();
    this->ColorIDCollectionComboBox->addColor(color,name);
    iter++;
    }
  QString Text(tr("Add a new %1...").arg(CollectionName->text()));
  this->ColorIDCollectionComboBox->setColorDialogEnabled(true,Text.toStdString());
  this->ColorIDCollectionComboBox->show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::SetColorTraceComboBox()
{
  ColorTraceComboBox = new QtColorComboBox;
  //ColorTraceComboBox->setColorDialogEnabled(true);
  ColorTraceComboBox->setCreationCollection(false);
  QLabel* ColorLbl = new QLabel(tr("Selected Color:"));
  QLabel* TraceLbl = new QLabel(tr("Current Trace: "));
  TraceName = new QLabel(tr("contour"));

  QHBoxLayout* HLayoutForTrace = new QHBoxLayout;
  HLayoutForTrace->addWidget(TraceLbl);
  HLayoutForTrace->addWidget(TraceName);

  HLayoutForColor = new QHBoxLayout;
  HLayoutForColor->addWidget(ColorLbl);
  HLayoutForColor->addWidget(ColorTraceComboBox);

  this->verticalLayout_2->addLayout(HLayoutForColor);
  this->verticalLayout_2->addLayout(HLayoutForTrace);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::SetColorIDCollectionComboBox()
{
  ColorIDCollectionComboBox = new QtColorComboBox;
  //ColorIDCollectionComboBox->setColorDialogEnabled(true);
  ColorIDCollectionComboBox->setCreationCollection(true);
  QLabel* CollectionLbl = new QLabel(tr("Current Collection: "));
  CollectionName = new QLabel (tr("mesh"));

  /// \todo make the names change with the interaction
  QHBoxLayout* HLayoutForCollection = new QHBoxLayout;
  HLayoutForCollection->addWidget(CollectionLbl);
  HLayoutForCollection->addWidget(CollectionName);
  HLayoutForCollection->addWidget(ColorIDCollectionComboBox);
  this->verticalLayout_2->addLayout(HLayoutForCollection);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoVisualizationDockWidget::SetEnableTraceCollectionColorBoxes(bool Enable)
{
  this->ColorIDCollectionComboBox->setEnabled(Enable);
  this->ColorTraceComboBox->setEnabled(Enable);
}
