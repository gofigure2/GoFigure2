/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#ifndef __itkChanAndVeseSegmentationFilter_h
#define __itkChanAndVeseSegmentationFilter_h

#include "itkScalarChanAndVeseSparseLevelSetImageFilter.h"
#include "itkScalarChanAndVeseLevelSetFunction.h"
#include "itkScalarChanAndVeseLevelSetFunctionData.h"
#include "itkConstrainedRegionBasedLevelSetFunctionSharedData.h"
#include "itkAtanRegularizedHeavisideStepFunction.h"

#include "itkFastMarchingImageFilter.h"

#include "itkImageToVTKImageFilter.h"

#include "vtkImageData.h"

//arnaudgelas: What is this file? 
//missing on server side
//#include "itkCellPreprocess.h"
#include "itkRegionOfInterestImageFilter.h"

namespace itk
{
/**
 * \class ChanAndVeseSegmentationFilter
 * \brief
*/
template< class TFeatureImage >
class ChanAndVeseSegmentationFilter : public Object
{
public:
  typedef ChanAndVeseSegmentationFilter Self;
  typedef SmartPointer< Self >          Pointer;
  typedef SmartPointer< const Self >    ConstPointer;
  typedef Object                        Superclass;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( ChanAndVeseSegmentationFilter, Object );
  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  itkStaticConstMacro( Dimension, unsigned int, TFeatureImage::ImageDimension);

  typedef Image< float, Dimension >                   InternalImageType;
  typedef typename InternalImageType::Pointer         InternalImagePointer;
  typedef typename InternalImageType::PointType       InternalPointType;
  typedef typename InternalPointType::CoordRepType    InternalCoordRepType;
  typedef typename InternalImageType::IndexType       InternalIndexType;
  typedef typename InternalIndexType::IndexValueType  InternalIndexValueType;
  typedef typename InternalImageType::SizeType        InternalSizeType;
  typedef typename InternalSizeType::SizeValueType    InternalSizeValueType;
  typedef typename InternalImageType::RegionType      InternalRegionType;
  typedef typename InternalImageType::PixelType       InternalPixelType;
  typedef typename InternalImageType::SpacingType     InternalSpacingType;

  typedef TFeatureImage                               FeatureImageType;
  typedef typename FeatureImageType::Pointer          FeatureImagePointer;
  typedef typename FeatureImageType::SizeType         FeatureSizeType;
  typedef typename FeatureImageType::SpacingType      FeatureSpacingType;

  typedef TFeatureImage                               OutputImageType;
  typedef typename OutputImageType::Pointer           OutputImagePointer;

  typedef ScalarChanAndVeseLevelSetFunctionData< InternalImageType,
    FeatureImageType > DataHelperType;

  typedef ConstrainedRegionBasedLevelSetFunctionSharedData< InternalImageType,
    FeatureImageType, DataHelperType > SharedDataHelperType;

  typedef ScalarChanAndVeseLevelSetFunction< InternalImageType,
    FeatureImageType, SharedDataHelperType > FunctionType;
  typedef ScalarChanAndVeseSparseLevelSetImageFilter< InternalImageType,
    FeatureImageType, OutputImageType, FunctionType, SharedDataHelperType > MultiLevelSetType;
  typedef typename MultiLevelSetType::Pointer MultiLevelSetPointer;

  //arnaudgelas: itk::CellPreprocess what is supposed to do?
  //typedef itk::CellPreprocess< FeatureImageType, FeatureImageType >
  //PreprocessFilterType;
  //typedef typename PreprocessFilterType::Pointer PreprocessFilterPointer;

  typedef RegionOfInterestImageFilter<
    FeatureImageType, FeatureImageType >              ROIFilterType;
  typedef typename ROIFilterType::Pointer             ROIFilterPointer;

  typedef AtanRegularizedHeavisideStepFunction< InternalPixelType, InternalPixelType >
    DomainFunctionType;
  typedef typename DomainFunctionType::Pointer
    DomainFunctionPointer;

  typedef  FastMarchingImageFilter< InternalImageType,
    InternalImageType >                                   FastMarchingFilterType;
  typedef typename FastMarchingFilterType::NodeContainer  NodeContainer;
  typedef typename FastMarchingFilterType::NodeType       NodeType;

  typedef ImageToVTKImageFilter< InternalImageType >  ConverterType;
  typedef typename ConverterType::Pointer             ConverterPointer;

  void SetCenter( const InternalPointType& iC )
    {
    m_Center = iC;
    }

  InternalPointType GetCenter( ) const
    {
    return m_Center;
    }

  void SetRadius( const InternalCoordRepType& iR )
    {
    m_Radius = iR;
    }
  InternalCoordRepType GetRadius( ) const
    {
    return m_Radius;
    }

  void SetFeatureImage( FeatureImageType* iImage )
    {
    m_FeatureImage = iImage;
    }

  void Update()
    {
    GenerateData();
    }

  vtkImageData* GetOutput()
    {
    return m_VTKImage;
    }

  itkGetConstMacro ( Preprocess, bool );
  itkSetMacro ( Preprocess, bool );

protected:
  ChanAndVeseSegmentationFilter() : m_VTKImage( 0 ), m_FeatureImage( 0 )
    {
    m_Converter = ConverterType::New();
    m_Center.Fill( 0. );
    m_Size.Fill( 0 );
    m_Radius = 0.;
    m_Preprocess = false;
    }

  ~ChanAndVeseSegmentationFilter()  {}

  vtkImageData*         m_VTKImage;
  ConverterPointer      m_Converter;
  FeatureImagePointer   m_FeatureImage; // Raw image -- very large in size
  InternalPointType     m_Center; // Center of the cell/nucleus
  InternalSizeType      m_Size; // Level-set image size
  InternalCoordRepType  m_Radius; // Radius of the cell
  InternalImagePointer  m_Output;
  bool                  m_Preprocess;


  void GenerateData()
    {
    if( m_FeatureImage.IsNull() )
      {
      std::cerr <<"m_FeatureImage is Null" <<std::endl;
      return;
      }
    FeatureSpacingType spacing = m_FeatureImage->GetSpacing();
    FeatureSizeType inputSize = m_FeatureImage->GetLargestPossibleRegion().GetSize();

    InternalIndexType start;
    InternalPointType origin;
    InternalIndexType cen;

    for( unsigned int j = 0; j < Dimension; j++ )
      {
      m_Size[j] =
        1 + 4. * static_cast< InternalSizeValueType >( m_Radius / spacing[j] );
      start[j] = 0;
      cen[j] = static_cast< InternalSizeValueType >( 2 * m_Radius / spacing[j] );
      origin[j] = m_Center[j] - 2 * m_Radius;
      }

    std::cout << "Spacing: " << spacing << std::endl;
    std::cout << "Input Size: " << inputSize << std::endl;
    std::cout << "Output Size: " << m_Size << std::endl;
    std::cout << "Origin: " << origin << std::endl;
    std::cout << "Radius: " << m_Radius << std::endl;
    std::cout << "Center: " << cen << std::endl;

    InternalRegionType region;
    region.SetSize( m_Size );
    region.SetIndex( start );

    NodeType node;
    node.SetValue( - m_Radius/2 );
    node.SetIndex( cen );

    typename NodeContainer::Pointer seeds = NodeContainer::New();
    seeds->Initialize();
    seeds->InsertElement( 0, node );

    typename FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();
    fastMarching->SetSpeedConstant( 1.0 );
    fastMarching->SetOutputOrigin( origin );
    fastMarching->SetOutputSpacing( spacing );
    fastMarching->SetOutputRegion( region );
    fastMarching->SetTrialPoints(  seeds  );
    fastMarching->Update();

    FeatureImagePointer feature;
    if ( m_Preprocess )
      {
      ROIFilterPointer roi = ROIFilterType::New();
      roi->SetInput( m_FeatureImage );
      roi->SetRegionOfInterest( region );
      roi->Update();

      PreprocessFilterPointer preprocess = PreprocessFilterType::New();
      preprocess->SetInput ( roi->GetOutput() );
      preprocess->SetLargestCellRadius ( m_Radius ); // in real coordinates
      preprocess->Update();

      feature = preprocess->GetOutput();
      feature->SetOrigin( origin );
      }
    else
      {
      feature = m_FeatureImage;
      }

    DomainFunctionPointer domainFunction = DomainFunctionType::New();
    domainFunction->SetEpsilon( 1. );

    typedef std::vector< unsigned int > VectorType;
    VectorType lookUp( 1, 1 );

    MultiLevelSetPointer LevelSetFilter = MultiLevelSetType::New();
    LevelSetFilter->SetFunctionCount( 1 );
    LevelSetFilter->SetLookup( lookUp );
    LevelSetFilter->SetFeatureImage( feature );
    LevelSetFilter->SetLevelSet( 0, fastMarching->GetOutput() );
    LevelSetFilter->SetNumberOfIterations( 50 );
    LevelSetFilter->SetMaximumRMSError( 0 );
    LevelSetFilter->SetUseImageSpacing( 1 );
    LevelSetFilter->SetInPlace( false );

    LevelSetFilter->GetDifferenceFunction(0)->SetDomainFunction( domainFunction );
    LevelSetFilter->GetDifferenceFunction(0)->SetCurvatureWeight( 0. );
    LevelSetFilter->GetDifferenceFunction(0)->SetAreaWeight( 0. );
    LevelSetFilter->GetDifferenceFunction(0)->SetLambda1( 1. );
    LevelSetFilter->GetDifferenceFunction(0)->SetLambda2( 1. );
    LevelSetFilter->Update();

    m_Output = LevelSetFilter->GetLevelSet( 0 );

    m_Converter->SetInput( m_Output );
    m_Converter->Update();

    m_VTKImage = m_Converter->GetOutput();
    std::cout << "Output Image Size: " << m_VTKImage->GetDimensions()[0]
      << " " << m_VTKImage->GetDimensions()[1]
      << " " << m_VTKImage->GetDimensions()[2] << std::endl;
    }

private:
  ChanAndVeseSegmentationFilter( const Self& );
  void operator = ( const Self& );
};
}
#endif
