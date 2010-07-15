/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#ifndef __itkBinaryMaskImageToGoFigureMeshAttributes_txx
#define __itkBinaryMaskImageToGoFigureMeshAttributes_txx

namespace itk
{

template<class TInput, class TMask>
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::
BinaryMaskImageToGoFigureMeshAttributes()
{}

template<class TInput, class TMask>
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::
~BinaryMaskImageToGoFigureMeshAttributes()
{}

template<class TInput, class TMask>
void
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::
SetImage(ImageType* iInput)
{
  m_InputImage = iInput;
}

template<class TInput, class TMask>
void
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::
SetMaskImage(MaskImageType* iMask)
{
  m_MaskImage = iMask;
}

template<class TInput, class TMask>
void
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::
Update()
{
  GenerateData();
}

template<class TInput, class TMask>
unsigned int
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::GetSize()
{
  return m_Size;
}

template<class TInput, class TMask>
double
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::GetPhysicalSize()
{
  return m_PhysicalSize;
}

template<class TInput, class TMask>
double BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::GetMeanIntensity()
{
  return m_Mean;
}

template<class TInput, class TMask>
double BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::GetSumIntensity()
{
  return m_Sum;
}

template<class TInput, class TMask>
void
BinaryMaskImageToGoFigureMeshAttributes<TInput, TMask>::
GenerateData()
{
  // shape stuff
  ShapeConverterPointer shapeConverter = ShapeConverterType::New();
  shapeConverter->SetInput(m_MaskImage);
  shapeConverter->SetBackgroundValue(0);
  try
    {
    shapeConverter->Update();
    }
  catch(itk::ExceptionObject & e)
    {
    std::cerr << "Exception Caught: " << e << std::endl;
    return;
    }

  ShapeLabelMapPointer        shapeLabelMap = shapeConverter->GetOutput();
  const ShapeLabelObjectType *shapeObject = shapeLabelMap->GetLabelObject(255);

  // stat stuff
  StatConverterPointer statConverter = StatConverterType::New();
  statConverter->SetInput(m_MaskImage);
  statConverter->SetFeatureImage(m_InputImage);
  statConverter->SetBackgroundValue(0);
  statConverter->Update();

  StatLabelMapPointer        statLabelMap = statConverter->GetOutput();
  const StatLabelObjectType *statObject = statLabelMap->GetLabelObject(255);

  // Number of voxels;
  m_Size = shapeObject->Size();

  // Volume or area in um^3
  m_PhysicalSize = shapeObject->GetPhysicalSize();

  m_Mean = statObject->GetMean();

  m_Sum = statObject->GetSum();
}

}

#endif
