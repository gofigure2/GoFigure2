#include "QGoTabImageViewNDBase.h"

#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkImageExtractComponents.h"

//--------------------------------------------------------------------------
QGoTabImageViewNDBase::
QGoTabImageViewNDBase( QWidget* iParent ) :
  QGoTabImageViewElementBase( iParent ),
  m_Image( 0 )
{
  
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageViewNDBase::
~QGoTabImageViewNDBase( )
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Set the image to be displaid
 * \param[in] iImage
 */
void QGoTabImageViewNDBase::
SetImage( vtkImageData* iImage )
{
  if( !iImage )
    {
    std::cerr <<"QGoTabImageViewNDBase::SetImage( 0x0 )" <<std::endl;
    return;
    }

  m_Image = iImage;

  int n = m_Image->GetNumberOfScalarComponents();
  this->m_VisuDockWidget->SetNumberOfChannels( n );

  if( n != 1 )
    {
    if( ( n == 3 ) || ( n == 4 ) )
      {
      this->m_VisuDockWidget->SetChannel( 0, tr( "Red" ) );
      this->m_VisuDockWidget->SetChannel( 1, tr( "Green" ) );
      this->m_VisuDockWidget->SetChannel( 2, tr( "Blue" ) );

      if( n == 4 )
        {
        this->m_VisuDockWidget->SetChannel( 3, tr( "Alpha" ) );
        }
      }
    else
      {
      for( int i = 0; i < n; i++ )
        {
        this->m_VisuDockWidget->SetChannel( i );
        }
      }
    }

  int extent[6];
  m_Image->GetExtent( extent );

  this->SetImageToImageViewer( m_Image );

  this->m_VisuDockWidget->SetXMinimumAndMaximum( extent[0], extent[1] );
  this->m_VisuDockWidget->SetXSlice( (extent[0]+extent[1])/2 );

  this->m_VisuDockWidget->SetYMinimumAndMaximum( extent[2], extent[3] );
  this->m_VisuDockWidget->SetYSlice( (extent[2]+extent[3])/2 );

  this->m_VisuDockWidget->SetZMinimumAndMaximum( extent[4], extent[5] );
  this->m_VisuDockWidget->SetZSlice( (extent[4]+extent[5])/2 );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iChecked
 */
void QGoTabImageViewNDBase::
ShowAllChannels( bool iChecked )
{
  if( iChecked )
    {
    this->SetImageToImageViewer( m_Image );
    }
  else
    {
    int ch = this->m_VisuDockWidget->GetCurrentChannel();
    if( ch != -1 )
      {
      vtkSmartPointer< vtkImageExtractComponents > extract =
        vtkSmartPointer< vtkImageExtractComponents >::New();
      extract->SetInput( m_Image );
      extract->SetComponents( ch );
      extract->Update();

      this->SetImageToImageViewer( extract->GetOutput() );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iChannel
 */
void QGoTabImageViewNDBase::
ShowOneChannel( int iChannel )
{
  if( m_Image )
    {
    if( iChannel != -1 )
      {
      vtkSmartPointer< vtkImageExtractComponents > extract =
        vtkSmartPointer< vtkImageExtractComponents >::New();
      extract->SetInput( m_Image );
      extract->SetComponents( iChannel );
      extract->Update();

      this->SetImageToImageViewer( extract->GetOutput() );
      }
    }
}
//--------------------------------------------------------------------------