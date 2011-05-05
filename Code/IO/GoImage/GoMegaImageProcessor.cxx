
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

#include "GoMegaImageProcessor.h"

//--------------------------------------------------------------------------
void
GoMegaImageProcessor::
setReader(itk::MegaCaptureReader::Pointer iReader)
{
  m_MegaImageReader = iReader;

  // update general parameters
  //--------------------
  // todo Nicolas- Create a method for that...
  m_BoundsTime = new unsigned int[2];
  m_BoundsTime[0] = m_MegaImageReader->GetMinTimePoint();
  m_BoundsTime[1] = m_MegaImageReader->GetMaxTimePoint();
  m_BoundsChannel = new unsigned int[2];
  m_BoundsChannel[0] = m_MegaImageReader->GetMinChannel();
  m_BoundsChannel[1] = m_MegaImageReader->GetMaxChannel();
  m_Extent = new int[6];
  (m_MegaImageReader->GetImage(m_BoundsChannel[0], m_BoundsTime[0]))->
      GetExtent(m_Extent);

  m_TimeInterval = m_MegaImageReader->GetTimeInterval();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoMegaImageProcessor::
setTimePoint(const unsigned int& iTime)
{
  //check if time point exists
  if(iTime >= m_BoundsTime[0] && iTime <= m_BoundsTime[1])
    {
    m_MegaImageReader->SetTimePoint(iTime);
    m_MegaImageReader->Update();
    m_MegaImageContainer.clear();
    }
  else
    {
    return;
    }

  // update the container
  // Get Number of channels from reader
  int numberOfChannels = getNumberOfChannels();

  while(numberOfChannels>0)
    {
    --numberOfChannels;

    // Get useful information from the reader
    // Nicolas Get Image or get output...?
    vtkSmartPointer<vtkImageData> image =
        m_MegaImageReader->GetOutput(numberOfChannels);

    // Get Color
    std::vector<std::vector<int> >channelColor =
        m_MegaImageReader->GetChannelColor();


    double random1 = channelColor[numberOfChannels][0];
    double value1 = random1/255;

    double random2 = channelColor[numberOfChannels][1];
    double value2 = random2/255;

    double random3 = channelColor[numberOfChannels][2];
    double value3 = random3/255;

    std::vector<double> color;
    color.push_back(value1);
    color.push_back(value2);
    color.push_back(value3);
    color.push_back(1);

    // Create LUT
    vtkSmartPointer<vtkLookupTable> lut = createLUT(color[0],
                                                    color[1],
                                                    color[2],
                                                    color[3],
                                                    image->GetScalarRange());

    // Update the MegaImageStructure
    // image, LUT, channel, time point
    m_MegaImageContainer.insert(GoMegaImageStructure(iTime,
                                                   numberOfChannels,
                                                   lut,
                                                   image,
                                                   color));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoMegaImageProcessor::
setDoppler(const unsigned int& iChannel, const unsigned int& iTime,
           const unsigned int& iPrevious)
{
  //to optimize doppler view later on
  (void) iPrevious;

  //check if time point exists
  if(iTime >= m_BoundsTime[0] && iTime <= m_BoundsTime[1])
    {
    m_MegaImageReader->SetTimePoint(iTime);
    m_MegaImageReader->Update();
    m_MegaImageContainer.clear();
    }
  else
    {
    return;
    }

  unsigned int* time = getBoundsTime();

  int t0 = iTime - m_DopplerStep;
  int t1 = iTime;
  int t2 = iTime + m_DopplerStep;

  // special case if we are at the borders
  if ( t0 < time[0] )
    {
    t0 = -1;
    }

  if ( t2 > time[1] )
    {
    t2 = -1;
    }

  int dopplerTime[3] = {t0, t1, t2};
  unsigned int channel = iChannel;

  for(unsigned int i=0; i<3; ++i)
    {
      if(dopplerTime[i] >= 0)
      {
    // Get useful information from the reader
    // Nicolas Get Image or get output...?
    vtkSmartPointer<vtkImageData> image =
        m_MegaImageReader->GetImage(iChannel,dopplerTime[i]);

    // color from red to blue
    std::vector<double> color;
    color.push_back(0.0);
    color.push_back(0.0);
    color.push_back(0.0);
    color.push_back(0.0);

    color[i] = 1.0;

    // Create LUT
    vtkSmartPointer<vtkLookupTable> lut = createLUT(color[0],
                                                    color[1],
                                                    color[2],
                                                    color[3],
                                                    image->GetScalarRange());

    // Update the MegaImageStructure
    // image, LUT, channel, time point
    m_MegaImageContainer.insert(GoMegaImageStructure(dopplerTime[i],
                                                     channel,
                                                     lut,
                                                     image,
                                                     color));
      }
    }
/*
  vtkSmartPointer< vtkImageAppendComponents > append_filter =
    vtkSmartPointer< vtkImageAppendComponents >::New();

  // if step != 1 and we have previous and next time point loaded
  if( m_DopplerStep != 1 || iPreviousT == 0 )
    {
    // resize internal image
    // clean the vector since is is a vector of smartpointers
    m_InternalImages.resize(3, NULL);

    vtkSmartPointer< vtkImageData > i0 = vtkSmartPointer< vtkImageData >::New();
    i0->ShallowCopy( m_MegaCaptureReader->GetImage(iChannel, t0) );
    m_InternalImages[0] = i0;
    append_filter->AddInput(m_InternalImages[0]);

    vtkSmartPointer< vtkImageData > i1 = vtkSmartPointer< vtkImageData >::New();
    i1->ShallowCopy( m_MegaCaptureReader->GetImage(iChannel, t1) );
    m_InternalImages[1] = i1;
    append_filter->AddInput(m_InternalImages[1]);

    vtkSmartPointer< vtkImageData > i2 = vtkSmartPointer< vtkImageData >::New();
    i2->ShallowCopy( m_MegaCaptureReader->GetImage(iChannel, t2) );
    m_InternalImages[2] = i2;
    append_filter->AddInput(m_InternalImages[2]);
    }
  else
    {
    // if we go FORWARD and step == 1
    if( iPreviousT < m_TCoord)
      {
      // assume we imcrease t point all the time for testing
      vtkSmartPointer< vtkImageData > i0 = vtkSmartPointer< vtkImageData >::New();
      i0->ShallowCopy(m_InternalImages[1]);
      // clean smartpointer
      m_InternalImages[0] = NULL;
      m_InternalImages[0] = i0;
      append_filter->AddInput(m_InternalImages[0]);

      vtkSmartPointer< vtkImageData > i1 = vtkSmartPointer< vtkImageData >::New();
      i1->ShallowCopy(m_InternalImages[2]);
      // clean smartpointer
      m_InternalImages[1] = NULL;
      m_InternalImages[1] = i1;
      append_filter->AddInput(m_InternalImages[1]);

      vtkSmartPointer< vtkImageData > i2 = vtkSmartPointer< vtkImageData >::New();
      i2->ShallowCopy( m_MegaCaptureReader->GetImage(iChannel, t2) );
      // clean smartpointer
      m_InternalImages[2] = NULL;
      m_InternalImages[2] = i2;
      append_filter->AddInput(m_InternalImages[2]);
      }
    // if we go BACKWARD and step == 1
    else
      {
      vtkSmartPointer< vtkImageData > i2 = vtkSmartPointer< vtkImageData >::New();
      i2->ShallowCopy(m_InternalImages[1]);
      // clean smartpointer
      m_InternalImages[2] = NULL;
      m_InternalImages[2] = i2;
      append_filter->AddInput(m_InternalImages[2]);

      vtkSmartPointer< vtkImageData > i1 = vtkSmartPointer< vtkImageData >::New();
      i1->ShallowCopy(m_InternalImages[0]);
      // clean smartpointer
      m_InternalImages[1] = NULL;
      m_InternalImages[1] = i1;
      append_filter->AddInput(m_InternalImages[1]);

      vtkSmartPointer< vtkImageData > i0 = vtkSmartPointer< vtkImageData >::New();
      i0->ShallowCopy( m_MegaCaptureReader->GetImage(iChannel, t0) );
      // clean smartpointer
      m_InternalImages[0] = NULL;
      m_InternalImages[0] = i0;
      append_filter->AddInput(m_InternalImages[0]);
      }
    }
  */
}
//--------------------------------------------------------------------------
