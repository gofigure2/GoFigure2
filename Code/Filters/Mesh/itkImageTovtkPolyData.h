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

#ifndef __itkImageTovtkPolyData_h
#define __itkImageTovtkPolyData_h

namespace itk
{
template< class TImage >
class ImageTovtkPolyData : public LightObject
{
public:
  typedef ImageTovtkPolyData Self;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  typedef LightObject Superclass;

  typedef TImage ImageType;
  typedef typename ImageType::Pointer ImagePointer;
  typedef typename ImageType::PointType ImagePointType;
  typedef typename ImagePointType::CoordRepType ImageCoordType;

  typedef Image< unsigned char, 3 > BinaryImageType;
  typedef typename BinaryImageType::Pointer BinaryImagePointer;

  void SetImage( ImageType* input );
  void Update();

  template< class TContainer >
  void SetThresholdValues( const TContainer& iVal )
    {
    m_Thresholds.resize( iVal.size() );

    size_t i = 0;
    typename TContainer::const_iterator it = iVal.begin();

    while( it != iVal.end() )
      {
      m_Thresholds[i] = static_cast< double >( *it );
      ++it;
      ++i;
      }
    this->Modified();
    }

protected:
  ImageTovtkPolyData();
  ~ImageTovtkPolyData();

  ImagePointer m_Image;
  std::vector< double > m_Thresholds;

  void GenerateData()
    {
    typedef QuadEdgeMesh< ImageCoordType, 3 > InternalMeshType;
    typedef BinaryMask3DMeshSource< ImageType, InternalMeshType > MeshingTechniqueType;

    typedef BinaryThresholdImageFilter< ImageType, BinaryImageType > TresholdFilterType;

    typename TresholdFilterType::Pointer thresholding = TresholdFilterType::New();
    thresholding->SetInput( m_Image );

    std::vector< double >::iterator it = m_Thresholds.begin();

    while( it != m_Thresholds.end() )
      {
      thresholding->SetLowerThreshold();

      typename MeshingTechniqueType::Pointer meshing = MeshingTechniqueType::New();
      meshing->SetInput( m_Image );
      meshing->Update();

      mesh[i] = meshing->Update();

      // smooth 15 times

      // decimation 200 triangles
      typedef NumberOfFacesCriterion< InternalMeshType > CriterionType;

      typename CriterionType::Pointer criterion = CriterionType::New();
      criterion->SetTopologicalChange( true );
      criterion->SetNumberOfElements( 200 );

      typedef SquaredEdgeLengthDecimationQuadEdgeMeshFilter< InternalMeshType,
          InternalMeshType, CriterionType > MeshDecimationType;

      typename MeshDecimationType::Pointer decimating =
          MeshDecimationType::New();
      decimating->SetInput( meshing->Update() );
      decimating->SetCriterion( criterion );
      decimating->Update();

      mesh[i] = decimating->GetOutput();

      ++it;
      }
    }


private:
  ImageTovtkPolyData( const Self& );
  void operator = ( const Self& );
};
}
#endif // __itkImageTovtkPolyData_h
