/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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
#include "QGoSeedsSegmentation.h"

// VTK
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"
#include "vtkMarchingSquares.h"
#include "vtkStripper.h"
#include "vtkPolylineDecimation.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkMarchingCubes.h"
#include "vtkMath.h"
#include "vtkSphereSource.h"

#include "vtkPointData.h"

//Connection VTK/ITK
#include "vtkImageExport.h"
#include "itkVTKImageImport.h"
#include "itkVTKImageToImageFilter.h"
#include "vtkitkAdaptor.h"

// ITK filter
#include "itkChanAndVeseSegmentationFilter.h"
#include "itkImage.h"

//--------------------------------------------------------------------------
QGoSeedsSegmentation::
QGoSeedsSegmentation()
{
  m_OriginImage    = vtkSmartPointer<vtkImageData>::New();

  m_OutputPolyData = vtkSmartPointer<vtkPolyData>::New();

  m_SeedsPosition[0] = 0.;
  m_SeedsPosition[1] = 0.;
  m_SeedsPosition[2] = 0.;

  m_Radius = 0.;

  m_NumberOfIterations = 0;

  m_CurvatureWeight = 0;

  m_OriginImageInformations = vtkSmartPointer<vtkViewImage2D> ::New();
}
//--------------------------------------------------------------------------
QGoSeedsSegmentation::
~QGoSeedsSegmentation()
{
}
//--------------------------------------------------------------------------
void
QGoSeedsSegmentation::
setInputVolume( vtkImageData* iInputVolume )
{
  m_OriginImage = iInputVolume;
}
//--------------------------------------------------------------------------
void
QGoSeedsSegmentation::
setSeedsPosition( double iSeedsPosition[3] )
{
  m_SeedsPosition[0] = iSeedsPosition[0];
  m_SeedsPosition[1] = iSeedsPosition[1];
  m_SeedsPosition[2] = iSeedsPosition[2];
}
//--------------------------------------------------------------------------
void
QGoSeedsSegmentation::
setRadius( double iRadius )
{
  m_Radius = iRadius;
}
//--------------------------------------------------------------------------
void
QGoSeedsSegmentation::
setNumberOfIterations( int iNumberOfIterations )
{
  m_NumberOfIterations = iNumberOfIterations;
}
//--------------------------------------------------------------------------
void
QGoSeedsSegmentation::
setCurvatureWeight( int iCurvatureWeight )
{
  m_CurvatureWeight = iCurvatureWeight;
}

//--------------------------------------------------------------------------
void
QGoSeedsSegmentation::
setOriginImageInformation( vtkViewImage2D* iOriginImageInformation )
{
  m_OriginImageInformations = iOriginImageInformation;
}
//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSeedsSegmentation::
output()
{
  return m_OutputPolyData;
}
//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSeedsSegmentation::
LevelSetSegmentation2D(int iDirection)
{
  // Not implemented yet
  (void)iDirection;

  // To be adapted depending on choosen orientation
  // Matrices for axial, coronal, sagittal view orientations
  static double elements[3][16] = {{
         1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1 },{
         1, 0, 0, 0,
         0, 0, 1, 0,
         0,-1, 0, 0,
         0, 0, 0, 1 },{
         0, 0,-1, 0,
         1, 0, 0, 0,
         0,-1, 0, 0,
         0, 0, 0, 1 }};

  // Set the slice orientation
  vtkSmartPointer<vtkMatrix4x4> resliceAxes =
      vtkSmartPointer<vtkMatrix4x4>::New();
  resliceAxes->DeepCopy(elements[0]);
  // Set the point through which to slice
  resliceAxes->SetElement(0, 3, m_SeedsPosition[0]);
  resliceAxes->SetElement(1, 3, m_SeedsPosition[1]);
  resliceAxes->SetElement(2, 3, m_SeedsPosition[2]);

  vtkSmartPointer<vtkImageReslice> reslicer =
      vtkSmartPointer<vtkImageReslice>::New();
  reslicer->SetOutputDimensionality(2);
  reslicer->SetInterpolationModeToLinear();
  reslicer->SetInput( m_OriginImage );
  reslicer->SetResliceAxes(resliceAxes);
  reslicer->Update();

  //Export VTK image to ITK
  vtkSmartPointer<vtkImageExport> movingExporter =
    vtkSmartPointer<vtkImageExport>::New();
  movingExporter->SetInput( reslicer->GetOutput() );
  movingExporter->Update();

  const unsigned int Dimension = 2;

  // ImageType
  typedef itk::Image< unsigned char, Dimension > ImageType;
  // Import VTK Image to ITK
  typedef itk::VTKImageImport<ImageType> ImageImportType;
  typedef ImageImportType::Pointer ImageImportPointer;
  ImageImportPointer movingImporter = ImageImportType::New();

  ConnectPipelines< vtkImageExport, ImageImportPointer >( 
    movingExporter, 
    movingImporter );
  // Apply LevelSet segmentation filter
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;

  typedef itk::ChanAndVeseSegmentationFilter< FeatureImageType >
    SegmentationFilterType;

  FeatureImageType::PointType pt;

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();
  filter->SetFeatureImage( movingImporter->GetOutput() );
  filter->SetPreprocess( 1 );

  // everything is in world coordinates
  // need to add newOrigin since origin moves when we extract slice
  // everything is in world coordinates

  double* newOrigin = reslicer->GetOutput()->GetOrigin();
  pt[0] = m_SeedsPosition[0]+newOrigin[0];
  pt[1] = m_SeedsPosition[1]+newOrigin[1];
  filter->SetCenter( pt );

  filter->SetRadius( m_Radius );
  filter->SetNumberOfIterations( m_NumberOfIterations );
  filter->SetCurvatureWeight( m_CurvatureWeight );
  filter->Update();

  // create iso-contours
  vtkSmartPointer<vtkMarchingSquares> contours =
    vtkSmartPointer<vtkMarchingSquares>::New();
  contours->SetInput( filter->GetOutput() );
  contours->GenerateValues ( 1, 0, 0 );

  // Create reorganize contours
  vtkSmartPointer<vtkStripper> stripper =
    vtkSmartPointer<vtkStripper>::New();
  stripper->SetInput( contours->GetOutput() );
  //Is it useful?? Which number is the best suited?
  stripper->SetMaximumLength( 999 );
  stripper->Update();

  // Reorder points
  stripper->GetOutput()->GetLines()->InitTraversal();

  // npts = nb of points in the line
  // *pts = pointer to each point

  vtkIdType *pts = NULL;
  vtkIdType npts = 0;
  stripper->GetOutput()->GetLines()->GetNextCell(npts,pts);
  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();

  vtkCellArray *lines       = vtkCellArray::New();
  vtkIdType    *lineIndices = new vtkIdType[static_cast<int>(npts+1)];

  for ( int k = 0; k< static_cast<int>( npts ); k++ )
    {
    points->InsertPoint(k, stripper->GetOutput()->GetPoints()->GetPoint(pts[k]));
    lineIndices[ k ] = k;
    }

  lineIndices[ static_cast<int>(npts) ] = 0;
  lines->InsertNextCell( npts + 1, lineIndices);
  delete [] lineIndices;

  vtkSmartPointer<vtkPolyData> testPolyD =
    vtkSmartPointer<vtkPolyData>::New();
  testPolyD->SetPoints( points );
  testPolyD->SetLines( lines );

  lines->Delete();

  //Decimation (has to be after points reorganization)
  vtkSmartPointer<vtkPolylineDecimation> decimator =
    vtkSmartPointer<vtkPolylineDecimation>::New();
  decimator->SetInput( testPolyD );

  /// \todo instead os setting it to 0.9, compute it to make 10 to 20 control points
  decimator->SetTargetReduction( 0.9 );
  decimator->Update();

  // Translate to real location (i.e. see_pos[])
  vtkSmartPointer<vtkTransform> t =
    vtkSmartPointer<vtkTransform>::New();
  t->Translate( m_SeedsPosition[0], m_SeedsPosition[1], m_SeedsPosition[2] );

  vtkSmartPointer<vtkTransformPolyDataFilter> tf =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  tf->SetTransform( t );
  tf->SetInput( decimator->GetOutput() );

  // Update required here
  tf->Update();

  return tf->GetOutput();
}

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSeedsSegmentation::
LevelSetSegmentation3D()
{
//Export VTK image to ITK
  vtkSmartPointer<vtkImageExport> movingExporter =
      vtkSmartPointer<vtkImageExport>::New();

  movingExporter->SetInput( m_OriginImage );

  const unsigned int Dimension = 3;

  // ImageType
  typedef itk::Image< unsigned char, Dimension > ImageType;
  // Import VTK Image to ITK
  typedef itk::VTKImageImport<ImageType> ImageImportType;
  ImageImportType::Pointer movingImporter = ImageImportType::New();

  ConnectPipelines< vtkImageExport, ImageImportType::Pointer >( movingExporter, movingImporter );
  // Apply LevelSet segmentation filter
  typedef itk::Image< unsigned char, Dimension > FeatureImageType;

  typedef itk::ChanAndVeseSegmentationFilter< FeatureImageType >
      SegmentationFilterType;

  FeatureImageType::PointType pt;

  SegmentationFilterType::Pointer filter = SegmentationFilterType::New();
  filter->SetFeatureImage( movingImporter->GetOutput() );
  filter->SetPreprocess( 1 );

  // everything is in world coordinates
  pt[0] = m_SeedsPosition[0];
  pt[1] = m_SeedsPosition[1];
  pt[2] = m_SeedsPosition[2];
  filter->SetCenter( pt );

  filter->SetRadius( m_Radius );
  filter->SetNumberOfIterations( m_NumberOfIterations );
  filter->SetCurvatureWeight( m_CurvatureWeight );
  filter->Update();

  // create iso-contours
  vtkSmartPointer<vtkMarchingCubes> contours =
      vtkSmartPointer<vtkMarchingCubes>::New();
  contours->SetInput( filter->GetOutput() );
  contours->GenerateValues ( 1, 0, 0 );
  contours->SetComputeGradients( 0 );
  contours->SetComputeNormals( 0 );
  contours->SetComputeScalars( 0 );
  contours->SetNumberOfContours( 1 );

  //Update required here!!
  contours->Update();

  return contours->GetOutput();
}

//--------------------------------------------------------------------------
std::vector< vtkSmartPointer<vtkPolyData> >
QGoSeedsSegmentation::
SphereContoursSegmentation()
{
  int* center_id = m_OriginImageInformations
      ->GetImageCoordinatesFromWorldCoordinates( m_SeedsPosition );

  double corner[3];
  corner[0] = m_SeedsPosition[0];
  corner[1] = m_SeedsPosition[1];
  corner[2] = m_SeedsPosition[2] - m_Radius;

  int* corner_id = m_OriginImageInformations
      ->GetImageCoordinatesFromWorldCoordinates( corner );
  int zlength = 2 * std::abs( center_id[2] - corner_id[2] );

  int idx[3];
  idx[0] = corner_id[0];
  idx[1] = corner_id[1];
  idx[2] = corner_id[2];

  double* seed_pos;

  std::vector< vtkSmartPointer<vtkPolyData> >              circleContoursVector;
  circleContoursVector.resize( zlength );

  // numberOfPointsToRepresentCircle: 4 is enough since there is an interpolation
  // to create the circle after

  int numberOfPointsToRepresentCircle = 4;

  for( int i = 0; i < zlength; i++, idx[2]++ )
    {
    seed_pos = m_OriginImageInformations
        ->GetWorldCoordinatesFromImageCoordinates( idx );

    vtkSmartPointer<vtkPolyData> circle =
        GenerateCircleFromGivenSphereAndGivenZ(
            m_SeedsPosition, m_Radius, seed_pos[2],
                numberOfPointsToRepresentCircle );

    // Store polyDatas in a vector then return it
    circleContoursVector[i] = circle;
    }

  return circleContoursVector;
}

//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSeedsSegmentation::
GenerateCircleFromGivenSphereAndGivenZ( double iC[3],
  const double& iRadius, double iZ, const int& iN )
{
  double res = ( iC[2] - iZ );
  res *= res;
  res = iRadius * iRadius - res;

  if( res < 0 )
    {
    return 0;
    }
  else
    {
    vtkSmartPointer<vtkPolyData> oCircle = vtkSmartPointer<vtkPolyData>::New();
    vtkPoints    *points      = vtkPoints::New();
    vtkCellArray *lines       = vtkCellArray::New();
    vtkIdType    *lineIndices = new vtkIdType[iN+1];

    double theta = 0.;
    double r = sqrt( res );

    for( int i = 0; i < iN; i++ )
      {
      theta = 2. * i * vtkMath::Pi() / static_cast< double >( iN );
      points->InsertPoint( static_cast< vtkIdType>( i ),
          iC[0] + r * cos( theta ), iC[1] + r * sin( theta ), iZ );
      lineIndices[i] = static_cast<vtkIdType>(i);
      }

    lineIndices[iN] = 0;
    lines->InsertNextCell(iN+1,lineIndices);
    delete [] lineIndices;
    oCircle->SetPoints( points );
    oCircle->SetLines(lines);

    return oCircle;
    }
}
//--------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData>
QGoSeedsSegmentation::
SphereVolumeSegmentation()
{
    // create sphere geometry
  vtkSmartPointer<vtkSphereSource> sphere =
      vtkSmartPointer<vtkSphereSource>::New();
  sphere->SetRadius( m_Radius );
  sphere->SetThetaResolution( 18 );
  sphere->SetPhiResolution( 18 );
  sphere->SetCenter( m_SeedsPosition );
  sphere->Update();
  sphere->GetOutput()->GetPointData()->SetNormals( NULL );

  return sphere->GetOutput();
}

