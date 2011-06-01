
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
  m_BoundsTime(NULL), m_BoundsChannel(NULL), m_Extent(NULL),
  m_DopplerMode(false), m_DopplerStep(1), m_DopplerChannel(0)
{
  m_DopplerTime = new int[3];
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoImageProcessor::GoImageProcessor(const GoImageProcessor & iE):
  m_MegaImageContainer(iE.m_MegaImageContainer), m_Output(iE.m_Output),
  m_BoundsTime(iE.m_BoundsTime), m_BoundsChannel(iE.m_BoundsChannel),
  m_Extent(iE.m_Extent),  m_DopplerMode(iE.m_DopplerMode),
  m_DopplerStep(iE.m_DopplerStep), m_DopplerTime(iE.m_DopplerTime),
  m_DopplerChannel(iE.m_DopplerChannel)
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

//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
createLUT(const double& iRed, const double& iGreen, const double& iBlue,
          const double& iAlpha, const double* iRange)
{
  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  double* HSV = vtkMath::RGBToHSV(iRed,iGreen,iBlue);
  lut->SetAlphaRange(0, 1);
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
                    const unsigned int& iIndex)
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    m_MegaImageContainer.get< Index >().modify( it , set_lut(iLUT) );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
getLookuptable(const unsigned int& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    return it->LUT;
    }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable>
GoImageProcessor::
getLookuptable(const std::string& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iIndex);

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
getOpacityTransferFunction(const std::string& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Name >().end())
    {
    return it->OpacityTF;
    }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPiecewiseFunction>
GoImageProcessor::
getOpacityTransferFunction() const
{
  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  if(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    return it->OpacityTF;
    }

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<double>
GoImageProcessor::
getColor(const unsigned int& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  assert(it!=m_MegaImageContainer.get< Index >().end());

  return it->Color;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<double>
GoImageProcessor::
getColor(const std::string& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iIndex);

  assert(it!=m_MegaImageContainer.get< Name >().end());

  return it->Color;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::map<unsigned int, unsigned int> >
GoImageProcessor::
getRGBA(const std::string& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iIndex);

  assert(it!=m_MegaImageContainer.get< Name >().end());

  return it->RGBA;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageAccumulate*
GoImageProcessor::
getHistogram(const std::string& iIndex) const
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iIndex);

  assert(it!=m_MegaImageContainer.get< Name >().end());

  double* range;
  range =  it->Image->GetScalarRange();

  vtkImageAccumulate* histogram = vtkImageAccumulate::New();
  histogram->SetInput( it->Image );
  histogram->SetComponentExtent(
    0,
    static_cast<int>(range[1])-static_cast<int>(range[0])-1,0,0,0,0 );
  histogram->SetComponentOrigin( range[0],0,0 );
  histogram->SetComponentSpacing( 1,0,0 );
  histogram->SetIgnoreZero( false );
  histogram->Update();

  return histogram;
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
vtkSmartPointer<vtkImageData>
GoImageProcessor::
getAllImages()
{
  vtkSmartPointer<vtkImageBlend> blendedImage =
      vtkSmartPointer<vtkImageBlend>::New();
  blendedImage->RemoveAllInputs();
  blendedImage->SetBlendModeToCompound();
  blendedImage->SetNumberOfThreads(VTK_MAX_THREADS);

  GoMegaImageStructureMultiIndexContainer::index<Visibility>::type::iterator it =
      m_MegaImageContainer.get< Visibility >().find(true);

  vtkIdType i(0);
  while(it!=m_MegaImageContainer.get< Visibility >().end())
    {
    blendedImage->AddInput(colorImage(it->Image, it->LUT));
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

  vtkSmartPointer<vtkImageShiftScale> scale =
      vtkSmartPointer<vtkImageShiftScale>::New();
  scale->SetInput(blendedImage->GetOutput());
  scale->SetScale(255/range);
  scale->SetOutputScalarTypeToUnsignedChar();
  scale->SetNumberOfThreads(VTK_MAX_THREADS);
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
  // value will be -1
  if (m_DopplerTime[0] < static_cast<int>(m_BoundsTime[0]))
    {
    m_DopplerTime[0] = -1;
    }
  if (m_DopplerTime[2] > static_cast<int>(m_BoundsTime[1]))
    {
    m_DopplerTime[2] = -1;
    }

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
void
GoImageProcessor::
setVisibilityChannel(const unsigned int& iIndex, const bool& iVisibility)
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    m_MegaImageContainer.get< Index >().modify( it , set_visibility(iVisibility) );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setNameChannel(const unsigned int& iIndex, const std::string& iName)
{
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    m_MegaImageContainer.get< Index >().modify( it , set_name(iName) );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string
GoImageProcessor::
getNameChannel(const unsigned int& iIndex)
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
std::vector<std::string>
GoImageProcessor::
getVisibilityVector()
{
std::vector<std::string> visibility;
GoMegaImageStructureMultiIndexContainer::iterator it =
        m_MegaImageContainer.begin();
while(it!=m_MegaImageContainer.end())
  {
  if(!it->Visibility)
    {
    visibility.push_back(it->Name);
    }
  ++it;
  }
return visibility;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
setVisibilityVector(const std::vector<std::string>& iVisibility)
{
  for(int i =0; i<iVisibility.size(); ++i)
    {
    GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
        m_MegaImageContainer.get< Name >().find(iVisibility[i]);
    if(it!=m_MegaImageContainer.get< Name >().end())
      {
      setVisibilityChannel(it->Index, false);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
unsigned int
GoImageProcessor::
getContainerSize()
{
  return m_MegaImageContainer.size();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
GoImageProcessor::
updatePoints(QString iName, std::vector< std::map< unsigned int, unsigned int> > iVector)
{
  GoMegaImageStructureMultiIndexContainer::index<Name>::type::iterator it =
      m_MegaImageContainer.get< Name >().find(iName.toStdString());

  if(it!=m_MegaImageContainer.get< Name >().end())
    {
    m_MegaImageContainer.get< Name >().modify( it , set_PointsRGBA(iVector));
    }
}
