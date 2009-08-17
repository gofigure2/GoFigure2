#include "QGoImageView2D.h"

#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"

//--------------------------------------------------------------------------
QGoImageView2D::QGoImageView2D( QWidget* parent )
{
  m_Image = 0;

  m_VTKEventQtConnector = vtkEventQtSlotConnect::New();

  this->setupUi( this );

  m_View = vtkViewImage2DWithContourWidget::New();
  m_View->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  m_View->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  vtkRenderWindow* renwin = m_QVTKWidgetXY->GetRenderWindow( );
  m_View->SetRenderWindow( renwin );
  m_View->SetRenderer( renwin->GetRenderers()->GetFirstRenderer() );
  m_View->SetupInteractor( m_QVTKWidgetXY->GetInteractor() );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoImageView2D::~QGoImageView2D()
{
  m_View->Delete();
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

  m_QVTKWidgetXY = new QVTKWidget;
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
void QGoTabImageView2D::SetImage( vtkImageData* iImage )
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
void QGoTabImageView2D::Update( )
{
  m_View->SetInput( this->m_Image );
  m_View->GetTextProperty()->SetFontFamilyToArial();
  m_View->GetTextProperty()->SetFontSize( 20 );

  m_View->Render();
  m_View->Reset();
}
//--------------------------------------------------------------------------

