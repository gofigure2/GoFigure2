/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "QGoTabImageViewNDBase.h"
#include "QGoVisualizationDockWidget.h"

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