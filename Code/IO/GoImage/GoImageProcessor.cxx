
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
#include "vtkImageMapToColors.h"
#include "vtkImageBlend.h"
#include "vtkPointData.h"
#include "vtkImageShiftScale.h"
//histogram
#include "vtkImageExtractComponents.h"
#include "vtkImageAccumulate.h"

#include "vtkPiecewiseFunction.h"

#include <QString>

// temp
#include "vtkImageWeightedSum.h"

//--------------------------------------------------------------------------
GoImageProcessor::GoImageProcessor():m_Output(NULL),
  m_MaxThreshold(0),
  m_DopplerMode(false), m_DopplerStep(1), m_DopplerChannel(0), m_DopplerSize(3)
{
  m_BoundsTime[0] = 0;
  m_BoundsTime[1] = 0;

  m_BoundsChannel[0] = 0;
  m_BoundsChannel[1] = 0;

  m_Extent[0] = 0;
  m_Extent[1] = 0;
  m_Extent[2] = 0;
  m_Extent[3] = 0;
  m_Extent[4] = 0;
  m_Extent[5] = 0;

  m_DopplerTime.resize(m_DopplerSize);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoImageProcessor::GoImageProcessor(const GoImageProcessor & iE):
  m_MegaImageContainer(iE.m_MegaImageContainer), m_Output(iE.m_Output),
  m_DopplerMode(iE.m_DopplerMode), m_DopplerStep(iE.m_DopplerStep),
  m_DopplerChannel(iE.m_DopplerChannel), m_DopplerSize(iE.m_DopplerSize)
{
  m_BoundsTime[0] = iE.m_BoundsTime[0];
  m_BoundsTime[1] = iE.m_BoundsTime[1];

  m_BoundsChannel[0] = iE.m_BoundsChannel[0];
  m_BoundsChannel[1] = iE.m_BoundsChannel[1];

  m_Extent[0] = iE.m_Extent[0];
  m_Extent[1] = iE.m_Extent[1];
  m_Extent[2] = iE.m_Extent[2];
  m_Extent[3] = iE.m_Extent[3];
  m_Extent[4] = iE.m_Extent[4];
  m_Extent[5] = iE.m_Extent[5];

  m_DopplerTime = iE.m_DopplerTime;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoImageProcessor::
~GoImageProcessor()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
createLUT(const double& iRed, const double& iGreen, const double& iBlue,
          const double& iAlpha)
{
  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  double* HSV = vtkMath::RGBToHSV(iRed,iGreen,iBlue);
  lut->SetAlphaRange(0, 1);
  lut->SetHueRange(HSV[0], HSV[0]);
  lut->SetSaturationRange(1, 1);
  lut->SetValueRange(0, 1);
  lut->SetNumberOfTableValues(m_MaxThreshold);
  double* range = new double[2];
  range[0] = 0;
  range[1] = m_MaxThreshold;
  lut->SetRange(range);

  lut->Build();

  delete range;

  return lut;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
getLookuptable(const std::string& iName) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  if(it!=m_MegaImageContainer.get< Name >().end())
    {
    return it->LUT;
    }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
getLookuptable() const
{
  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  assert(it!=m_MegaImageContainer.get< Visibility >().end());

  return it->LUT;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPiecewiseFunction>
GoImageProcessor::
getOpacityTransferFunction(const std::string& iName) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  if(it!=m_MegaImageContainer.get< Name >().end())
    {
    return it->OpacityTF;
    }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<vtkPiecewiseFunction*>
GoImageProcessor::
getOpacityTransferFunctions()
{
  std::vector<vtkPiecewiseFunction*> opacityTFs;

  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  while(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    opacityTFs.push_back(it->OpacityTF);
    ++it;
    }

  return opacityTFs;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<double>
GoImageProcessor::
getColor(const std::string& iName) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  assert(it!=m_MegaImageContainer.get< Name >().end());

  return it->Color;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::map<unsigned int, unsigned int> >
GoImageProcessor::
getRGBA(const std::string& iName) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  assert(it!=m_MegaImageContainer.get< Name >().end());

  return it->RGBA;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageAccumulate>
GoImageProcessor::
getHistogram(const std::string& iName) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  assert(it!=m_MegaImageContainer.get< Name >().end());

  return it->Histogram;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
colorImage(vtkSmartPointer<vtkImageData> iImage,
           vtkSmartPointer<vtkLookupTable> iLUT)
{
  vtkSmartPointer<vtkImageMapToColors> coloredImage =
      vtkSmartPointer<vtkImageMapToColors>::New();
  coloredImage->SetLookupTable(iLUT);
  coloredImage->SetInput( iImage );
  coloredImage->SetOutputFormatToRGB();
  coloredImage->SetNumberOfThreads(VTK_MAX_THREADS);
  coloredImage->ReleaseDataFlagOn();
  coloredImage->Update();

  return coloredImage->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getImageBW(const unsigned int& iIndex)
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);
  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    return it->Image;
    }
  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getImageBW()
{
  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  if(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    return it->Image;
    }
  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<vtkImageData*>
GoImageProcessor::
getColoredImages()
{
  std::vector<vtkImageData*> images;

  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  while(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    // requiered deepcopy....
    vtkImageData* image = vtkImageData::New();
    image->DeepCopy(colorImage(it->Image, it->LUT));
    images.push_back(image);
    ++it;
    }

  return images;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getVisibleImages()
{
  vtkSmartPointer<vtkImageBlend> blendedImage =
      vtkSmartPointer<vtkImageBlend>::New();
  blendedImage->RemoveAllInputs();
  blendedImage->SetBlendModeToCompound();
  blendedImage->ReleaseDataFlagOn();
  blendedImage->SetNumberOfThreads(VTK_MAX_THREADS);

  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  vtkIdType i(0);

  while(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    vtkSmartPointer<vtkImageData> temp = colorImage(it->Image, it->LUT);
    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
    image->DeepCopy(temp);
    temp = NULL;
    blendedImage->AddInput(image);
    ++i;
    ++it;
    }
  blendedImage->Update();

  double rangeR[2];
  blendedImage->GetOutput()->GetPointData()->GetScalars()->GetRange(rangeR, 0);
  double rangeG[2];
  blendedImage->GetOutput()->GetPointData()->GetScalars()->GetRange(rangeG, 1);
  double rangeB[2];
  blendedImage->GetOutput()->GetPointData()->GetScalars()->GetRange(rangeB, 2);

  double range = std::max(rangeB[1], std::max(rangeR[1], rangeG[1]));

  if(range)
    {
    vtkSmartPointer<vtkImageShiftScale> scale =
        vtkSmartPointer<vtkImageShiftScale>::New();
    scale->SetInput(blendedImage->GetOutput());
    scale->SetScale(m_MaxThreshold/range);
    scale->SetOutputScalarTypeToUnsignedChar();
    scale->ReleaseDataFlagOn();
    scale->SetNumberOfThreads(VTK_MAX_THREADS);
    scale->Update();

    blendedImage = NULL;

    return scale->GetOutput();
    }

  return blendedImage->GetOutput();
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
std::vector<int>
GoImageProcessor::
getDopplerTime(unsigned int iTime)
{
  // get min t point
  int time = iTime - (m_DopplerSize/2)*m_DopplerStep;

  for(unsigned int i=0; i<m_DopplerSize; ++i)
    {
    m_DopplerTime[i] = time +i*m_DopplerStep;
    std::cout << "doppler time: " << i << " is "<< time +i*m_DopplerStep << std::endl;

    // special case if we are at the borders
    // value will be -1
    if (m_DopplerTime[i] < static_cast<int>(m_BoundsTime[0]))
      {
      m_DopplerTime[i] = -1;
      }
    if (m_DopplerTime[i] > static_cast<int>(m_BoundsTime[1]))
      {
      m_DopplerTime[i] = -1;
      }
    }
/*
  m_DopplerTime[0] = iTime - m_DopplerStep;
  m_DopplerTime[1] = iTime;
  m_DopplerTime[2]= iTime + m_DopplerStep;

*/

  return m_DopplerTime;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setDopplerMode(const bool& iEnable, const unsigned int& iChannel)
{
  m_DopplerMode = iEnable;
  m_DopplerChannel = iChannel;
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
unsigned int
GoImageProcessor::
getDopplerChannel()
{
  return m_DopplerChannel;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getDopplerSize()
{
  return m_DopplerSize;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setDopplerSize(int iSize)
{
  m_DopplerSize = iSize;
  m_DopplerTime.resize(iSize);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string
GoImageProcessor::
getChannelName(const unsigned int& iIndex)
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    return it->Name;
    }
  return "";
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
visibilityChanged(std::string iName, bool iVisibility)
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  if(it!=m_MegaImageContainer.get< Name >().end())
    {
    m_MegaImageContainer.get< Name >().modify( it , set_visibility(iVisibility));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getNumberOfVisibleChannels()
{
  unsigned int numberOfVisibleChannels(0);

  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true)
      ;
  while(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    ++numberOfVisibleChannels;
    ++it;
    }

  return numberOfVisibleChannels;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
updatePoints(std::string iName, std::vector< std::map< unsigned int, unsigned int> > iVector)
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName);

  if(it!=m_MegaImageContainer.get< Name >().end())
    {
    m_MegaImageContainer.get< Name >().modify( it , set_PointsRGBA(iVector));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int
GoImageProcessor::
getMaxThreshold()
{
  return m_MaxThreshold;
}
//--------------------------------------------------------------------------
