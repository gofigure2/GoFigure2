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
#ifndef __itkBinaryMaskImageToGoFigureMeshAttributes_h
#define __itkBinaryMaskImageToGoFigureMeshAttributes_h

#include "GoFiltersConfigure.h"

#include "itkLightObject.h"
#include "itkShapeLabelObject.h"
#include "itkStatisticsLabelObject.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"

namespace itk
{
/** \class BinaryMaskImageToGoFigureMeshAttributes
 *  \brief Compute Mesh Attributes from a binary mask image
 *  \tparam TInput Input Image Type (depends on the kind of images used,
 *  but one channel)
 *  \tparam TMask Mast Image Type
 *  \sa GoFigureMeshAttributes
 * */
template< class TInput, class TMask >
class GOFILTERS_EXPORT BinaryMaskImageToGoFigureMeshAttributes:public LightObject
{
public:
  typedef BinaryMaskImageToGoFigureMeshAttributes Self;
  typedef LightObject                             Superclass;
  typedef SmartPointer< Self >                    Pointer;
  typedef SmartPointer< const Self >              ConstPointer;

  typedef TInput                          ImageType;
  typedef typename ImageType::Pointer     ImagePointer;
  typedef typename ImageType::SizeType    ImageSizeType;
  typedef typename ImageType::SpacingType ImageSpacingType;

  typedef TMask                           MaskImageType;
  typedef typename MaskImageType::Pointer MaskImagePointer;

  itkStaticConstMacro(ImageDimension, unsigned int,
                      ImageType::ImageDimension);

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(BinaryMaskImageToGoFigureMeshAttributes, LightObject);

  /** Display */
//     void PrintSelf( std::ostream& os, Indent indent ) const;

  typedef unsigned char LabelType;

  typedef ShapeLabelObject< LabelType, ImageDimension > ShapeLabelObjectType;
  typedef typename ShapeLabelObjectType::Pointer        ShapeLabelObjectPointer;

  typedef StatisticsLabelObject< LabelType, ImageDimension > StatLabelObjectType;
  typedef typename StatLabelObjectType::Pointer StatLabelObjectPointer;

  typedef LabelMap< ShapeLabelObjectType >    ShapeLabelMapType;
  typedef typename ShapeLabelMapType::Pointer ShapeLabelMapPointer;

  typedef LabelMap< StatLabelObjectType >    StatLabelMapType;
  typedef typename StatLabelMapType::Pointer StatLabelMapPointer;

  typedef LabelImageToShapeLabelMapFilter< MaskImageType,
                                           ShapeLabelMapType >  ShapeConverterType;
  typedef typename ShapeConverterType::Pointer                  ShapeConverterPointer;

  typedef LabelImageToStatisticsLabelMapFilter< MaskImageType,
                                                ImageType,
                                                StatLabelMapType >  StatConverterType;
  typedef typename StatConverterType::Pointer                       StatConverterPointer;

  /** \brief Set the Image */
  void SetImage(ImageType *iInput);

  /** \brief Set the binary mask which corresponds to the input mesh */
  void SetMaskImage(MaskImageType *iMask);

  /** \brief Do you need to compute any intensity related attributes */
  void SetIntensityBasedComputation( const bool& iComputation );

  /** \brief Here make all the computation */
  void Update();

  /** \brief Get the number of voxels inside the mesh */
  unsigned int GetSize();

  /** \brief Get the volume inside the mesh */
  double GetPhysicalSize();

  /** \brief Get the mean intensity inside the mesh */
  double GetMeanIntensity();

  /** \brief Get the total intensity inside the mesh */
  double GetSumIntensity();

protected:
  /** \brief Constructor */
  BinaryMaskImageToGoFigureMeshAttributes();

  /** \brief Destructor*/
  ~BinaryMaskImageToGoFigureMeshAttributes();

  ImagePointer m_InputImage;

  MaskImagePointer m_MaskImage;

  unsigned int m_Size;

  double m_PhysicalSize;
  double m_Mean;
  double m_Sum;
  bool m_IntensityComputation;

  virtual void GenerateData();

private:
  BinaryMaskImageToGoFigureMeshAttributes(const Self &);
  void operator=(const Self &);
};
}
#include "itkBinaryMaskImageToGoFigureMeshAttributes.txx"
#endif
