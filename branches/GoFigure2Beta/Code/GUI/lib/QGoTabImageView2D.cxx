#include "QGoTabImageView2D.h"

#include "QGoLUTDialog.h"

#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"
#include "vtkImageExtractComponents.h"

#include "QGoImageFilterPluginBase.h"

#include <QMenu>
#include <QColorDialog>
#include <QMessageBox>
#include <QSettings>
#include <QVariant>

//--------------------------------------------------------------------------
QGoTabImageView2D::QGoTabImageView2D( QWidget* parent )
{
  m_Image = 0;
  setupUi( this );

  m_VisuDockWidget = new QGoVisualizationDockWidget( this, 2 );
  m_VisuDockWidget->resize( 120, 300 );

  QAction* LookupTableAction = new QAction( tr( "Lookup Table" ), this );
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table" ) );

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
    this, SLOT( ChangeLookupTable() ) );

  m_ViewActions.push_back( LookupTableAction );

  QAction* ScalarBarAction = new QAction( tr( "Display Scalar Bar" ), this );
  ScalarBarAction->setCheckable( true );
  m_ViewActions.push_back( ScalarBarAction );

  QObject::connect( ScalarBarAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ShowScalarBar( bool ) ) );

  QAction* BackgroundColorAction = new QAction( tr("Background Color"), this );
  m_ViewActions.push_back( BackgroundColorAction );

  QObject::connect( BackgroundColorAction, SIGNAL( triggered() ),
    this, SLOT( ChangeBackgroundColor() ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( ShowAllChannelsChanged( bool ) ),
    this, SLOT( ShowAllChannels( bool ) ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( ShowOneChannelChanged( int ) ),
    this, SLOT( ShowOneChannel( int ) ) );

  ReadSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView2D::~QGoTabImageView2D()
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigure::TabDimensionType QGoTabImageView2D::GetTabDimensionType( ) const
{
  return GoFigure::TWO_D;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
    }

  m_ImageView = new QGoImageView2D( this );
  m_ImageView->SetBackgroundColor( m_BackgroundColor );

  this->m_LayOut = new QHBoxLayout( parent );
  this->m_LayOut->addWidget( m_ImageView  );

  retranslateUi(parent);

  QMetaObject::connectSlotsByName(parent);
} // setupUi
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( tr( "QGoTabImageView2D" ) );
  Q_UNUSED(parent);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::Update( )
{
  m_ImageView->Update();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabImageView2D::ViewActions()
{
  return m_ViewActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::SetBackgroundColorToImageViewer()
{
  m_ImageView->SetBackgroundColor( this->m_BackgroundColor );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::GetBackgroundColorFromImageViewer( )
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  this->m_BackgroundColor.setRgbF( r, g, b );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::ChangeLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
    tr( "Choose one look-up table") ) );
  m_ImageView->SetLookupTable( lut );
  lut->Delete();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::ShowScalarBar( const bool& iShow )
{
  m_ImageView->ShowScalarBar( iShow );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QDockWidget* > QGoTabImageView2D::DockWidget()
{
  std::list< QDockWidget* > oList;
  return oList;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QWidget* > QGoTabImageView2D::AdditionalWidget()
{
  std::list< QWidget* > oList;
  return oList;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::WriteSettings()
{
  QGoTabImageViewElementBase::WriteSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::ReadSettings()
{
  QGoTabImageViewElementBase::ReadSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::SetImageToImageViewer( vtkImageData* iImage )
{
  m_ImageView->SetImage( iImage );
  m_ImageView->Update();
}
//--------------------------------------------------------------------------
