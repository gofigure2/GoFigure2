#ifndef __ContourContainerFileSystem_h
#define __ContourContainerFileSystem_h

#include "itkArchetypeSeriesFileName.h"


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

  ContourContainerFileSystem( const std::string& iBaseName ) :
    m_BaseName( iBaseName )
  {}
  ~ContourContainerFileSystem()
  {}

  void SetGrouping( const size_t& iGrouping )
  {
    m_Grouping = iGrouping;
  }


  void SaveAllContours( ContainerType& iContainer )
  {

  }

  void LoadAllContours( ContainerType& iContainer )
  {
    iContainer.clear();

    itk::ArchetypeSeriesFileNames::Pointer fit =
      itk::ArchetypeSeriesFileNames::New();
    fit->SetArchetype( m_BaseName );

    std::vector< std::string > filenames = fit->GetFileNames( m_Grouping );

    for( std::vector< std::string >::const_iterator it = filenames.begin();
          it != filenames.end();
         it++ )
    {
      vtkPolyDataReader* reader = vtkPolyDataReader::New();
      reader->SetFileName( (*it).c_str() );
      reader->Update();

      iContainer.push_back( reader->GetOutput() );
      reader->Delete();
    }
  }

  void LoadAllContours( ContainerType& iContainer, const size_t& iGrouping )
  {
    SetGrouping( iGrouping );
    LoadAllContours( iContainer );
  }

protected:
  std::string m_BaseName;
  size_t m_Gouping;

private:
  ContourContainerFileSystem( const Self& );
  void operator = ( const Self& );

};
#endif
