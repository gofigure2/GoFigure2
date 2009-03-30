#ifndef __ContourContainerBase_h
#define __ContourContainerBase_h

template< class TContainer >
class ContourContainerBase
{
public:
  typedef ContourContainerBase Self;

  typedef TContainer ContainerType;
  typedef typename ContainerType::iterator ContainerIterator;
  typedef typename ContainerType::const_iterator ContainerConstIterator;

  ContourContainerBase( );
  ~ContourContainerBase( );

  virtual void SaveAllContours( ContainerType& iContainer ) = 0;
  virtual void LoadAllContours( ContainerType& iContainer ) = 0;

private:
  ContourContainerBase( const Self& );
  void operator = ( const Self& );

};
#endif
