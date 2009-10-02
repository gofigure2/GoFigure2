#include "QGoTabImageView3D.h"

#include "QGoImageView3D.h"

QGoTabImageView3D::QGoTabImageView3D( QWidget* parent )
{
  m_Image = 0;

  setupUi( this );

  QAction* QuadViewAction = new QAction( tr("QuadView"), this );
  m_ViewActions.push_back( QuadViewAction );

  QAction* FullScreenXYAction = new QAction( tr( "Full-Screen XY" ), this );
  m_ViewActions.push_back( FullScreenXYAction );

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

  ReadSettings();
}

QGoTabImageView3D::~QGoTabImageView3D( )
{
}

//--------------------------------------------------------------------------
void QGoTabImageView3D::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
    }

  m_ImageView = new QGoImageView3D( this );
  m_ImageView->SetBackgroundColor( m_BackgroundColor );

  m_LayOut = new QHBoxLayout( parent );
  m_LayOut->addWidget( m_ImageView  );

  retranslateUi(parent);

  QMetaObject::connectSlotsByName(parent);
} // setupUi
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( tr( "QGoTabImageView2D" ) );
  Q_UNUSED(parent);
}
//--------------------------------------------------------------------------

GoFigure::TabDimensionType QGoTabImageView3D::GetTabDimensionType( ) const
{
  return GoFigure::THREE_D;
}

void QGoTabImageView3D::SetImage( vtkImageData* iImage )
{
  m_ImageView->SetImage( iImage );
  m_Image = iImage;
}

void QGoTabImageView3D::Update()
{
  m_ImageView->Update();
}
