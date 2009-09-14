#include "QGoTabImageView2D.h"

#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"

#include <QMenu>

//--------------------------------------------------------------------------
QGoTabImageView2D::QGoTabImageView2D( QWidget* parent )
{
  m_ImageView = new QGoImageView2D( this );

  m_LUTDialog = new QGoLUTDialog( this );
  m_LUTDialog->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView2D::~QGoTabImageView2D()
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView2D::SetImage( vtkImageData* iImage )
{
  m_ImageView->SetImage( iImage );
}
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

  QMenu* ViewMenu = new QMenu( tr( "&View" ) );
  QAction* LookupTableAction = new QAction( tr( "Lookup Table" ), this );
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table" ) );

  // Here write the connection
//   QObject::connect( LookupTableAction, SIGNAL( triggered() ),
//     this, SLOT( ChooseLookupTable() ) );

  ViewMenu->addAction( LookupTableAction );

  QAction* ScalarBarAction = new QAction( tr( "Display Scalar Bar" ), this );
  ViewMenu->addAction( ScalarBarAction );

  oMenuVector.push_back( ViewMenu );

  QMenu* PropertiesMenu = new QMenu( "&Properties" );
  QAction* BackgroundColorAction = new QAction( tr("Background Color"), this );
  PropertiesMenu->addAction( BackgroundColorAction );

  oMenuVector.push_back( PropertiesMenu );

  return oMenuVector;
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

