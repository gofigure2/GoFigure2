
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

#include "GoLSMImageProcessor.h"

#include "vtkMath.h"

//--------------------------------------------------------------------------
void
GoLSMImageProcessor::
setReader(vtkLSMReader* iReader)
{
  m_LSMReader = iReader;

  // update general parameters
  //--------------------
  int numberOfTimePoints = m_LSMReader->GetNumberOfTimePoints();
  m_BoundsTime[0] = 0;
  m_BoundsTime[1] = numberOfTimePoints -1;

  int numberOfChannels = m_LSMReader->GetNumberOfChannels();
  m_BoundsChannel[0] = 0;
  m_BoundsChannel[1] = numberOfChannels -1;

  m_TimeInterval = m_LSMReader->GetTimeInterval();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoLSMImageProcessor::
initTimePoint(const unsigned int& iTime)
{
  if( iTime != this->m_CurrentTimePoint )
    {
    //check if time point exists
    if(iTime >= m_BoundsTime[0] && iTime <= m_BoundsTime[1])
      {
      m_MegaImageContainer.clear();
      }
    else
      {
      return;
      }
    this->m_CurrentTimePoint = iTime;

    // update the container
    // Get Number of channels from reader
    int numberOfChannels = this->getNumberOfChannels();

  #ifdef HAS_OPENMP
  #pragma omp for
  #endif
    for( int i = 0; i < numberOfChannels; i++ )
      {
      vtkSmartPointer<vtkLSMReader> reader =
        vtkSmartPointer<vtkLSMReader>::New();
      reader->SetFileName(m_LSMReader->GetFileName());
      reader->SetUpdateChannel( i );
      reader->SetUpdateTimePoint(iTime);
      reader->Update();

       // get image
      vtkSmartPointer<vtkImageData> image = reader->GetOutput();

      // capacity of image -> rescale in multichannelmode
      int type = image->GetScalarSize();
      double threshold = pow((double)2, (int)8*type) - 1;
      m_MaxImage = threshold;
      // max pixel in image
      double range = image->GetScalarRange()[1];
      if(m_MaxThreshold < range)
        {
        m_MaxThreshold = range;
        }

      // Get Color
      double random1 = reader->
        GetChannelColorComponent(i, 0);
      double value1 = random1;

      double random2 = reader->
        GetChannelColorComponent(i, 1);
      double value2 = random2;

      double random3 = reader->
        GetChannelColorComponent(i, 2);
      double value3 = random3;

      std::vector<double> color( 4 );
      color[0] = value1;
      color[1] = value2;
      color[2] = value3;
      color[3] = 255;

      // Create LUT
      vtkSmartPointer<vtkLookupTable> lut = createLUT(color[0],
                                                      color[1],
                                                      color[2],
                                                      color[3]);
      // create name...
      std::stringstream channelName;
      channelName << "Channel ";
      channelName << i;

      // Update the MegaImageStructure
      // image, LUT, channel, time point
      m_MegaImageContainer.insert(GoMegaImageStructure(i,
                                                       lut,
                                                       image,
                                                       color,
                                                       true,
                                                       channelName.str()));
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoLSMImageProcessor::
setTimePoint(const unsigned int& iTime)
{
  if( iTime != this->m_CurrentTimePoint )
    {
    //check if time point exists
    if(iTime < m_BoundsTime[0] && iTime <= m_BoundsTime[1])
      {
      return;
      }

    this->m_CurrentTimePoint = iTime;

    // update the container
    // Get Number of channels from reader
    int numberOfChannels = this->getNumberOfChannels();

  #ifdef HAS_OPENMP
  #pragma omp for
  #endif
    for( int i = 0; i < numberOfChannels; i++ )
      {
      vtkSmartPointer<vtkLSMReader> reader =
        vtkSmartPointer<vtkLSMReader>::New();
      reader->SetFileName(m_LSMReader->GetFileName());
      reader->SetUpdateChannel( i );
      reader->SetUpdateTimePoint(iTime);
      reader->Update();

       // get image
      vtkSmartPointer<vtkImageData> image = reader->GetOutput();

      // could iterate on sth else...
      GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
        m_MegaImageContainer.get< Index >().find( i );

      if(it!=m_MegaImageContainer.get< Index >().end())
        {
        m_MegaImageContainer.get< Index >().modify( it , set_image(image) );
        }
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoLSMImageProcessor::
setDoppler(const unsigned int& iTime, const unsigned int& iPrevious)
{
  //to optimize doppler view later on
  (void) iPrevious;

  //check if time point exists
  if(iTime >= m_BoundsTime[0] && iTime <= m_BoundsTime[1])
    {
    }
  else
    {
    return;
    }

  std::vector<int> dopplerTime = getDopplerTime(iTime);

  unsigned int dopplerSize = this->getDopplerSize();

#ifdef HAS_OPENMP
#pragma omp for
#endif
  for(unsigned int i=0; i < dopplerSize; ++i)
    {
    if(dopplerTime[i] >= 0)
      {
      vtkSmartPointer<vtkLSMReader> reader =
          vtkSmartPointer<vtkLSMReader>::New();
      reader->SetFileName(m_LSMReader->GetFileName());
      reader->SetUpdateChannel(m_DopplerChannel);
      reader->SetUpdateTimePoint(dopplerTime[i]);
      reader->Update();

      // get image
      vtkSmartPointer<vtkImageData> image = reader->GetOutput();

      // hue: 0 to 0.7
      double* rgb = vtkMath::HSVToRGB(
            static_cast<double>(i)/static_cast<double>(dopplerSize),1,1);

      // color from red to blue
      std::vector<double> color;
      color.push_back(rgb[0]*255);
      color.push_back(rgb[1]*255);
      color.push_back(rgb[2]*255);
      color.push_back(255);

      // Create LUT
      vtkSmartPointer<vtkLookupTable> lut = createLUT(color[0],
                                                      color[1],
                                                      color[2],
                                                      color[3]);

      // channel name
      std::stringstream channelName;
      //channelName << "t: ";
      channelName << dopplerTime[i];

      // Update the MegaImageStructure
      // image, LUT, channel, time point
      m_MegaImageContainer.insert(GoMegaImageStructure(dopplerTime[i],
                                                       lut,
                                                       image,
                                                       color,
                                                       true,
                                                       channelName.str()));
      }
    }
}
//--------------------------------------------------------------------------
