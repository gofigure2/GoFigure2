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

#ifndef __itkCellPreprocess_txx
#define __itkCellPreprocess_txx

#include "itkCellPreprocess.h"

namespace itk
{
  template < class TInputImage,class TOutputImage >
  CellPreprocess< TInputImage,TOutputImage >
  ::CellPreprocess() : m_LargestCellRadius ( 4.0 )
  {
    m_MembraneData = false;
    this->Superclass::SetNumberOfRequiredInputs ( 1 );
    this->Superclass::SetNumberOfRequiredOutputs ( 1 );

    this->Superclass::SetNthOutput ( 0,TOutputImage::New() );
  }

  template < class TInputImage,class TOutputImage >
  void
  CellPreprocess< TInputImage,TOutputImage >::
  GenerateData()
  {
    InputCastPointer m_CastInput = InputCastType::New();
    m_CastInput->SetInput ( this->GetInput() );
    m_CastInput->Update();

    ImageSpacingType spacing = this->GetInput()->GetSpacing();
    ImageSizeType radius;
    for ( unsigned int j = 0; j < ImageDimension; j++ )
      {
      radius[j] = static_cast<ImageSizeValueType> (
                    0.3*m_LargestCellRadius/spacing[j] );
      }

    ImagePointer cellImg;
      {
      MedianFilterPointer m_Median = MedianFilterType::New();
      m_Median = MedianFilterType::New();
      m_Median->SetRadius ( radius );
      m_Median->SetInput ( m_CastInput->GetOutput() );
      m_Median->Update();

      SmoothingFilterPointer m_smooth = SmoothingFilterType::New();
      m_smooth->SetInput ( m_Median->GetOutput() );
      m_smooth->SetNumberOfIterations( 5 );
      m_smooth->SetTimeStep( 0.0125 );
      m_smooth->SetConductanceParameter( 3 );
      m_smooth->UseImageSpacingOn();
      m_smooth->Update();

      if ( m_MembraneData )
        {
        cellImg = m_smooth->GetOutput();
        cellImg->DisconnectPipeline();
        }
      else
        {
        GrayscaleFillholePointer m_fillHole = GrayscaleFillholeFilterType::New();
        m_fillHole->SetInput ( m_smooth->GetOutput() );
        m_fillHole->SetFullyConnected ( 0 );
        m_fillHole->Update();
        cellImg = m_fillHole->GetOutput();
        cellImg->DisconnectPipeline();
        }
      }
    OutputCastPointer m_CastOutput = OutputCastType::New();
    m_CastOutput->SetInput ( cellImg );
    m_CastOutput->GraftOutput ( this->GetOutput() );
    m_CastOutput->Update();

    this->GraftOutput ( m_CastOutput->GetOutput() );
    }

  template < class TInputImage,class TOutputImage >
  void
  CellPreprocess< TInputImage,TOutputImage >::
  PrintSelf ( std::ostream& os, Indent indent ) const
    {
    Superclass::PrintSelf ( os,indent );
    os << indent << "Class Name:        " << GetNameOfClass() << std::endl;
    os << indent << "Largest cell radius: " << GetLargestCellRadius() <<
    std::endl;
    }

  } /* end namespace itk */

#endif

