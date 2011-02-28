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
#ifndef __itkExtractMeshesFromLabelImageFilter_h
#define __itkExtractMeshesFromLabelImageFilter_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkImageToImageFilter.h"
#include "itkImage.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkShapeLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkShapeRelabelImageFilter.h"
#include <itkQuadEdgeMesh.h>

#include "itkVector.h"
#include <itkQuadEdgeMesh.h>
#include "itkSmoothingQuadEdgeMeshFilter.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter.h"
#include "itkBinaryMask3DMeshSource.h"

#include <fstream>
#include "itkVTKPolyDataWriter.h"
#include "itkMultiThreader.h"

namespace itk
{

/**
  \class ExtractMeshesFromLabelImageFilter
  \brief
  \author Kishore Mosaliganti
  */
template< class TImage >
class ITK_EXPORT ExtractMeshesFromLabelImageFilter : public Object
{
public:
  typedef ExtractMeshesFromLabelImageFilter Self;
  typedef Object                            Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  itkStaticConstMacro ( ImageDimension, unsigned int, TImage::ImageDimension );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ExtractMeshesFromLabelImageFilter, Object );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Input Image typedef */
  typedef TImage                             ImageType;
  typedef typename ImageType::Pointer        ImagePointer;
  typedef typename ImageType::ConstPointer   ImageConstPointer;
  typedef typename ImageType::IndexType      IndexType;
  typedef typename IndexType::IndexValueType IndexValueType;
  typedef typename ImageType::PixelType      PixelType;
  typedef typename ImageType::SizeType       SizeType;
  typedef typename SizeType::SizeValueType   SizeValueType;
  typedef typename ImageType::RegionType     RegionType;
  typedef typename ImageType::SpacingType    SpacingType;
  typedef typename ImageType::PointType      PointType;
  typedef typename PointType::CoordRepType   CoordType;

  typedef float Coord;
  typedef QuadEdgeMesh< Coord, ImageDimension >   MeshType;
  typedef typename MeshType::Pointer              MeshPointer;

  typedef std::vector< MeshPointer >              MeshVectorType;

  typedef unsigned int                                        ShapeLabelType;
  typedef ShapeLabelObject< ShapeLabelType, ImageDimension >  ShapeLabelObjectType;
  typedef LabelMap< ShapeLabelObjectType >                    ShapeLabelMapType;
  typedef typename ShapeLabelMapType::Pointer                 ShapeLabelMapPointer;

  typedef LabelImageToShapeLabelMapFilter< ImageType, ShapeLabelMapType > ShapeConverterType;
  typedef typename ShapeConverterType::Pointer ShapeConverterPointer;
  typedef typename ShapeLabelMapType::LabelObjectContainerType LabelObjectContainerType;
  typedef typename LabelObjectContainerType::const_iterator LabelObjectIterator;

  typedef RegionOfInterestImageFilter< ImageType, ImageType > ROIFilterType;
  typedef typename ROIFilterType::Pointer                     ROIFilterPointer;

  typedef ImageRegionIterator< ImageType > IteratorType;
  typedef ImageRegionIteratorWithIndex< ImageType > IndexIteratorType;

  typedef BinaryMask3DMeshSource< ImageType, MeshType > MeshSourceType;
  typedef typename MeshSourceType::Pointer MeshSourcePointer;

  typedef SmoothingQuadEdgeMeshFilter< MeshType, MeshType > MeshSmoothingType;
  typedef typename MeshSmoothingType::Pointer MeshSmoothingPointer;

  typedef NumberOfFacesCriterion< MeshType > CriterionType;
  typedef typename CriterionType::Pointer CriterionPointer;

  typedef SquaredEdgeLengthDecimationQuadEdgeMeshFilter<
    MeshType, MeshType, CriterionType > DecimationType;
  typedef typename DecimationType::Pointer DecimationPointer;

  typedef VTKPolyDataWriter< MeshType > MeshWriterType;
  typedef typename MeshWriterType::Pointer MeshWriterPointer;

  typedef MultiThreader ThreaderType;
  typedef typename ThreaderType::Pointer ThreaderPointer;

  itkSetConstObjectMacro( Input , ImageType );
  itkGetConstMacro( NumberOfMeshes, unsigned int );
  itkSetMacro( NumberOfTrianglesPerMesh, unsigned int );
  itkGetConstMacro( DelaunayConforming, bool );
  itkSetMacro( DelaunayConforming, bool );

  itkSetMacro( UseDecimation, bool );
  itkGetConstMacro( UseDecimation, bool );

  itkSetMacro( UseSmoothing, bool );
  itkGetConstMacro( UseSmoothing, bool );

  itkGetConstMacro( NumberOfTrianglesPerMesh, unsigned int );
  itkSetMacro( NumberOfSmoothingIterations, unsigned int );
  itkGetConstMacro( NumberOfSmoothingIterations, unsigned int );
  itkSetMacro( SmoothingRelaxationFactor, double );
  itkGetConstMacro( SmoothingRelaxationFactor, double );
  itkGetConstMacro( NumberOfThreads, unsigned int );
  itkSetMacro( NumberOfThreads, unsigned int );

  MeshVectorType m_Meshes;

  void GenerateData();

protected:
  ExtractMeshesFromLabelImageFilter();
  ~ExtractMeshesFromLabelImageFilter(){}
  void PrintSelf(std::ostream& os, Indent indent) const;
  void ThreadedExtractMesh( const unsigned int& startLabel,
                            const unsigned int& endLabel );

  static ITK_THREAD_RETURN_TYPE ThreaderCallback(void * arg);

  struct ThreadStruct
  {
    Self*                 Filter;
  };

  ImageConstPointer     m_Input;
  ShapeLabelMapPointer  m_ShapeLabelMap;

  unsigned int m_NumberOfThreads;
  unsigned int m_NumberOfMeshes;
  unsigned int m_NumberOfTrianglesPerMesh;
  unsigned int m_NumberOfSmoothingIterations;
  unsigned int m_SmoothingRelaxationFactor;

  bool m_DelaunayConforming;
  bool m_UseSmoothing;
  bool m_UseDecimation;

private:
  ExtractMeshesFromLabelImageFilter(const Self&) {}
  void operator=(const Self&) {}
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExtractMeshesFromLabelImageFilter.txx"
#endif

#endif
