/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#ifndef TRACECONTAINERBASE_H
#define TRACECONTAINERBASE_H

template< class TContainer >
class TraceContainerBase : public QObject
{
public:
  typedef TContainer MultiIndexContainerType;
  typedef typename MultiIndexContainerType::ElementType ElementType;

  typedef MultiIndexContainer::index< ActorXY >::type::iterator
  MultiIndexContainerActorXYIterator;

  typedef MultiIndexContainer::index< ActorXZ >::type::iterator
  MultiIndexContainerActorXZIterator;

  typedef MultiIndexContainer::index< ActorYZ >::type::iterator
  MultiIndexContainerActorYZIterator;

  typedef MultiIndexContainer::index< ActorXYZ >::type::iterator
  MultiIndexContainerActorXYZIterator;

  typedef MultiIndexContainer::index< Nodes >::type::iterator
  MultiIndexContainerNodesIterator;

  typedef MultiIndexContainer::index< TraceID >::type::iterator
  MultiIndexContainerTraceIDIterator;

  typedef MultiIndexContainer::index< Highlighted >::type::iterator
  MultiIndexContainerHighlightedIterator;

  typedef MultiIndexContainer::index< Visible >::type::iterator
  MultiIndexContainerVisibleIterator;

  explicit TraceContainer( QObject* iParent, QGoImageView3D* iView );
  virtual ~TraceContainer();


  MultiIndexContainerType m_Container;

  /** \brief Link to the visualization. */
  QGoImageView3D *m_ImageView;

  ElementType m_CurrentElement;

  // ----------------------------------------------------------------------

  /** \brief Print the container content in the application output */
  template< class TIterator >
  void Print(TIterator iBegin, TIterator iEnd)
  {
    TIterator it = iBegin;

    while ( it != iEnd )
      {
      std::cout << *it;
      std::cout << "***" << std::endl;
      std::cout << std::endl;
      ++it;
      }
  }

  /**
    \brief Print the container content in the application output according
    to the template parameter.
    \tparam TIndex
    */
  template< class TIndex >
  void Print()
  {
    this->Print( m_Container.get< TIndex >().begin(),
                 m_Container.get< TIndex >().end() );
  }

  /** \brief Print the container content in the application output. */
  void Print();
  // ----------------------------------------------------------------------
};

#endif // TRACECONTAINERBASE_H
