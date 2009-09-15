#include "QGoImageView2D.h"

#include "QVTKWidget.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"
#include "vtkViewImage2DCollection.h"
#include "vtkViewImage2D.h"

//--------------------------------------------------------------------------
QGoImageView2D::QGoImageView2D( QWidget* parent )
{
  m_Image = 0;

  m_VTKEventQtConnector = vtkEventQtSlotConnect::New();

  this->setupUi( this );

  m_Pool = vtkViewImage2DCollection::New();

  vtkViewImage2D* View = vtkViewImage2D::New();
  View->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  View->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  vtkRenderWindow* renwin = m_QVTKWidgetXY->GetRenderWindow( );
  View->SetRenderWindow( renwin );
  View->SetRenderer( renwin->GetRenderers()->GetFirstRenderer() );
  View->SetupInteractor( m_QVTKWidgetXY->GetInteractor() );

  m_Pool->AddItem( View );
  View->Delete();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoImageView2D::~QGoImageView2D()
{
  m_Pool->Delete();
  m_VTKEventQtConnector->Delete();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoImageView2D::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
    }

  m_QVTKWidgetXY = new QVTKWidget( this );
  m_QVTKWidgetXY->resize(800,800);

  m_LayOut = new QHBoxLayout( parent );
  m_LayOut->addWidget( m_QVTKWidgetXY  );

  retranslateUi(parent);

  QMetaObject::connectSlotsByName(parent);
} // setupUi
//--------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoImageView2D::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( tr( "QGoImageView2D" ) );
  Q_UNUSED(parent);
}
//------------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoImageView2D::SetImage( vtkImageData* iImage )
{
  int dim[3];
  iImage->GetDimensions( dim );

  if( ( dim[0] > 1 ) && ( dim[1] > 1 ) && ( dim[2] > 1 ) )
    {
    std::cout <<"void QGoTabImageView2D::SetImage( vtkImageData* iImage )"
      <<std::endl;
    std::cout <<"This widget only display 2D images." <<std::endl;
    return;
    }
  else
    {
    m_Image = iImage;
    }
}
//--------------------------------------------------------------------------
void QGoImageView2D::Update( )
{
  vtkViewImage2D* View = m_Pool->GetItem( 0 );
  View->SetInput( this->m_Image );
  View->GetTextProperty()->SetFontFamilyToArial();
  View->GetTextProperty()->SetFontSize( 20 );

  m_Pool->Initialize();
  m_Pool->InitializeAllObservers();
  m_Pool->SyncSetShowAnnotations( true );
  m_Pool->SyncSetShowScalarBar( false );
  m_Pool->SyncRender();
  m_Pool->SyncReset();
}
//--------------------------------------------------------------------------

