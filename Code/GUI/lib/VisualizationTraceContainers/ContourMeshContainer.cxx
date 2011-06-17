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

#include "ContourMeshContainer.h"
#include "StructureHelper.h"

#include "vtkActor.h"

//-------------------------------------------------------------------------
ContourMeshContainer::ContourMeshContainer(QObject *iParent,
                                           QGoImageView3D *iView) :
  Superclass(iParent, iView),
  m_TCoord( std::numeric_limits< unsigned int >::max() )
{
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshContainer::
~ContourMeshContainer()
{
  MultiIndexContainerType::iterator it = m_Container.begin();

  while ( it != m_Container.end() )
    {
    if ( it->Nodes )
      {
      it->Nodes->Delete();
      }
    if ( it->ActorXY )
      {
      it->ActorXY->Delete();
      }
    if ( it->ActorXZ )
      {
      it->ActorXZ->Delete();
      }
    if ( it->ActorYZ )
      {
      it->ActorYZ->Delete();
      }
    if ( it->ActorXYZ )
      {
      it->ActorXYZ->Delete();
      }

    ++it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::SetTimePoint(const unsigned int & iT)
{
  this->m_TCoord = iT;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::UpdateCurrentElementFromVisu(std::vector< vtkActor * >& iActors,
                                                   vtkPolyData *iNodes,
                                                   const unsigned int & iT,
                                                   const bool & iHighlighted,
                                                   const bool & iVisible)
{
  this->m_CurrentElement.TCoord = iT;
  this->UpdateCurrentElementFromVisuBase(iActors, iNodes,
                                         iHighlighted, iVisible);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::RemoveActorsWithGivenTimePoint(const unsigned int & iT)
{
  MultiIndexContainerTCoordIterator it0, it1;

  boost::tuples::tie(it0, it1) = m_Container.get< TCoord >().equal_range(iT);

  ChangeActorsVisibility< TCoord >(it0, it1, false);

  if ( m_ImageView )
    {
    m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::ShowActorsWithGivenTimePoint(const unsigned int & iT)
{
  if ( iT != m_TCoord )
    {
    MultiIndexContainerTCoordIterator it, it0, it1;

    boost::tuples::tie(it0, it1) = m_Container.get< TCoord >().equal_range(iT);

    ChangeActorsVisibility< TCoord >(m_Container.get< TCoord >().begin(), it0, false);
    ChangeActorsVisibility< TCoord >(it0, it1, true);
    ChangeActorsVisibility< TCoord >(it1, m_Container.get< TCoord >().end(), false);

    m_TCoord = iT;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::AddActorsWithGivenTimePoint(const unsigned int & iT)
{
  MultiIndexContainerTCoordIterator it0, it1;

  boost::tuples::tie(it0, it1) = m_Container.get< TCoord >().equal_range(iT);

  ChangeActorsVisibility< TCoord >(it0, it1, true);

  if ( m_ImageView )
    {
    m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::UpdateElementHighlightingWithGivenTraceIDs(const QStringList & iList,
                                                                 const Qt::CheckState & iCheck)
{
  Superclass::UpdateElementHighlightingWithGivenTraceIDsBase(iList,
                                                             iCheck);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ContourMeshContainer::UpdateElementVisibilityWithGivenTraceIDs(const QStringList & iList,
                                                               const Qt::CheckState & iCheck)
{
  Superclass::UpdateElementVisibilityWithGivenTraceIDsBase(iList, iCheck);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
ContourMeshContainer::DeleteElement(const unsigned int & iId)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  return DeleteElement(it);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
ContourMeshContainer::DeleteElement(MultiIndexContainerTraceIDIterator iIter)
{
  if ( iIter != m_Container.get< TraceID >().end() )
    {
    if ( iIter->ActorXY )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(0, iIter->ActorXY);
        }
      iIter->ActorXY->Delete();
      }
    if ( iIter->ActorXZ )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(1, iIter->ActorXZ);
        }
      iIter->ActorXZ->Delete();
      }
    if ( iIter->ActorYZ )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(2, iIter->ActorYZ);
        }
      iIter->ActorYZ->Delete();
      }
    if ( iIter->ActorXYZ )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(3, iIter->ActorXYZ);
        }
      iIter->ActorXYZ->Delete();
      }

    if ( iIter->Nodes )
      {
      iIter->Nodes->Delete();
      }

    m_Container.get< TraceID >().erase(iIter);

    if ( m_ImageView )
      {
      m_ImageView->UpdateRenderWindows();
      }
    return true;
    }
  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
ContourMeshContainer::DeleteAllHighlightedElements()
{
  MultiIndexContainerHighlightedIterator it0, it1, it_t;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  std::list< unsigned int > oList;

  while ( it0 != it1 )
    {

      std::cout << "TRACE ID TO BE DELETED: " << it0->TraceID << std::endl;

    oList.push_back(it0->TraceID);

    if ( it0->ActorXY )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(0, it0->ActorXY);
        }
      it0->ActorXY->Delete();
      }
    if ( it0->ActorXZ )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(1, it0->ActorXZ);
        }
      it0->ActorXZ->Delete();
      }
    if ( it0->ActorYZ )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(2, it0->ActorYZ);
        }
      it0->ActorYZ->Delete();
      }
    if ( it0->ActorXYZ )
      {
      if ( m_ImageView )
        {
        this->m_ImageView->RemoveActor(3, it0->ActorXYZ);
        }
      it0->ActorXYZ->Delete();
      }
    if ( it0->Nodes )
      {
      it0->Nodes->Delete();
      }

    it_t = it0;
    ++it0;

    m_Container.get< Highlighted >().erase(it_t);
    }

  if ( m_ImageView )
    {
    m_ImageView->UpdateRenderWindows();
    }

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
ContourMeshContainer::GetElementsTraceIDForGivenTimePoint(unsigned int iTimePoint)
{
  MultiIndexContainerTCoordIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_Container.get< TCoord >().equal_range(iTimePoint);

  std::list< unsigned int > oList;
  while ( it0 != it1 )
    {
    oList.push_back(it0->TraceID);
    ++it0;
    }
  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map< unsigned int, double * >
ContourMeshContainer::GetMeshesPoints(std::list< unsigned int > iMeshID)
{
  std::map< unsigned int, double * >  meshPosition;
  std::list< unsigned int >::iterator listIt = iMeshID.begin();
  MultiIndexContainerTraceIDIterator  containerIt;

  while ( listIt != iMeshID.end() )
    {
    containerIt = m_Container.get< TraceID >().find(*listIt);

    //if element found
    if ( containerIt != m_Container.get< TraceID >().end() )
      {
      unsigned int time = containerIt->TCoord;
      double *     point = new double[3];

      double bounds[6];
      containerIt->Nodes->GetBounds(bounds);

      for ( int i = 0; i < 3; ++i )
        {
        point[i] = ( bounds[2 * i] + bounds[2 * i + 1] ) / 2;
        }

      meshPosition.insert( std::pair< unsigned int, double * >(time, point) );
      }
    ++listIt;
    }

  return meshPosition;
}

//-------------------------------------------------------------------------
