
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

#include "GoImageProcessor.h"

// external library include
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageBlend.h"

// test
#include "vtkImageShiftScale.h"

#include "VisualizePolydataHelper.h"

//--------------------------------------------------------------------------
GoImageProcessor::GoImageProcessor():m_Output(NULL),
  m_BoundsTime(NULL), m_BoundsChannel(NULL), m_Extent(NULL), m_DopplerStep(1),
  m_NumberOfImages(3), m_DopplerMode(false)
{
  m_DopplerTime = new int[3];
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoImageProcessor::GoImageProcessor(const GoImageProcessor & iE):
  m_MegaImageContainer(iE.m_MegaImageContainer), m_Output(iE.m_Output),
  m_BoundsTime(iE.m_BoundsTime), m_BoundsChannel(iE.m_BoundsChannel),
  m_Extent(iE.m_Extent), m_DopplerStep(iE.m_DopplerStep),
  m_NumberOfImages(iE.m_NumberOfImages), m_DopplerTime(iE.m_DopplerTime),
  m_DopplerMode(iE.m_DopplerMode)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoImageProcessor::
~GoImageProcessor()
{
  if(m_BoundsTime)
    {
    delete[] m_BoundsTime;
    m_BoundsTime = NULL;
    }

  if(m_BoundsChannel)
    {
    delete[] m_BoundsChannel;
    m_BoundsChannel = NULL;
    }

  if(m_Extent)
    {
    delete[] m_Extent;
    m_Extent = NULL;
    }

  if(m_DopplerTime)
   {
    delete[] m_DopplerTime;
    m_DopplerTime = NULL;
   }
}
//--------------------------------------------------------------------------
// TODO Nicolas-Range is missing
//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
createLUT(const double& iRed, const double& iGreen, const double& iBlue,
          const double& iAlpha, const double* iRange)
{
  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  double* HSV = vtkMath::RGBToHSV(iRed,iGreen,iBlue);
  lut->SetAlpha(iAlpha);
  lut->SetHueRange(HSV[0], HSV[0]);
  lut->SetSaturationRange(1, 1);
  lut->SetValueRange(0, 1);
  lut->SetRange(const_cast<double*>(iRange));
  lut->Build();
  return lut;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setLookupTable(vtkSmartPointer<vtkLookupTable> iLUT,
                    const unsigned int& iChannel,
                    const unsigned int& iTime)
{
  GoMegaImageStructureMultiIndexContainer::index<Channel>::type::iterator it =
      m_MegaImageContainer.get< Channel >().find(iChannel);

  while(it!=m_MegaImageContainer.get< Channel >().end())
    {
    if(it->Time==iTime)
      {
      m_MegaImageContainer.get< Channel >().modify( it , set_lut(iLUT) );
      break;
      }
    ++it;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
getLookuptable(const unsigned int& iChannel, const unsigned int& iTime) const
{
  GoMegaImageStructureMultiIndexContainer::index<Channel>::type::iterator it =
      m_MegaImageContainer.get< Channel >().find(iChannel);

  while(it!=m_MegaImageContainer.get< Channel >().end())
    {
    if(it->Time==iTime)
      {
      return it->LUT;
      }
    ++it;
    }
  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
colorImage(vtkSmartPointer<vtkImageData> iImage,
           vtkSmartPointer<vtkLookupTable> iLUT)
{
  double* range = iLUT->GetRange();
  std::cout << "range LUT: " << range[0] << " to " << range[1] << std::endl;
  range = iImage->GetScalarRange();
  std::cout << "range image: " << range[0] << " to " << range[1] << std::endl;

  vtkSmartPointer<vtkImageMapToWindowLevelColors> coloredImage =
      vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();
  coloredImage->SetLookupTable(iLUT);
  coloredImage->SetInputConnection( iImage->GetProducerPort() );
  coloredImage->PassAlphaToOutputOff();
  coloredImage->SetOutputFormatToRGB();
  coloredImage->Update();

  range = coloredImage->GetOutput()->GetScalarRange();
  std::cout << "range after color image: " << range[0] << " to " << range[1] << std::endl;

  return coloredImage->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getImage(const unsigned int& iTime, const unsigned int& iChannel)
{
  GoMegaImageStructureMultiIndexContainer::index<Channel>::type::iterator it =
      m_MegaImageContainer.get< Channel >().find(iChannel);

  while(it!=m_MegaImageContainer.get< Channel >().end())
    {
    if(it->Time==iTime)
      {
      return colorImage(it->Image, it->LUT);
      }
    ++it;
    }
  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getImageBW(const unsigned int& iTime, const unsigned int& iChannel)
{
  GoMegaImageStructureMultiIndexContainer::index<Channel>::type::iterator it =
      m_MegaImageContainer.get< Channel >().find(iChannel);

  while(it!=m_MegaImageContainer.get< Channel >().end())
    {
    if(it->Time==iTime)
      {
      return it->Image;
      }
    ++it;
    }
  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getTimeAllChannels(const unsigned int& iTime)
{
  vtkSmartPointer<vtkImageBlend> blendedImage =
      vtkSmartPointer<vtkImageBlend>::New();
  blendedImage->RemoveAllInputs();

  GoMegaImageStructureMultiIndexContainer::index<Time>::type::iterator it =
      m_MegaImageContainer.get< Time >().find(iTime);

  /*
    \todo Nicolas - do sth else....
   */
  double size(0);
  while(it!=m_MegaImageContainer.get< Time >().end())
    {
    ++size;
    ++it;
    }
  it = m_MegaImageContainer.get< Time >().find(iTime);

  vtkIdType i(0);
  while(it!=m_MegaImageContainer.get< Time >().end())
    {
    it->LUT->Modified();
    it->LUT->Build();
    blendedImage->AddInputConnection(
          (this->colorImage(it->Image, it->LUT))->GetProducerPort());
    blendedImage->SetOpacity(i,1/size);
    ++i;
    ++it;
    }
  blendedImage->Update();

  return blendedImage->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getChannelAllTimes(const unsigned int& iChannel)
{
  vtkSmartPointer<vtkImageBlend> blendedImage =
      vtkSmartPointer<vtkImageBlend>::New();
  blendedImage->RemoveAllInputs();

  GoMegaImageStructureMultiIndexContainer::index<Channel>::type::iterator it =
      m_MegaImageContainer.get< Channel >().find(iChannel);

  /*
    \todo Nicolas - do sth else....
   */
  double size(0);
  while(it!=m_MegaImageContainer.get< Channel >().end())
    {
    ++size;
    ++it;
    }
  it = m_MegaImageContainer.get< Channel >().find(iChannel);

  vtkIdType i(0);
  while(it!=m_MegaImageContainer.get< Channel >().end())
    {
    blendedImage->AddInput(colorImage(it->Image, it->LUT));
    blendedImage->SetOpacity(i, 1/size);
    ++i;
    ++it;
    }
  blendedImage->Update();

  return blendedImage->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getAllImages()
{
  vtkSmartPointer<vtkImageBlend> blendedImage =
      vtkSmartPointer<vtkImageBlend>::New();
  blendedImage->RemoveAllInputs();
  blendedImage->SetBlendModeToCompound();

  double size = m_MegaImageContainer.size();

  GoMegaImageStructureMultiIndexContainer::iterator it =
      m_MegaImageContainer.begin();

  vtkIdType i(0);
  while(it!=m_MegaImageContainer.end())
    {
    std::cout << "in loop... " << i << std::endl;
    //double* range = colorImage(it->Image, it->LUT)->GetScalarRange();
    blendedImage->AddInput(colorImage(it->Image, it->LUT));
    blendedImage->SetOpacity(i,1/size);
    ++i;
    ++it;
    }
  blendedImage->Update();

  double* range = blendedImage->GetOutput()->GetScalarRange();

  std::cout << "range after blending: " << range[0] << " to " << range[1] << std::endl;

  vtkSmartPointer<vtkImageShiftScale> scale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  scale->SetInput(blendedImage->GetOutput());
  scale->SetScale(255/range[1]);
  scale->SetOutputScalarTypeToUnsignedChar();
  scale->Update();

  return scale->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int*
GoImageProcessor::
getBoundsTime()
{
  return m_BoundsTime;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int*
GoImageProcessor::
getBoundsChannel()
{
  return m_BoundsChannel;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int*
GoImageProcessor::
getExtent()
{
  return m_Extent;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getNumberOfTimePoints()
{
  return m_BoundsTime[1] - m_BoundsTime[0] + 1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getNumberOfChannels()
{
  return m_BoundsChannel[1] - m_BoundsChannel[0] + 1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getTimeInterval() const
{
  return m_TimeInterval;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getDopplerStep()
{
  return m_DopplerStep;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setDopplerStep(unsigned int iStep)
{
  m_DopplerStep = iStep;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int*
GoImageProcessor::
getDopplerTime(unsigned int iTime)
{
  m_DopplerTime[0] = iTime - m_DopplerStep;
  m_DopplerTime[1] = iTime;
  m_DopplerTime[2]= iTime + m_DopplerStep;

  // special case if we are at the borders
  if ( m_DopplerTime[0] < m_BoundsTime[0] )
    {
    m_DopplerTime[0] = -1;
    }
  if ( m_DopplerTime[2] > m_BoundsTime[1] )
    {
    m_DopplerTime[2] = -1;
    }

  return m_DopplerTime;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setDopplerMode(const bool& iEnable)
{
  m_DopplerMode = iEnable;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
GoImageProcessor::
getDopplerMode()
{
  return m_DopplerMode;
}
//--------------------------------------------------------------------------
