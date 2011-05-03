
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

//--------------------------------------------------------------------------
void
GoLSMImageProcessor::
setReader(vtkLSMReader* iReader)
{
  m_LSMReader = iReader;

  std::cout << "init parameters" << std::endl;

  // update general parameters
  //--------------------
  int numberOfTimePoints = m_LSMReader->GetNumberOfTimePoints();
  m_BoundsTime = new unsigned int[2];
  m_BoundsTime[0] = 0;
  m_BoundsTime[1] = numberOfTimePoints -1;

  int numberOfChannels = m_LSMReader->GetNumberOfChannels();
  m_BoundsChannel = new unsigned int[2];
  m_BoundsChannel[0] = 0;
  m_BoundsChannel[1] = numberOfChannels -1;

  m_Extent = new int[6];
  (m_LSMReader->GetTimePointOutput(m_BoundsChannel[0], m_BoundsTime[0]))->
      GetExtent(m_Extent);

  m_TimeInterval = m_LSMReader->GetTimeInterval();

  //--------------------

  setTimePoint(m_BoundsTime[0]);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoLSMImageProcessor::
setTimePoint(const unsigned int& iTime)
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

  // update the container
  // Get Number of channels from reader
  int numberOfChannels = getNumberOfChannels();

  while(numberOfChannels>0)
    {
    --numberOfChannels;

    // Get useful information from the reader
    // Get Image
    vtkSmartPointer<vtkImageData> image =
        m_LSMReader->GetTimePointOutput(numberOfChannels, iTime);

    // Get Color
    // vtkGetObjectMacro(ChannelColors, vtkIntArray);

    // int vtkLSMReader::GetChannelColorComponent(int ch, int component)
    //std::vector<std::vector<int> >channelColor =
    //    m_MegaImageReader->GetChannelColor();


    double random1 = m_LSMReader->
        GetChannelColorComponent(numberOfChannels, 0);//channelColor[numberOfChannels][0];
    double value1 = random1/255;

    double random2 = m_LSMReader->
        GetChannelColorComponent(numberOfChannels, 1);
    double value2 = random2/255;

    double random3 = m_LSMReader->
        GetChannelColorComponent(numberOfChannels, 2);
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
                                                    color[3]);
    // set up range
    lut->SetRange(image->GetScalarRange());
    lut->Build();

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
GoLSMImageProcessor::
setDoppler(const unsigned int& iChannel, const unsigned int& iTime,
           const unsigned int& iPrevious)
{
/*  //check if time point exists
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
*/
}
//--------------------------------------------------------------------------
