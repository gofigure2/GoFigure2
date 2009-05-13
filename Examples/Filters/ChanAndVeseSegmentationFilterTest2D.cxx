#include "itkVTKPolyDataWriter.h"

#include <iostream>

#include "itkImageFileReader.h"
#include "itkCastImageFilter.h"
#include "itkScalarChanAndVeseLevelSetFunction.h"
#include "itkScalarChanAndVeseSparseLevelSetImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkAtanRegularizedHeavisideStepFunction.h"

#include "itkImage.h"

int main( int argc, char** argv )
{
  const unsigned int Dimension = 2;
  typedef float ScalarPixelType;
  typedef itk::Image< ScalarPixelType, Dimension > LevelSetImageType;
  typedef LevelSetImageType::PointType LevelSetPointType;
  typedef LevelSetImageType::IndexType LevelSetIndexType;
  typedef LevelSetIndexType::IndexValueType LevelSetIndexValueType;
  typedef LevelSetImageType::SizeType LevelSetSizeType;
  typedef LevelSetSizeType::SizeValueType LevelSetSizeValueType;
  typedef LevelSetImageType::RegionType LevelSetRegionType;

  typedef itk::Image< ScalarPixelType, Dimension > FeatureImageType;

  typedef itk::ScalarChanAndVeseLevelSetFunction< LevelSetImageType,
    FeatureImageType > LevelSetFunctionType;
  typedef itk::ScalarChanAndVeseSparseLevelSetImageFilter< LevelSetImageType,
    FeatureImageType, LevelSetImageType, LevelSetFunctionType > MultiLevelSetType;

  typedef itk::ImageFileReader< FeatureImageType > FeatureReaderType;
  typedef itk::DanielssonDistanceMapImageFilter< LevelSetImageType,LevelSetImageType >
    DistanceFilterType;

  FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
  featureReader->SetFileName( argv[1] );
  featureReader->Update();

  double pt[3];
  pt[0] = atof( argv[2] );
  pt[1] = atof( argv[3] );
  pt[2] = atof( argv[4] );

  double cellRadius = atof( argv[5] );

  FeatureImageType::Pointer featureImage = featureReader->GetOutput();

  FeatureImageType::SpacingType spacing = featureImage->GetSpacing();
  FeatureImageType::SizeType inputSize = featureImage->GetLargestPossibleRegion().GetSize();

  LevelSetIndexType start;
  LevelSetPointType origin;
  LevelSetIndexType cen;
  LevelSetSizeType cellSize;
  for( unsigned int j = 0; j < Dimension; j++ )
  {
    cellSize[j] =
      static_cast<LevelSetSizeValueType>( 2*cellRadius/spacing[j] );
    start[j] = 0;
    origin[j] = pt[j] - cellRadius;
    cen[j] =
      static_cast<LevelSetIndexValueType>( cellRadius/spacing[j] );
  }

  LevelSetRegionType region;
  region.SetSize( cellSize );
  region.SetIndex( start );

  LevelSetImageType::Pointer levelSet = LevelSetImageType::New();
  levelSet->SetRegions( region );
  levelSet->Allocate();
  levelSet->FillBuffer( 0 );
  levelSet->SetSpacing( spacing );
  levelSet->SetOrigin( origin );
  levelSet->SetPixel( cen, 1);

  DistanceFilterType::Pointer Dist = DistanceFilterType::New();
  Dist->SetInput( levelSet );
  Dist->InputIsBinaryOn();
  Dist->SetUseImageSpacing( 1 );
  Dist->Update();

  LevelSetImageType::Pointer contourImage = Dist->GetOutput();

  typedef itk::AtanRegularizedHeavisideStepFunction< ScalarPixelType, ScalarPixelType >  DomainFunctionType;
  DomainFunctionType::Pointer domainFunction = DomainFunctionType::New();
  domainFunction->SetEpsilon( 1. );

  typedef std::vector< unsigned int > VectorType;
  VectorType lookUp( 1, 1 );

  MultiLevelSetType::Pointer levelSetFilter = MultiLevelSetType::New();
  levelSetFilter->SetFunctionCount( 1 );
  levelSetFilter->SetLookup( lookUp );
  levelSetFilter->SetFeatureImage( featureImage );
  levelSetFilter->SetLevelSet( 0, contourImage );
  levelSetFilter->SetNumberOfIterations( 50 );
  levelSetFilter->SetMaximumRMSError( 0 );
  levelSetFilter->SetUseImageSpacing( 0 );

  levelSetFilter->GetDifferenceFunction(0)->SetDomainFunction( domainFunction );
  levelSetFilter->GetDifferenceFunction(0)->SetCurvatureWeight( 0. );
  levelSetFilter->GetDifferenceFunction(0)->SetAreaWeight( 0. );
  levelSetFilter->GetDifferenceFunction(0)->SetLambda1( 1. );
  levelSetFilter->GetDifferenceFunction(0)->SetLambda2( 1. );
  levelSetFilter->GetOutput();

  return EXIT_SUCCESS;
}
