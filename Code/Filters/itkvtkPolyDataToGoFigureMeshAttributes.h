/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
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
#ifndef __itkvtkPolyDataToGoFigureMeshAttributes_h
#define __itkvtkPolyDataToGoFigureMeshAttributes_h

#include "itkLightObject.h"
#include "itkvtkPolyDataToBinaryMaskImageFilter.h"
#include "itkBinaryMaskImageToGoFigureMeshAttributes.h"
#include "itkImageFileWriter.h"

namespace itk
{
template< class TImage >
class vtkPolyDataToGoFigureMeshAttributes:public LightObject
{
public:
  typedef vtkPolyDataToGoFigureMeshAttributes Self;
  typedef LightObject                         Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  typedef TImage                      ImageType;
  typedef typename ImageType::Pointer ImagePointer;

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(vtkPolyDataToGoFigureMeshAttributes, LightObject);

  typedef vtkPolyDataToBinaryMaskImageFilter< ImageType, ImageType >
  PolyDataToBinaryMaskImageFilterType;
  typedef typename PolyDataToBinaryMaskImageFilterType::Pointer
  PolyDataToBinaryMaskImageFilterPointer;

  typedef BinaryMaskImageToGoFigureMeshAttributes< ImageType, ImageType >
  BinaryMaskImageToGoFigureMeshAttributesType;
  typedef typename BinaryMaskImageToGoFigureMeshAttributesType::Pointer
  BinaryMaskImageToGoFigureMeshAttributesPointer;

  typedef ImageFileWriter< ImageType > WriterType;

  virtual void SetImage(ImageType *iImage);

  virtual void SetPolyData(vtkPolyData *iMesh);

  void SetIntensityBasedComputation( const bool& iComputation );
  virtual void Update();

  unsigned int GetSize();

  double GetPhysicalSize();

  double GetMeanIntensity();

  double GetSumIntensity();

  double GetArea();

protected:
  vtkPolyDataToGoFigureMeshAttributes();
  ~vtkPolyDataToGoFigureMeshAttributes();

  vtkPolyData *m_Mesh;
  ImagePointer m_Image;

  PolyDataToBinaryMaskImageFilterPointer m_Binarizer;
  BinaryMaskImageToGoFigureMeshAttributesPointer m_AttributeCalculator;

  unsigned int m_Size;
  double m_PhysicalSize;
  double m_Area;
  double m_Mean;
  double m_Sum;
  bool m_IntensityComputation;

  virtual void GenerateData();

  void ComputeArea();

private:
  vtkPolyDataToGoFigureMeshAttributes(const Self &);
  void operator=(const Self &);
};
}

#include "itkvtkPolyDataToGoFigureMeshAttributes.txx"
#endif
