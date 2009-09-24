#include "QGoTabImageView2D.h"

#include "QGoLUTDialog.h"

#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"

#include "QGoImageFilterPluginBase.h"

#include <QMenu>
#include <QColorDialog>

//--------------------------------------------------------------------------
QGoTabImageView2D::QGoTabImageView2D( QWidget* parent )
{
  setupUi( this );

  m_ViewMenu = new QMenu( tr( "&View" ) );
  QAction* LookupTableAction = new QAction( tr( "Lookup Table" ), this );
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table" ) );

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
    this, SLOT( ChangeLookupTable() ) );

  m_ViewMenu->addAction( LookupTableAction );

  QAction* ScalarBarAction = new QAction( tr( "Display Scalar Bar" ), this );
  ScalarBarAction->setCheckable( true );
  m_ViewMenu->addAction( ScalarBarAction );

  QObject::connect( ScalarBarAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ShowScalarBar( bool ) ) );

  m_PropertiesMenu = new QMenu( "&Properties" );
  QAction* BackgroundColorAction = new QAction( tr("Background Color"), this );
  m_PropertiesMenu->addAction( BackgroundColorAction );

  QObject::connect( BackgroundColorAction, SIGNAL( triggered() ),
    this, SLOT( ChangeBackgroundColor() ) );

  m_FilteringMenu = new QMenu( tr( "&Filtering" ) );
  m_FilteringMenu->setDisabled( true );

  m_SegmentationMenu = new QMenu( tr( "&Segmentation" ) );
  m_SegmentationMenu->setDisabled( true );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView2D::~QGoTabImageView2D()
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
    }

  m_ImageView = new QGoImageView2D( this );

  m_LayOut = new QHBoxLayout( parent );
  m_LayOut->addWidget( m_ImageView  );

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
void QGoTabImageView2D::SetImage( vtkImageData* iImage )
{
  m_ImageView->SetImage( iImage );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::Update( )
{
  m_ImageView->Update();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QMenu* > QGoTabImageView2D::Menus()
{
  std::vector< QMenu* > oMenuVector;
  oMenuVector.push_back( m_ViewMenu );
  oMenuVector.push_back( m_PropertiesMenu );
  oMenuVector.push_back( m_FilteringMenu );
  oMenuVector.push_back( m_SegmentationMenu );

  return oMenuVector;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::ChangeBackgroundColor()
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  QColor initcolor( static_cast< int >( 255. * r ),
    static_cast< int >( 255. * g ),
    static_cast< int >( 255. * b ) );

  QColor color = QColorDialog::getColor( initcolor, this,
    tr( "Choose Background Color" ) );

  m_ImageView->SetBackgroundColor( color );
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
std::list< QToolBar* > QGoTabImageView2D::ToolBar()
{
  std::list< QToolBar* > oList;
  return oList;
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
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::ReadSettings()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::PopulateMenus( QObject *plugin )
{
  QGoImageFilterPluginBase* filter =
    qobject_cast< QGoImageFilterPluginBase* >( plugin );
  if( filter )
    {
    this->AddToMenu( plugin, QStringList( "yo" ),
      m_FilteringMenu, SLOT( temp() ), 0 );
    }
 }
//--------------------------------------------------------------------------