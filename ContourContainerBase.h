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

  //NOTE it is really dirty to use QImagePageViewTracer
  virtual void SaveAllContours( const unsigned int& iId,
    ContainerType& iContainer,
    QImagePageViewTracer* view ) = 0;
  virtual void LoadAllContours( ContainerType& iContainer ) = 0;

private:
  ContourContainerBase( const Self& );
  void operator = ( const Self& );

};
#endif
