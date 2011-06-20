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

#ifndef __itkvtkMeshFilterBase_txx
#define __itkvtkMeshFilterBase_txx

#include "itkvtkMeshFilterBase.h"

namespace itk
{
template< class TFeatureImage >
vtkMeshFilterBase< TFeatureImage >::
vtkMeshFilterBase() : Superclass(),
  m_ShapeComputation( false ),
  m_IntensityComputation( false )
{
}

template< class TFeatureImage >
void
vtkMeshFilterBase< TFeatureImage >::
SetNumberOfImages( const size_t& iN )
{
  m_Images.resize( iN );
}

template< class TFeatureImage >
void
vtkMeshFilterBase< TFeatureImage >::
SetFeatureImage( const size_t& iId, FeatureImageType* iImage )
{
  if( iId < m_Images.size() )
    {
    m_Images[iId] = iImage;
    this->Modified();
    }
}

template< class TFeatureImage >
void
vtkMeshFilterBase< TFeatureImage >::
Update()
{
  this->GenerateData();
  this->ComputeOutputAttributes();
}

template< class TFeatureImage >
void
vtkMeshFilterBase< TFeatureImage >::
ComputeOutputAttributes()
{
  if( m_ShapeComputation || m_IntensityComputation )
    {
    std::vector< vtkPolyData* >::iterator it = m_Outputs.begin();
    std::vector< vtkPolyData* >::iterator end = m_Outputs.end();

    typename MeshToLabelFilterType::MeshVectorType mesh_vector( m_Outputs.size() );
    size_t i = 0;

    while( it != end )
      {
      MeshConverterPointer converter = MeshConverterType::New();
      converter->SetInput( *it );
      converter->Update();

      mesh_vector[i] = converter->GetOutput();
      mesh_vector[i]->DisconnectPipeline();
      ++i;
      ++it;
      }

    FeatureImagePointer image = FeatureImageType::New();
    image->CopyInformation( this->m_Images.front() );
    image->SetRegions( this->m_Images.front()->GetLargestPossibleRegion() );
    image->Allocate();
    image->FillBuffer( 0 );

    typename MeshToLabelFilterType::Pointer
        labelizer = MeshToLabelFilterType::New();
    labelizer->SetInput( image );
    labelizer->SetMeshes( mesh_vector );
    labelizer->Update();

    if( m_ShapeComputation )
      {
      ShapeConverterPointer shapeConverter = ShapeConverterType::New();
      shapeConverter->SetInput( image );
      shapeConverter->SetBackgroundValue ( 0 );
      shapeConverter->Update();

      m_ShapeLabelMap = shapeConverter->GetOutput();
      m_ShapeLabelMap->DisconnectPipeline();
      }
    if( m_IntensityComputation )
      {
      size_t NumberOfFeatureImages = m_Images.size();
      m_StatLabelMap.resize( NumberOfFeatureImages );

      typename std::vector< FeatureImagePointer >::iterator f_it =  m_Images.begin();
      typename std::vector< FeatureImagePointer >::iterator f_end = m_Images.end();

      i = 0;

      while( f_it != f_end )
        {
        StatConverterPointer statConverter = StatConverterType::New();
        statConverter->SetInput( image );
        statConverter->SetFeatureImage( *f_it );
        statConverter->SetBackgroundValue( 0 );
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
    }
  }
}

#endif
