/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMorphologicalWatershedFromMarkersImageFilter2.txx,v $
  Language:  C++
  Date:      $Date: 2008-02-07 15:07:57 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMorphologicalWatershedFromMarkersImageFilter2_txx
#define __itkMorphologicalWatershedFromMarkersImageFilter2_txx

#include <algorithm>
#include <queue>
#include <list>
#include "itkMorphologicalWatershedFromMarkersImageFilter2.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"
#include "itkSize.h"
#include "itkConnectedComponentAlgorithm.h"
// #include "itkFillSides.h"

namespace itk
{
template< class TInputImage, class TLabelImage >
MorphologicalWatershedFromMarkersImageFilter2< TInputImage, TLabelImage >
::MorphologicalWatershedFromMarkersImageFilter2()
{
  this->SetNumberOfRequiredInputs(2);
  m_FullyConnected = false;
  m_MarkWatershedLine = true;

  m_ForegroundImg = 0;
}

template< class TInputImage, class TLabelImage >
void
MorphologicalWatershedFromMarkersImageFilter2< TInputImage, TLabelImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the inputs
  LabelImagePointer markerPtr =
    const_cast< LabelImageType * >( this->GetMarkerImage() );

  InputImagePointer inputPtr =
    const_cast< InputImageType * >( this->GetInput() );

  if ( !markerPtr || !inputPtr )
    {
    return;
    }

  // We need to
  // configure the inputs such that all the data is available.
  //
  markerPtr->SetRequestedRegion( markerPtr->GetLargestPossibleRegion() );
  inputPtr->SetRequestedRegion( inputPtr->GetLargestPossibleRegion() );
}

template< class TInputImage, class TLabelImage >
void
MorphologicalWatershedFromMarkersImageFilter2< TInputImage, TLabelImage >
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion(
    this->GetOutput()->GetLargestPossibleRegion() );
}

template< class TInputImage, class TLabelImage >
void
MorphologicalWatershedFromMarkersImageFilter2< TInputImage, TLabelImage >
::GenerateData()
{
  // there is 2 possible cases: with or without watershed lines.
  // the algorithm with watershed lines is from Meyer
  // the algorithm without watershed lines is from beucher
  // The 2 algorithms are very similar and so are integrated in the same filter.

  //---------------------------------------------------------------------------
  // declare the vars common to the 2 algorithms: constants, iterators,
  // hierarchical queue, progress reporter, and status image
  // also allocate output images and verify preconditions
  //---------------------------------------------------------------------------

  // the label used to find background in the marker image
  static const LabelImagePixelType bgLabel =
    NumericTraits< LabelImagePixelType >::Zero;
  // the label used to mark the watershed line in the output image
  static const LabelImagePixelType wsLabel =
    NumericTraits< LabelImagePixelType >::Zero;

  this->AllocateOutputs();

  if ( !m_ForegroundImg )
    {
    m_ForegroundImg = LabelImageType::New();
    m_ForegroundImg->CopyInformation( this->GetOutput() );
    m_ForegroundImg->SetRegions( this->GetOutput()->GetLargestPossibleRegion() );
    m_ForegroundImg->Allocate();
    m_ForegroundImg->FillBuffer(1);
    }

  LabelImageConstPointer markerImage = this->GetMarkerImage();
  InputImageConstPointer inputImage = this->GetInput();
  LabelImagePointer      outputImage = this->GetOutput();

  // Set up the progress reporter
  // we can't found the exact number of pixel to process in the 2nd pass, so we
  // use the maximum number possible.
  ProgressReporter
  progress(this, 0, markerImage->GetRequestedRegion().GetNumberOfPixels() *2);

  // mask and marker must have the same size
  if ( markerImage->GetRequestedRegion().GetSize() != inputImage->GetRequestedRegion().GetSize() )
    {
    itkExceptionMacro(<< "Marker and input must have the same size.");
    }

  // FAH (in french: File d'Attente Hierarchique)
  typedef std::queue< IndexType >                    QueueType;
  typedef std::map< InputImagePixelType, QueueType > MapType;
  MapType fah;

  // the radius which will be used for all the shaped iterators
  Size< ImageDimension > radius;
  radius.Fill(1);

  // iterator for the marker image
  typedef ConstShapedNeighborhoodIterator< LabelImageType > MarkerIteratorType;
  typename MarkerIteratorType::ConstIterator nmIt;
  MarkerIteratorType
  markerIt( radius, markerImage, markerImage->GetRequestedRegion() );

  typedef ShapedNeighborhoodIterator< LabelImageType > ForegroundIteratorType;
  typename ForegroundIteratorType::Iterator fgIt;
  ForegroundIteratorType
  foregroundIt( radius, m_ForegroundImg, m_ForegroundImg->GetRequestedRegion() );

  // add a boundary constant to avoid adding pixels on the border in the fah
  ConstantBoundaryCondition< LabelImageType > lcbc;
  lcbc.SetConstant( NumericTraits< LabelImagePixelType >::max() );
  markerIt.OverrideBoundaryCondition(&lcbc);
  setConnectivity(&markerIt, m_FullyConnected);
  foregroundIt.OverrideBoundaryCondition(&lcbc);
  setConnectivity(&foregroundIt, m_FullyConnected);

  // iterator for the input image
  typedef ConstShapedNeighborhoodIterator< InputImageType > InputIteratorType;
  InputIteratorType
  inputIt( radius, inputImage, inputImage->GetRequestedRegion() );

  typename InputIteratorType::ConstIterator niIt;
  setConnectivity(&inputIt, m_FullyConnected);

  // iterator for the output image
  typedef ShapedNeighborhoodIterator< LabelImageType > OutputIteratorType;
  typedef typename OutputIteratorType::OffsetType      OffsetType;
  typename OutputIteratorType::Iterator noIt;
  OutputIteratorType
  outputIt( radius, outputImage, outputImage->GetRequestedRegion() );

  setConnectivity(&outputIt, m_FullyConnected);

  //---------------------------------------------------------------------------
  // Meyer's algorithm
  //---------------------------------------------------------------------------
  if ( m_MarkWatershedLine )
    {
    // first stage:
    //  - set markers pixels to already processed status
    //  - copy markers pixels to output image
    //  - init FAH with indexes of background pixels with marker pixel(s) in
    //    their neighborhood

    ConstantBoundaryCondition< LabelImageType > lcbc2;
    // outside pixel are watershed so they won't be use to find real watershed
    // pixels
    lcbc2.SetConstant(wsLabel);
    outputIt.OverrideBoundaryCondition(&lcbc2);

    // create a temporary image to store the state of each pixel (processed or
    // not)
    typedef Image< bool, ImageDimension > StatusImageType;
    typename StatusImageType::Pointer statusImage = StatusImageType::New();
    statusImage->SetRegions( markerImage->GetLargestPossibleRegion() );
    statusImage->Allocate();

    // iterator for the status image
    typedef ShapedNeighborhoodIterator< StatusImageType > StatusIteratorType;
    typename StatusIteratorType::Iterator nsIt;
    StatusIteratorType
    statusIt( radius, statusImage, outputImage->GetRequestedRegion() );

    ConstantBoundaryCondition< StatusImageType > bcbc;
    bcbc.SetConstant(true);    // outside pixel are already processed
    statusIt.OverrideBoundaryCondition(&bcbc);
    setConnectivity(&statusIt, m_FullyConnected);

    // the status image must be initialized before the first stage. In the
    // first stage, the set to true are the neighbors of the marker (and the
    // marker) so it's difficult (impossible ?) to init the status image at
    // the same time
    // the overhead should be small
    statusImage->FillBuffer(false);

    for ( markerIt.GoToBegin(), statusIt.GoToBegin(), outputIt.GoToBegin(), inputIt.GoToBegin();
          !markerIt.IsAtEnd();
          ++markerIt, ++outputIt )
      {
      LabelImagePixelType markerPixel = markerIt.GetCenterPixel();
      if ( markerPixel != bgLabel )
        {
        IndexType idx = markerIt.GetIndex();

        // move the iterators to the right place
        OffsetType shift = idx - statusIt.GetIndex();
        statusIt += shift;
        inputIt += shift;

        // this pixel belongs to a marker
        // mark it as already processed
        statusIt.SetCenterPixel(true);
        // copy it to the output image
        outputIt.SetCenterPixel(markerPixel);
        // and increase progress because this pixel will not be used in the
        // flooding stage.
        progress.CompletedPixel();

        // search the background pixels in the neighborhood
        for ( nmIt = markerIt.Begin(), nsIt = statusIt.Begin(), niIt = inputIt.Begin();
              nmIt != markerIt.End();
              nmIt++, nsIt++, niIt++ )
          {
          if ( !nsIt.Get() && nmIt.Get() == bgLabel )
            {
            // this neighbor is a background pixel and is not already
            // processed; add its index to fah
            fah[niIt.Get()].push( markerIt.GetIndex()
                                  + nmIt.GetNeighborhoodOffset() );
            // mark it as already in the fah to avoid adding it several times
            nsIt.Set(true);
            }
          }
        }
      else
        {
        // Some pixels may be never processed so, by default, non marked pixels
        // must be marked as watershed
        outputIt.SetCenterPixel(wsLabel);
        }
      // one more pixel done in the init stage
      progress.CompletedPixel();
      }
    // fill the borders of the status image with "true"
    //FillSides<StatusImageType>(statusImage, true);
    // Now disable the boundary checks
    //outputIt.NeedToUseBoundaryConditionOff();
    //statusIt.NeedToUseBoundaryConditionOff();
    //inputIt.NeedToUseBoundaryConditionOff();
    // end of init stage

    // flooding
    // init all the iterators
    outputIt.GoToBegin();
    statusIt.GoToBegin();
    inputIt.GoToBegin();

    // and start flooding
    while ( !fah.empty() )
      {
      // store the current vars
      InputImagePixelType currentValue = fah.begin()->first;
      QueueType           currentQueue = fah.begin()->second;
      // and remove them from the fah
      fah.erase( fah.begin() );

      while ( !currentQueue.empty() )
        {
        IndexType idx = currentQueue.front();
        currentQueue.pop();

        // move the iterators to the right place
        OffsetType shift = idx - outputIt.GetIndex();
        outputIt += shift;
        statusIt += shift;
        inputIt += shift;

        // iterate over the neighbors. If there is only one marker value, give
        // that value to the pixel, else keep it as is (watershed line)
        LabelImagePixelType marker = wsLabel;
        bool                collision = false;
        for ( noIt = outputIt.Begin(); noIt != outputIt.End(); noIt++ )
          {
          LabelImagePixelType o = noIt.Get();
          if ( o != wsLabel )
            {
            if ( marker != wsLabel && o != marker )
              {
              collision = true;
              break;
              }
            else
              {
              marker = o;
              }
            }
          }
        if ( !collision )
          {
          // set the marker value
          outputIt.SetCenterPixel(marker);
          // and propagate to the neighbors
          for ( niIt = inputIt.Begin(), nsIt = statusIt.Begin();
                niIt != inputIt.End();
                niIt++, nsIt++ )
            {
            if ( !nsIt.Get() )
              {
              // the pixel is not yet processed. add it to the fah
              InputImagePixelType GrayVal = niIt.Get();
              if ( GrayVal <= currentValue )
                {
                currentQueue.push( inputIt.GetIndex()
                                   + niIt.GetNeighborhoodOffset() );
                }
              else
                {
                fah[GrayVal].push( inputIt.GetIndex()
                                   + niIt.GetNeighborhoodOffset() );
                }
              // mark it as already in the fah
              nsIt.Set(true);
              }
            }
          }
        // one more pixel in the flooding stage
        progress.CompletedPixel();
        }
      }
    }

  //---------------------------------------------------------------------------
  // Beucher's algorithm
  //---------------------------------------------------------------------------
  else
    {
    // first stage:
    //  - copy markers pixels to output image
    //  - init FAH with indexes of pixels with background pixel in their
    //    neighborhood

    ConstantBoundaryCondition< LabelImageType > lcbc2;
    // outside pixel are watershed so they won't be use to find real watershed
    // pixels
    lcbc2.SetConstant( NumericTraits< LabelImagePixelType >::max() );
    outputIt.OverrideBoundaryCondition(&lcbc2);

    foregroundIt.GoToBegin();
    for ( markerIt.GoToBegin(), outputIt.GoToBegin(), inputIt.GoToBegin();
          !markerIt.IsAtEnd();
          ++markerIt, ++outputIt )
      {
      LabelImagePixelType markerPixel = markerIt.GetCenterPixel();
      LabelImagePixelType fgPixel = foregroundIt.GetCenterPixel();
      if ( ( markerPixel != bgLabel ) && ( fgPixel > 0 ) )
        {
        IndexType  idx = markerIt.GetIndex();
        OffsetType shift = idx - inputIt.GetIndex();
        inputIt += shift;

        // this pixels belongs to a marker
        // copy it to the output image
        outputIt.SetCenterPixel(markerPixel);
        // search if it has background pixel in its neighborhood
        bool haveBgNeighbor = false;

        fgIt = foregroundIt.Begin();
        for ( nmIt = markerIt.Begin(); nmIt != markerIt.End(); nmIt++ )
          {
          // check for the m_Foreground pixel only
          if ( ( nmIt.Get() == bgLabel ) && ( fgIt.Get() > 0 ) )
            {
            haveBgNeighbor = true;
            break;
            }
          fgIt++;
          }
        if ( haveBgNeighbor )
          {
          // there is a background pixel in the neighborhood; add to fah
          fah[inputIt.GetCenterPixel()].push( markerIt.GetIndex() );
          }
        else
          {
          // increase progress because this pixel will not be used in the
          // flooding stage.
          progress.CompletedPixel();
          }
        }
      else
        {
        outputIt.SetCenterPixel(wsLabel);
        }
      progress.CompletedPixel();
      ++foregroundIt;
      }
    // end of init stage

    // flooding
    // init all the iterators
    outputIt.GoToBegin();
    inputIt.GoToBegin();
    foregroundIt.GoToBegin();

    // and start flooding
    while ( !fah.empty() )
      {
      // store the current vars
      InputImagePixelType currentValue = fah.begin()->first;
      QueueType           currentQueue = fah.begin()->second;
      // and remove them from the fah
      fah.erase( fah.begin() );

      while ( !currentQueue.empty() )
        {
        IndexType idx = currentQueue.front();
        currentQueue.pop();

        // move the iterators to the right place
        OffsetType shift = idx - outputIt.GetIndex();
        outputIt += shift;
        inputIt += shift;
        foregroundIt += shift;

        LabelImagePixelType currentMarker = outputIt.GetCenterPixel();
        // get the current value of the pixel
        // iterate over neighbors to propagate the marker
        fgIt = foregroundIt.Begin();
        for ( noIt = outputIt.Begin(), niIt = inputIt.Begin();
              noIt != outputIt.End();
              noIt++, niIt++, fgIt++ )
          {
          if ( ( noIt.Get() == wsLabel ) && ( fgIt.Get() ) )
            {
            // the pixel is not yet processed. It can be labeled with the
            // current label
            noIt.Set(currentMarker);
            InputImagePixelType GrayVal = niIt.Get();
            if ( GrayVal <= currentValue )
              {
              currentQueue.push( inputIt.GetIndex()
                                 + noIt.GetNeighborhoodOffset() );
              }
            else
              {
              fah[GrayVal].push( inputIt.GetIndex()
                                 + noIt.GetNeighborhoodOffset() );
              }
            progress.CompletedPixel();
            }
          }
        }
      }
    }
}

template< class TInputImage, class TLabelImage >
void
MorphologicalWatershedFromMarkersImageFilter2< TInputImage, TLabelImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "FullyConnected: "  << m_FullyConnected << std::endl;
  os << indent << "MarkWatershedLine: "  << m_MarkWatershedLine << std::endl;
}
} // end namespace itk
#endif
