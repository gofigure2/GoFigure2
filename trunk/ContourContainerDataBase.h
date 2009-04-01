#ifndef __ContourContainerDataBase_h
#define __ContourContainerDataBase_h

template< class TContainer >
class ContourContainerDataBase : public ContourContainerBase< TContainer >
{
public:
  typedef ContourContainerDataBase Self;
  typedef ContourContainerBase< TContainer > Superclass;

  typedef TContainer ContainerType;
  typedef typename ContainerType::iterator ContainerIterator;
  typedef typename ContainerType::const_iterator ContainerConstIterator;

  ContourContainerDataBase();
  ~ContourContainerDataBase();

  //To be implemented
  void SaveAllContours( ContainerType& iContainer )
  {
  }

  void LoadAllContours( ContainerType& iContainer )
  {

  }

private:
  ContourContainerDataBase( const Self& );
  void operator = ( const Self& );

};
#endif
