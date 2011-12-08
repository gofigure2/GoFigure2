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

#ifndef __itkvtkMeshFilterBase_h
#define __itkvtkMeshFilterBase_h

#include "itkObject.h"
#include "itkConvertMeshesToLabelImageFilter.h"
#include "itkvtkPolyDataToitkQuadEdgeMesh.h"

#include "itkShapeLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"

namespace itk
{
template< class TFeatureImage >
class vtkMeshFilterBase : public Object
{
public:
  typedef Object Superclass;
  typedef vtkMeshFilterBase Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( vtkMeshFilterBase, Object );

  typedef TFeatureImage FeatureImageType;
  typedef typename FeatureImageType::Pointer FeatureImagePointer;
  typedef typename FeatureImageType::PixelType FeatureImagePixelType;
  typedef typename FeatureImageType::IndexType FeatureImageIndexType;
  typedef typename FeatureImageType::PointType FeatureImagePointType;

  typedef ConvertMeshesToLabelImageFilter< FeatureImageType > MeshToLabelFilterType;
  typedef typename MeshToLabelFilterType::Pointer MeshToLabelFilterPointer;
  typedef typename MeshToLabelFilterType::MeshType QuadEdgeMeshType;

  typedef vtkPolyDataToitkQuadEdgeMesh< QuadEdgeMeshType > MeshConverterType;
  typedef typename MeshConverterType::Pointer MeshConverterPointer;


  // ---------------------------------------------------------------------------

  /** \typedef LabelType label type*/
  typedef unsigned int                                LabelType;

  // ---------------------------------------------------------------------------

  /** \typedef ShapeLabelObjectType */
  typedef ShapeLabelObject< LabelType,
                            FeatureImageType::ImageDimension >
                                                        ShapeLabelObjectType;
  typedef typename ShapeLabelObjectType::Pointer        ShapeLabelObjectPointer;

  /** \typedef ShapeLabelMapType */
  typedef LabelMap< ShapeLabelObjectType >              ShapeLabelMapType;
  typedef typename ShapeLabelMapType::Pointer           ShapeLabelMapPointer;

  /** \typedef ShapeConverterType */
  typedef LabelImageToShapeLabelMapFilter< FeatureImageType,
                                           ShapeLabelMapType >
                                                        ShapeConverterType;
  typedef typename ShapeConverterType::Pointer          ShapeConverterPointer;


  typedef typename ShapeLabelMapType::LabelObjectContainerType
                                                        LabelObjectContainerType;
  typedef typename LabelObjectContainerType::const_iterator
                                                        LabelObjectIterator;

  // ---------------------------------------------------------------------------

  /** \typedef StatLabelObjectType */
  typedef StatisticsLabelObject< LabelType,
                                 FeatureImageType::ImageDimension >
                                                StatLabelObjectType;
  typedef typename StatLabelObjectType::Pointer StatLabelObjectPointer;


  typedef LabelMap< StatLabelObjectType >     StatLabelMapType;
  typedef typename StatLabelMapType::Pointer  StatLabelMapPointer;

  typedef LabelImageToStatisticsLabelMapFilter<
    FeatureImageType,
    FeatureImageType,
    StatLabelMapType >                        StatConverterType;

  typedef typename StatConverterType::Pointer StatConverterPointer;

  // ---------------------------------------------------------------------------

  void SetNumberOfImages( const size_t& iN );

  void SetFeatureImage( const size_t& iId, FeatureImageType* iImage );

  void Update();

protected:
  vtkMeshFilterBase();
  virtual ~vtkMeshFilterBase() {}

  virtual void GenerateData() = 0;

  virtual void SetRequiredAttributeComputationFlags() = 0;

  void ComputeOutputAttributes();

  std::vector< FeatureImagePointer > m_Images;
  std::vector< vtkPolyData* > m_Outputs;
  ShapeLabelMapPointer  m_ShapeLabelMap;
  std::vector< StatLabelMapPointer > m_StatLabelMap;
  std::map< size_t, LabelType > m_MeshtoLabelIdMap;

  bool m_ShapeComputation;
  bool m_IntensityComputation;

private:
  vtkMeshFilterBase( const Self& );
  void operator = ( const Self& );
};
}

#include "itkvtkMeshFilterBase.txx"
#endif

