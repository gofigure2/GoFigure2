#ifndef __ContourContainerFileSystem_h
#define __ContourContainerFileSystem_h

// #include "itkArchetypeSeriesFileName.h"
#include "ContourContainerBase.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "QImagePageViewTracer.h"

/**
  BaseName_id@@cellid@@_m@@minXidx@@_@@minYidy@@_@@minZidz_M@@maxXidx@@_@@maxYidy@@_@@maxZidz@@
  example:
    contour_id1_m369_310_21_M644_638_21.vtk
    id = 1
    minXidx = 369
    minYidy = 310
    minZidz = 21
    maxXidx = 644
    maxYidy = 638
    maxZidz = 21
*/
template< class TContainer >
class ContourContainerFileSystem : public ContourContainerBase< TContainer >
{
public:
  typedef ContourContainerFileSystem Self;
  typedef ContourContainerBase< TContainer > Superclass;

  typedef TContainer ContainerType;
  typedef typename ContainerType::iterator ContainerIterator;
  typedef typename ContainerType::const_iterator ContainerConstIterator;

  ContourContainerFileSystem( )
  {}
  ~ContourContainerFileSystem()
  {}

//   void SetGrouping( const size_t& iGrouping )
//   {
//     m_Grouping = iGrouping;
//   }


  void SaveAllContours( const unsigned int& iId,
    ContainerType& iContainer, QImagePageViewTracer* iView )
  {
    //DATABASE
//     for( ContainerIterator c_it = iContainer.begin();
//         c_it != iContainer.end();
//         ++c_it )
//     {
//       double bounds[6];
//       (*c_it)->GetBounds( bounds );
//
//       double Min[3], Max[3];
//       Min[0] = bounds[0];
//       Max[0] = bounds[1];
//       Min[1] = bounds[2];
//       Max[1] = bounds[3];
//       Min[2] = bounds[4];
//       Max[2] = bounds[5];
//
//       int* min_idx = iView->GetImageCoordinatesFromWorldCoordinates( Min );
//       int* max_idx = iView->GetImageCoordinatesFromWorldCoordinates( Max );
//
//       QString identifier = QString( "_id%1" ).arg( iId );
//       QString MinString = QString( "_m%1_%2_%3" ).arg( min_idx[0] ).arg( min_idx[1] ).arg( min_idx[2] );
//       QString MaxString = QString( "_M%1_%2_%3" ).arg( max_idx[0] ).arg( max_idx[1] ).arg( max_idx[2] );
//
//       QString filename = QString( "%1%2%3%4%5" ).arg( "Contour" )
//         .arg( identifier ).arg( MinString ).arg( MaxString ).arg( ".vtk" );
//
//       vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
//       writer->SetInput( *c_it );
//       writer->SetFileName( filename.toAscii().constData() );
//       writer->Write();
//
//       writer->Delete();
//     }
  }

  void LoadAllContours( ContainerType& iContainer )
  {
//     iContainer.clear();
//
//     itk::ArchetypeSeriesFileNames::Pointer fit =
//       itk::ArchetypeSeriesFileNames::New();
//     fit->SetArchetype( m_BaseName );
//
//     std::vector< std::string > filenames = fit->GetFileNames( m_Grouping );
//
//     for( std::vector< std::string >::const_iterator it = filenames.begin();
//           it != filenames.end();
//          it++ )
//     {
//       vtkPolyDataReader* reader = vtkPolyDataReader::New();
//       reader->SetFileName( (*it).c_str() );
//       reader->Update();
//
//       iContainer.push_back( reader->GetOutput() );
//       reader->Delete();
//     }
  }

  void LoadAllContours( ContainerType& iContainer, const size_t& iGrouping )
  {
//     SetGrouping( iGrouping );
//     LoadAllContours( iContainer );
  }

protected:
  size_t m_Gouping;

private:
  ContourContainerFileSystem( const Self& );
  void operator = ( const Self& );

};
#endif
