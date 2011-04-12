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

#include "LineageContainer.h"
#include "itkMacro.h"

//-------------------------------------------------------------------------
LineageContainer::
LineageContainer(QObject *iParent,QGoImageView3D *iView):Superclass(iParent, iView)
{}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
LineageContainer::
~LineageContainer()
{
  MultiIndexContainerType::iterator it = m_Container.begin();

  while ( it != m_Container.end() )
    {
    it->ReleaseData();
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void LineageContainer::
InsertNewLineage(const unsigned int& iLineageID,
    double irgba[4], const unsigned int& iTrackIDRoot,
    const bool& IsVisible)
{
  MultiIndexContainerElementType NewElement;
  NewElement.TraceID = iLineageID;
  NewElement.rgba[0] = irgba[0];
  NewElement.rgba[1] = irgba[1];
  NewElement.rgba[2] = irgba[2];
  NewElement.rgba[3] = irgba[3];
  NewElement.TrackRootID = iTrackIDRoot;
  NewElement.Visible = IsVisible;
  this->Insert(NewElement);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool LineageContainer::DeleteElement(const unsigned int & iId)
{
  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool LineageContainer::DeleteElement(MultiIndexContainerTraceIDIterator iIter)
{
  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > LineageContainer::DeleteAllHighlightedElements()
{
  return std::list< unsigned int >();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< vtkActor* > LineageContainer::AddTrace( vtkPolyData* , vtkProperty* )
{
  return std::vector< vtkActor* >();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> LineageContainer::GetListOfTrackRootIDs()
{
  std::list<unsigned int> listOfTrackIDs;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().begin();

  while( it != m_Container.get< TraceID >().end() )
    {
    listOfTrackIDs.push_back(it->TrackRootID);
    ++it;
    }

  return listOfTrackIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> LineageContainer::GetListOfLineageIDs()
{
  std::list<unsigned int> listOfLineageIDs;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().begin();

  while( it != m_Container.get< TraceID >().end() )
    {
    listOfLineageIDs.push_back(it->TraceID);
    ++it;
    }

  return listOfLineageIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int LineageContainer::GetLineageTrackRootID( const unsigned int& iTraceID )
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find( iTraceID );

  if( it != m_Container.get< TraceID >().end() )
    {
    return it->TrackRootID;
    }
  else
    {
    itkGenericExceptionMacro( <<"iTraceId is not in this container" );
    return 0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool LineageContainer::GetLineageVisibile( const unsigned int& iTraceID )
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find( iTraceID );

  if( it != m_Container.get< TraceID >().end() )
    {
    return it->Visible;
    }
  else
    {
    itkGenericExceptionMacro( <<"iTraceId is not in this container" );
    return false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool LineageContainer::GetLineageHighlighted( const unsigned int& iTraceID )
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find( iTraceID );

  if( it != m_Container.get< TraceID >().end() )
    {
    return it->Highlighted;
    }
  else
    {
    itkGenericExceptionMacro( <<"iTraceId is not in this container" );
    return false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double*
LineageContainer::
GetLineageColor( const unsigned int& iTraceID )
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find( iTraceID );

  double* color = NULL;

  if( it != m_Container.get< TraceID >().end() )
    {
    color = const_cast<double*>(it->rgba);
    }

  return color;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
LineageContainer::
UpdateElementHighlightingWithGivenTraceIDs( const QStringList & iList,
                                            const Qt::CheckState & iCheck)
{
  std::cout << "highlight lineage list not tested" << std::endl;
  // emit signal for each lineage to be highlighted
  // signal contains the trackIDroot and the state
  if ( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;
    QStringList::const_iterator constIterator = iList.begin();

    bool highlighted = iCheck;

    while ( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find( ( *constIterator ).toUInt() );

      if ( it != m_Container.get< TraceID >().end() )
        {
        // modify the structure highlight
        m_Container.get< TraceID >().
            modify( it , change_highlighted<MultiIndexContainerElementType>(highlighted) );
        //send signal to track container
        emit HighlightLineage(it->TraceID, highlighted);
        }
      ++constIterator;
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
LineageContainer::UpdateElementVisibilityWithGivenTraceIDs(const QStringList & iList,
                                                         const Qt::CheckState & iCheck)
{
  std::cout << "visible lineage list not tested" << std::endl;
  // emit signal for each lineage to be shown/hidden
  // signal contains the trackIDroot and the state

  if ( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;
    QStringList::const_iterator constIterator = iList.begin();

    bool visible = iCheck;

    while ( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find( ( *constIterator ).toUInt() );

      if ( it != m_Container.get< TraceID >().end() )
        {
        //modify the structure visibility
        m_Container.get< TraceID >().
            modify( it , change_visible<MultiIndexContainerElementType>(visible) );
        //send signal to track container
        emit ShowLineage(it->TraceID, visible);
        }
      ++constIterator;
      }
    }
}
//-------------------------------------------------------------------------
