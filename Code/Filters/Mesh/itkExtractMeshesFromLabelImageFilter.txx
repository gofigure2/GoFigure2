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

#ifndef __itkExtractMeshesFromLabelImageFilter_txx
#define __itkExtractMeshesFromLabelImageFilter_txx

#include "itkExtractMeshesFromLabelImageFilter.h"

static itk::SimpleFastMutexLock m_Mutex;

namespace itk
{

template< class TImage, class TFeatureImage >
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >
::ExtractMeshesFromLabelImageFilter()
{
  m_Input = 0;
  m_NumberOfMeshes = 0;
  m_NumberOfTrianglesPerMesh = 200;
  m_NumberOfSmoothingIterations = 15;
  m_SmoothingRelaxationFactor = 1.0;
  m_DelaunayConforming = true;
  m_NumberOfThreads = 1;
  m_UseSmoothing = true;
  m_UseDecimation = true;
  m_ComputeIntensityStatistics = false;
}

template< class TImage, class TFeatureImage >
void
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
SetNumberOfFeatureImages( const size_t& iN )
{
  m_FeatureImages.resize( iN );
}

template< class TImage, class TFeatureImage >
void
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
SetFeatureImage( const size_t& iId, FeatureImageType* iF )
{
  if( iId < m_FeatureImages.size() )
    {
    m_FeatureImages[iId] = iF;
    }
}

template< class TImage, class TFeatureImage >
void
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
Update()
{
  if( m_Input.IsNull() )
    {
    itkGenericExceptionMacro( << "Input is NULL" );
    }
  this->GenerateData();
}

template< class TImage, class TFeatureImage >
typename ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::MeshVectorType
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
GetOutputs()
{
  return m_Meshes;
}

template< class TImage, class TFeatureImage >
void
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
GenerateData()
{
  ShapeConverterPointer shapeConverter = ShapeConverterType::New();
  shapeConverter->SetInput ( m_Input );
  shapeConverter->SetBackgroundValue ( 0 );
  shapeConverter->Update();

  m_ShapeLabelMap = shapeConverter->GetOutput();
  m_ShapeLabelMap->DisconnectPipeline();

  m_NumberOfMeshes = m_ShapeLabelMap->GetNumberOfLabelObjects();

  this->m_Meshes.resize( m_NumberOfMeshes );

  if( m_ComputeIntensityStatistics )
    {
    size_t NumberOfFeatureImages = m_FeatureImages.size();

    if( NumberOfFeatureImages == 0 )
      {
      itkGenericExceptionMacro( <<"No Feature Image Provided" );
      }
    m_StatLabelMap.resize( NumberOfFeatureImages );

    typename FeatureImageVectorType::iterator f_it =  m_FeatureImages.begin();
    typename FeatureImageVectorType::iterator f_end = m_FeatureImages.end();

    size_t i = 0;

    while( f_it != f_end )
      {
      StatConverterPointer statConverter = StatConverterType::New();
      statConverter->SetInput( m_Input );
      statConverter->SetFeatureImage( *f_it );
      statConverter->SetBackgroundValue ( 0 );
      statConverter->SetComputePerimeter(false);

      try
        {
        statConverter->Update();
        }
      catch(itk::ExceptionObject & e)
        {
        std::cerr << "Exception Caught: " << e << std::endl;
        std::cerr << "statConverter->Update()" << std::endl;
        return;
        }

      m_StatLabelMap[i] = statConverter->GetOutput();
      m_StatLabelMap[i]->DisconnectPipeline();

      ++i;
      ++f_it;
      }
    }

  ThreadStruct str( this );

  ThreaderPointer threader = ThreaderType::New();
  threader->SetNumberOfThreads( m_NumberOfThreads );
  threader->SetSingleMethod( this->ThreaderCallback, &str );
  threader->SingleMethodExecute();
}

template< class TImage, class TFeatureImage >
ITK_THREAD_RETURN_TYPE
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
ThreaderCallback(void * arg)
{
  unsigned int ThreadId =
    ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;

  ThreadStruct * str =
    (ThreadStruct *) (((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  unsigned int numOfMeshes = str->Filter->m_NumberOfMeshes;
  unsigned int numOfThreads = str->Filter->m_NumberOfThreads;

  unsigned int MeshChunk = numOfMeshes/ numOfThreads;
  ++MeshChunk;

  unsigned int startLabel = ThreadId * MeshChunk + 1;
  unsigned int endLabel = ThreadId * MeshChunk + MeshChunk;

  if ( ThreadId == numOfThreads-1 )
    {
    endLabel = numOfMeshes;
    }

  str->Filter->ThreadedExtractMesh( startLabel, endLabel );

  return ITK_THREAD_RETURN_VALUE;
}


template< class TImage, class TFeatureImage >
void
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >::
ThreadedExtractMesh( const unsigned int& startLabel,
                     const unsigned int& endLabel )
{
  LabelObjectContainerType container = m_ShapeLabelMap->GetLabelObjectContainer();

  SizeType size;
  IndexType index;
  RegionType region;
  SizeType inputSize = m_Input->GetLargestPossibleRegion().GetSize();

  LabelObjectIterator l_it = container.begin();

  for ( unsigned int j = 1; j < startLabel; ++j, ++l_it )
    {}

  MeshPointer mesh, smoothMesh, decimatedMesh;

  LabelType label = startLabel;
  LabelObjectIterator l_end = container.end();

  while( ( label <= endLabel ) && ( l_it != l_end ) )
    {
    LabelType i = l_it->first;
    region = m_ShapeLabelMap->GetLabelObject ( i )->GetBoundingBox();
    index = region.GetIndex();
    size = region.GetSize();

    for( unsigned int j = 0; j < ImageDimension; ++j )
      {
      if ( static_cast<IndexValueType>( index[j] ) > 0 )
        {
        index[j] = static_cast<IndexValueType>( index[j] - 1 );
        size[j] = static_cast<SizeValueType>( size[j] + 1 );
        }
      if ( static_cast<SizeValueType>( index[j] + size[j] ) < inputSize[j] )
        {
        size[j] = static_cast<SizeValueType>( size[j] + 1 );
        }
      }
    region.SetIndex( index );
    region.SetSize( size );

    // Extract mesh
    MeshSourcePointer meshSource = MeshSourceType::New();
    meshSource->SetInput( m_Input );
    meshSource->SetRegionOfInterest ( region );
    meshSource->SetObjectValue( i );
    meshSource->Update();

    mesh = meshSource->GetOutput();
    mesh->DisconnectPipeline();

    // Smooth the mesh
    if ( m_UseSmoothing )
      {
      ConformalMatrixCoefficients< MeshType > coeff0;

      MeshSmoothingPointer smoothingFilter = MeshSmoothingType::New( );
      smoothingFilter->SetInput( mesh );
      smoothingFilter->SetNumberOfIterations( m_NumberOfSmoothingIterations );
      smoothingFilter->SetRelaxationFactor( m_SmoothingRelaxationFactor );
      smoothingFilter->SetDelaunayConforming( m_DelaunayConforming );
      smoothingFilter->SetCoefficientsMethod( &coeff0 );
      smoothingFilter->Update();

      smoothMesh = smoothingFilter->GetOutput();
      smoothMesh->DisconnectPipeline();
      }
    else
      {
      smoothMesh = mesh;
      }

    // Check topology

    // Decimate the mesh
    if ( m_UseDecimation )
      {
      CriterionPointer criterion = CriterionType::New();
      criterion->SetTopologicalChange( true );
      criterion->SetNumberOfElements( m_NumberOfTrianglesPerMesh );

      DecimationPointer decimate = DecimationType::New();
      decimate->SetInput( smoothMesh );
      decimate->SetCriterion( criterion );
      decimate->Update();

      decimatedMesh = decimate->GetOutput();
      decimatedMesh->DisconnectPipeline();
      }
    else
      {
      decimatedMesh = smoothMesh;
      }

    m_Meshes[label-1] = decimatedMesh;
    m_Meshes[label-1]->DisconnectPipeline();

    m_MeshtoLabelIdMap.insert( std::pair< size_t, LabelType >( label - 1, i ) );

    ++l_it;
    ++label;
  }
}


/** Print Self information */
template< class TImage, class TFeatureImage >
void
ExtractMeshesFromLabelImageFilter< TImage, TFeatureImage >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "NumberOfMeshes: " << m_NumberOfMeshes << std::endl;
  os << indent << "NumberOfTrianglesPerMesh: " << m_NumberOfTrianglesPerMesh << std::endl;
  os << indent << "NumberOfSmoothingIterations: " << m_NumberOfSmoothingIterations << std::endl;
  os << indent << "SmoothingRelaxationFactor: " << m_SmoothingRelaxationFactor << std::endl;
}

} // end namespace

#endif
