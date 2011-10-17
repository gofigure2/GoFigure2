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
#ifndef __itkConvertMeshesToLabelImageFilter_h
#define __itkConvertMeshesToLabelImageFilter_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "GoFiltersConfigure.h"

#include "itkImageToImageFilter.h"
#include "itkImage.h"

#include "itkImageRegionIteratorWithIndex.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkQuadEdgeMesh.h"
#include "itkTriangleMeshToBinaryImageFilter.h"
#include "itkVector.h"

#include <fstream>
#include "itkMultiThreader.h"

namespace itk
{

template< class TImage,
          class TMesh = QuadEdgeMesh< typename TImage::PointType::CoordRepType,
                                      TImage::ImageDimension > >
class GOFILTERS_EXPORT ConvertMeshesToLabelImageFilter : public Object
{
public:
  typedef ConvertMeshesToLabelImageFilter   Self;
  typedef Object                            Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  itkStaticConstMacro ( ImageDimension, unsigned int, TImage::ImageDimension );

  /** Run-time type information (and related methods). */
  itkTypeMacro( ConvertMeshesToLabelImageFilter, Object );

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

  typedef TMesh                      MeshType;
  typedef typename MeshType::Pointer MeshPointer;
  typedef std::vector< MeshPointer > MeshVectorType;

  typedef RegionOfInterestImageFilter< ImageType, ImageType > ROIFilterType;
  typedef typename ROIFilterType::Pointer ROIFilterPointer;

  typedef ImageRegionIterator< ImageType > IteratorType;
  typedef ImageRegionIteratorWithIndex< ImageType > IndexIteratorType;

  typedef TriangleMeshToBinaryImageFilter< MeshType, ImageType > MeshToImageFilterType;
  typedef typename MeshToImageFilterType::Pointer MeshToImageFilterPointer;

  typedef MultiThreader ThreaderType;
  typedef typename ThreaderType::Pointer ThreaderPointer;

  itkSetObjectMacro( Input , ImageType );
  itkGetConstMacro( NumberOfMeshes, size_t );

#ifdef ITKv4
  itkGetConstMacro( NumberOfThreads, ThreadIdType );
  itkSetMacro( NumberOfThreads, ThreadIdType );
#else
  itkGetConstMacro( NumberOfThreads, int );
  itkSetMacro( NumberOfThreads, int );
#endif

  void SetMeshes( const MeshVectorType& iMeshes );

  void Update();

protected:
  ConvertMeshesToLabelImageFilter();
  ~ConvertMeshesToLabelImageFilter(){}
  void PrintSelf(std::ostream& os, Indent indent) const;
  void ThreadedExtractMesh( const unsigned int& startLabel,
                            const unsigned int& endLabel );

  static ITK_THREAD_RETURN_TYPE ThreaderCallback(void * arg);

  struct ThreadStruct
  {
    Self* Filter;
  };

  size_t        m_NumberOfMeshes;
#ifdef ITKv4
  ThreadIdType  m_NumberOfThreads;
#else
  int           m_NumberOfThreads;
#endif

  ImageType*    m_Input;

  MeshVectorType m_Meshes;

  void GenerateData();

private:
  ConvertMeshesToLabelImageFilter(const Self&);
  void operator=(const Self&);
};

} // end namespace itk

#include "itkConvertMeshesToLabelImageFilter.txx"

#endif
