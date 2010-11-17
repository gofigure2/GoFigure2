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

#include "TrackContainer.h"

#include "vtkActor.h"

// reconstruct the polydata
#include "vtkPointData.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"

// tubes and glyph
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"

// to convert coordinates
#include "vtkViewImage2D.h"

// FOR TESTING
#include "VisualizePolydataHelper.h"
#include "vtkPolyDataWriter.h"

//-------------------------------------------------------------------------
TrackContainer::
TrackContainer(QObject *iParent,QGoImageView3D *iView):QObject(iParent),
  m_ImageView(iView),
  m_CurrentElement(),
  m_HighlightedProperty(NULL)
{}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
TrackContainer::
~TrackContainer()
{
  MultiIndexContainer::iterator it = m_Container.begin();

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

    std::map< unsigned int,double*>::const_iterator begin = it->PointsMap.begin();
    std::map< unsigned int,double*>::const_iterator end = it->PointsMap.end();
    // if there is a point, delete it and return true
    if ( begin != end )
      {
      // free memory
      delete[] begin->second;

      ++begin;
      }

    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
Print()
{
  this->Print( m_Container.begin(), m_Container.end() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
InsertCurrentElement()
{
  m_Container.insert(m_CurrentElement);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCurrentElementFromVisu(std::vector< vtkActor * > iActors,
                                                        vtkPolyData *iNodes,
                                                        const bool & iHighlighted,
                                                        const bool & iVisible)
{
  if ( iActors.size() != 4 )
    {
    std::cerr << "iActors.size() != 4" << std::endl;
    return;
    }
  else
    {
    this->m_CurrentElement.ActorXY = iActors[0];
    this->m_CurrentElement.ActorXZ = iActors[1];
    this->m_CurrentElement.ActorYZ = iActors[2];
    this->m_CurrentElement.ActorXYZ = iActors[3];
    }
  this->m_CurrentElement.Nodes = iNodes;
  this->m_CurrentElement.Highlighted = iHighlighted;
  this->m_CurrentElement.Visible = iVisible;

  if ( iHighlighted )
    {
    this->m_CurrentElement.ActorXY->SetProperty(this->m_HighlightedProperty);
    this->m_CurrentElement.ActorXZ->SetProperty(this->m_HighlightedProperty);
    this->m_CurrentElement.ActorYZ->SetProperty(this->m_HighlightedProperty);
    this->m_CurrentElement.ActorXYZ->SetProperty(this->m_HighlightedProperty);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCurrentElementFromDB(unsigned int iTraceID,
                           double irgba[4])
{
  this->m_CurrentElement.TraceID = iTraceID;
  this->m_CurrentElement.rgba[0] = irgba[0];
  this->m_CurrentElement.rgba[1] = irgba[1];
  this->m_CurrentElement.rgba[2] = irgba[2];
  this->m_CurrentElement.rgba[3] = irgba[3];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool TrackContainer::
UpdateCurrentElementFromExistingOne(unsigned int iTraceID)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iTraceID);
  if ( it != m_Container.get< TraceID >().end() )
    {
    // update current element
    this->m_CurrentElement = *it;

    // clean the container but don't erase the pointers since we still have the
    // adresses in the m_CurrentElement
    m_Container.get< TraceID >().erase(it);

    return true;
    }
  else
    {
    return false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
Insert(const TrackStructure & iE)
{
  m_Container.insert(iE);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
ResetCurrentElement()
{
  m_CurrentElement = TrackStructure();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
RemoveActorsWithGivenTimePoint(const unsigned int & iT)
{
/// TODO FILL IT
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
ShowActorsWithGivenTimePoint(const unsigned int & iT)
{
  /*
  MultiIndexContainerTCoordIterator it0, it1;

  boost::tuples::tie(it0, it1) = m_Container.get< TCoord >().equal_range(iT);

  ChangeActorsVisibility< TCoord >(it0, it1, false);
*/
  m_ImageView->UpdateRenderWindows();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
AddActorsWithGivenTimePoint(const unsigned int & iT)
{
  /// TODO FILL IT
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::RemoveElementFromVisualizationWithGivenTraceID(
  const unsigned int & iId)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  if ( it != m_Container.get< TraceID >().end() )
    {
    if ( it->ActorXY )
      {
      this->m_ImageView->RemoveActor(0, it->ActorXY);
      }
    if ( it->ActorXZ )
      {
      this->m_ImageView->RemoveActor(1, it->ActorXZ);
      }
    if ( it->ActorYZ )
      {
      this->m_ImageView->RemoveActor(2, it->ActorYZ);
      }
    if ( it->ActorXYZ )
      {
      this->m_ImageView->RemoveActor(3, it->ActorXYZ);
      }

    this->m_CurrentElement = *it;

    m_Container.get< TraceID >().erase(it);

    m_ImageView->UpdateRenderWindows();

    return true;
    }
  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::
UpdateElementHighlightingWithGivenTraceID(const unsigned int & iId)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  vtkProperty *temp_property = NULL;

  if ( it != m_Container.get< TraceID >().end() )
    {
    if ( it->Highlighted )
      {
      temp_property = vtkProperty::New();
      temp_property->SetColor(it->rgba[0],
                              it->rgba[1],
                              it->rgba[2]);
      temp_property->SetOpacity(it->rgba[3]);
      }
    else
      {
      temp_property = this->m_HighlightedProperty;
      }

    if ( it->ActorXY )
      {
      it->ActorXY->SetProperty(temp_property);
      }
    if ( it->ActorXZ )
      {
      it->ActorXZ->SetProperty(temp_property);
      }
    if ( it->ActorYZ )
      {
      it->ActorYZ->SetProperty(temp_property);
      }
    if ( it->ActorXYZ )
      {
      it->ActorXYZ->SetProperty(temp_property);
      }

    if ( it->Highlighted )
      {
      temp_property->Delete();
      }

    TrackStructure tempStructure(*it);
    tempStructure.Highlighted = !it->Highlighted;

    m_Container.get< TraceID >().replace(it, tempStructure);
    m_ImageView->UpdateRenderWindows();

    return true;
    }

  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::
UpdateElementVisibilityWithGivenTraceID(const unsigned int & iId)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
  ImageViewMember f;

  if ( it != m_Container.get< TraceID >().end() )
    {

    if ( it->Visible )
      {
      f = &QGoImageView3D::RemoveActor;
      }
    else
      {
      f = &QGoImageView3D::AddActor;
      }

    if ( it->ActorXY )
      {
      ( m_ImageView->*f )(0, it->ActorXY);
      }
    if ( it->ActorXZ )
      {
      ( m_ImageView->*f )(1, it->ActorXZ);
      }
    if ( it->ActorYZ )
      {
      ( m_ImageView->*f )(2, it->ActorYZ);
      }
    if ( it->ActorXYZ )
      {
      ( m_ImageView->*f )(3, it->ActorXYZ);
      }

    it->SetActorVisibility( !it->Visible );

    TrackStructure tempStructure(*it);
    tempStructure.Visible = !it->Visible;

    m_Container.get< TraceID >().replace(it, tempStructure);

    m_ImageView->UpdateRenderWindows();
    return true;
    }

  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::
DeleteElement(const unsigned int & iId)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);
  return DeleteElement(it);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool TrackContainer::DeleteElement(MultiIndexContainerTraceIDIterator iIter)
{
  if ( iIter != m_Container.get< TraceID >().end() )
    {
    if ( iIter->ActorXY )
      {
      this->m_ImageView->RemoveActor(0, iIter->ActorXY);
      iIter->ActorXY->Delete();
      }
    if ( iIter->ActorXZ )
      {
      this->m_ImageView->RemoveActor(1, iIter->ActorXZ);
      iIter->ActorXZ->Delete();
      }
    if ( iIter->ActorYZ )
      {
      this->m_ImageView->RemoveActor(2, iIter->ActorYZ);
      iIter->ActorYZ->Delete();
      }
    if ( iIter->ActorXYZ )
      {
      this->m_ImageView->RemoveActor(3, iIter->ActorXYZ);
      iIter->ActorXYZ->Delete();
      }

    if ( iIter->Nodes )
      {
      iIter->Nodes->Delete();
      }

    if ( iIter->PointsMap.size() )
      {
      std::map< unsigned int, double* >::const_iterator begin = (iIter->PointsMap).begin();
      std::map< unsigned int, double* >::const_iterator end = (iIter->PointsMap).end();

      while( begin != end )
        {
        delete[] begin->second;
        ++begin;
        }
      }

    m_Container.get< TraceID >().erase(iIter);
    m_ImageView->UpdateRenderWindows();
    return true;
    }
  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
TrackContainer::
DeleteAllHighlightedElements()
{
  MultiIndexContainerHighlightedIterator it0, it1, it_t;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  std::list< unsigned int > oList;

  while ( it0 != it1 )
    {
    oList.push_back(it0->TraceID);

    if ( it0->ActorXY )
      {
      this->m_ImageView->RemoveActor(0, it0->ActorXY);
      it0->ActorXY->Delete();
      }
    if ( it0->ActorXZ )
      {
      this->m_ImageView->RemoveActor(1, it0->ActorXZ);
      it0->ActorXZ->Delete();
      }
    if ( it0->ActorYZ )
      {
      this->m_ImageView->RemoveActor(2, it0->ActorYZ);
      it0->ActorYZ->Delete();
      }
    if ( it0->ActorXYZ )
      {
      this->m_ImageView->RemoveActor(3, it0->ActorXYZ);
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

  m_ImageView->UpdateRenderWindows();

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
TrackContainer::
GetHighlightedElementsTraceID()
{
  MultiIndexContainerHighlightedIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  std::list< unsigned int > oList;
  while ( it0 != it1 )
    {
    oList.push_back(it0->TraceID);
    std::cout<< "highlighted track ID: " << it0->TraceID << std::endl;
    ++it0;
    }
  return oList;
}

//-------------------------------------------------------------------------
std::list< unsigned int >
TrackContainer::
UpdateAllHighlightedElementsWithGivenColor(QColor iColor)
{
  MultiIndexContainerHighlightedIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  double r(1.), g(1.), b(1.), a(1.);
  if ( iColor.isValid() )
    {
    iColor.getRgbF(&r, &g, &b, &a);
    }

  std::list< unsigned int > oList;
  while ( it0 != it1 )
    {
    TrackStructure temp(*it0);
    temp.rgba[0] = r;
    temp.rgba[1] = g;
    temp.rgba[2] = b;
    temp.rgba[3] = a;

    m_Container.get< Highlighted >().replace(it0, temp);

    oList.push_back(it0->TraceID);

    ++it0;
    }

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetHighlightedProperty(vtkProperty *iProperty)
{
  this->m_HighlightedProperty = iProperty;

  MultiIndexContainerHighlightedIterator it0, it1;
  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  while ( it0 != it1 )
    {
    if ( it0->Highlighted )
      {
      it0->SetActorProperties( this->m_HighlightedProperty );
      }
    ++it0;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkProperty *
TrackContainer::
GetHighlightedProperty()
{
  return m_HighlightedProperty;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*bool
TrackContainer::
AddPointToCurrentElement(int iTime, double* iPoint, bool iReconstructPolyData)
{
  //add the point in the map
  bool pointInserted = this->m_CurrentElement.InsertElement( iTime, iPoint );

  if(pointInserted)
    {
    // check time point too
    if(!this->m_CurrentElement.Nodes)
      {
      // Create a new polydata
      // no actors to be removed then
      this->m_CurrentElement.Nodes = vtkPolyData::New();

      vtkSmartPointer< vtkPoints > newPoints = vtkSmartPointer< vtkPoints >::New();
      vtkSmartPointer<vtkIntArray> newArray = vtkSmartPointer<vtkIntArray>::New();
      newArray->SetNumberOfComponents(1);
      newArray->SetName("TemporalInformation");

      newArray->InsertValue( 0, iTime );
      newPoints->InsertPoint( 0, iPoint );

      //add the points to the dataset
      this->m_CurrentElement.Nodes->SetPoints(newPoints);
      //add the temporal information
      this->m_CurrentElement.Nodes->GetPointData()->AddArray(newArray);

      CreateCurrentTrackActors();

      emit CurrentTrackToSave();

      // save in DB and insert current element
      return pointInserted;
      }

    if(iReconstructPolyData)
      {
      UpdateTrackStructurePolyData( this->m_CurrentElement );
      }

    emit CurrentTrackToSave();
    }
  return pointInserted;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::
DeletePointFromCurrentElement(int iTime, bool iReconstructPolyData)
{
  //add the point in the map
  bool pointDeleted = this->m_CurrentElement.DeleteElement( iTime );

  // build the new polydata if a point has been deleted
  if(pointDeleted && iReconstructPolyData)
    {
    UpdateTrackStructurePolyData( this->m_CurrentElement );
    }

  return pointDeleted;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*bool
TrackContainer::
ReplacePointFromCurrentElement(int iTime, double* iPoint)
{
  // replace the existing element
  bool pointReplaced = DeleteElement(iTime);

  // if sth has been deleted, insert the point and return true
  if(pointReplaced)
    {
    return AddPointToCurrentElement(iTime,iPoint);
    }

  return pointReplaced;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkPolyData*
TrackContainer::
GetCurrentElementNodes()
{
  return this->m_CurrentElement.Nodes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::
UpdateTrackStructurePolyData( const TrackStructure& iTrackStructure)
{
  if( iTrackStructure.PointsMap.empty() )
    {
    std::cout << "No points in the map, erase polydata and actors" << std::endl;

    // delete polydata and actors
    if ( iTrackStructure.Nodes )
      {
      if( iTrackStructure.Nodes->GetPointData() )
        {
        iTrackStructure.Nodes->GetPointData()->Reset();
        }
      if( iTrackStructure.Nodes->GetPoints() )
        {
        iTrackStructure.Nodes->GetPoints()->Reset();
        }
      if( iTrackStructure.Nodes->GetLines() )
        {
        iTrackStructure.Nodes->GetLines()->Reset();
        }
      if( iTrackStructure.Nodes )
        {
        iTrackStructure.Nodes->Reset();
        }
      }

    return false;
    }

  // read map and fill points
  vtkSmartPointer< vtkPoints > newPoints = vtkSmartPointer< vtkPoints >::New();
  vtkSmartPointer<vtkIntArray> newArray = vtkSmartPointer<vtkIntArray>::New();
  newArray->SetNumberOfComponents(1);
  newArray->SetName("TemporalInformation");
  std::map< unsigned int, double*>::const_iterator it
      = iTrackStructure.PointsMap.begin();

  while( it != iTrackStructure.PointsMap.end() )
    {
    newArray->InsertNextValue( it->first );
    newPoints->InsertNextPoint(it->second);
    ++it;
    }

  // Create a line from points
  vtkSmartPointer<vtkPolyLine> polyLine =
      vtkSmartPointer<vtkPolyLine>::New();
  polyLine->GetPointIds()->SetNumberOfIds( newPoints->GetNumberOfPoints() );
  for(unsigned int i = 0; i < newPoints->GetNumberOfPoints(); i++)
    {
    polyLine->GetPointIds()->SetId(i,i);
    }

  //Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> cells =
      vtkSmartPointer<vtkCellArray>::New();
  cells->InsertNextCell(polyLine);

  //Create a polydata to store everything in
  vtkSmartPointer< vtkPolyData > polyData = vtkSmartPointer< vtkPolyData >::New();

  //add the points to the dataset
  polyData->SetPoints(newPoints);
  //add the lines to the dataset
  polyData->SetLines(cells);
  //add the temporal information
  polyData->GetPointData()->AddArray(newArray);

  iTrackStructure.Nodes->DeepCopy(polyData);

  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCurrentElementActorsFromVisu(std::vector< vtkActor * > iActors)
{
  this->m_CurrentElement.ActorXY = iActors[0];
  this->m_CurrentElement.ActorXZ = iActors[1];
  this->m_CurrentElement.ActorYZ = iActors[2];
  this->m_CurrentElement.ActorXYZ = iActors[3];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                            const Qt::CheckState& iCheck )
{
  //need to be reimplemented
  /*if( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;

    vtkProperty *temp_property = NULL;

    QStringList::const_iterator constIterator = iList.begin();

    while( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find((*constIterator).toUInt());

      if ( it != m_Container.get< TraceID >().end() )
        {
        if ( !iCheck )
          {
          temp_property = vtkProperty::New();
          temp_property->SetColor(it->rgba[0],
                                  it->rgba[1],
                                  it->rgba[2]);
          temp_property->SetOpacity(it->rgba[3]);
          }
        else
          {
          temp_property = this->m_HighlightedProperty;
          }

        if ( it->ActorXY )
          {
          it->ActorXY->SetProperty(temp_property);
          }
        if ( it->ActorXZ )
          {
          it->ActorXZ->SetProperty(temp_property);
          }
        if ( it->ActorYZ )
          {
          it->ActorYZ->SetProperty(temp_property);
          }
        if ( it->ActorXYZ )
          {
          it->ActorXYZ->SetProperty(temp_property);
          }

        if ( !iCheck )
          {
          temp_property->Delete();
          }

        TrackStructure tempStructure(*it);
        tempStructure.Highlighted = iCheck;

        m_Container.get< TraceID >().replace(it, tempStructure);
        }

      ++constIterator;
      }

    m_ImageView->UpdateRenderWindows();
    }*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                          const Qt::CheckState& iCheck )
{
  /*
  if( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;

    typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
    ImageViewMember f;

    QStringList::const_iterator constIterator = iList.begin();

    while( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find((*constIterator).toUInt());

      if ( it != m_Container.get< TraceID >().end() )
        {
        if ( iCheck )
          {
          f = &QGoImageView3D::RemoveActor;
          }
        else
          {
          f = &QGoImageView3D::AddActor;
          }

        if ( it->ActorXY )
          {
          ( m_ImageView->*f )(0, it->ActorXY);
          }
        if ( it->ActorXZ )
          {
          ( m_ImageView->*f )(1, it->ActorXZ);
          }
        if ( it->ActorYZ )
          {
          ( m_ImageView->*f )(2, it->ActorYZ);
          }
        if ( it->ActorXYZ )
          {
          ( m_ImageView->*f )(3, it->ActorXYZ);
          }


        if ( it->ActorXY )
          {
          it->ActorXY->SetVisibility(iCheck);
          }
        if ( it->ActorXZ )
          {
          it->ActorXZ->SetVisibility(iCheck);
          }
        if ( it->ActorYZ )
          {
          it->ActorYZ->SetVisibility(iCheck);
          }
        if ( it->ActorXYZ )
          {
          it->ActorXYZ->SetVisibility(iCheck);
          }

        TrackStructure tempStructure(*it);
        tempStructure.Visible = iCheck;

        m_Container.get< TraceID >().replace(it, tempStructure);
        }

      ++constIterator;
      }
    m_ImageView->UpdateRenderWindows();
    }*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateTracksStrings( std::vector<int> iTrackList )
{
  std::vector<int>::iterator it = iTrackList.begin();

  while( it != iTrackList.end() )
    {
    // update the current element
    UpdateCurrentElementFromExistingOne( *it );

    // emit signal to get the meshes informations
    emit NeedMeshesInfoForImportedTrack( (*it) );

    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCurrentElementMap( std::map< unsigned int, double* > iMeshes)
{
  // add points to existing map, not erasing
  std::map< unsigned int,double*>::iterator beginMesh = iMeshes.begin();
  std::map< unsigned int,double*>::iterator endMesh = iMeshes.end();

  while( beginMesh != endMesh)
    {
    bool addPoint = this->m_CurrentElement.InsertElement( beginMesh->first, beginMesh->second );

    if(addPoint)
      {
      // Point has been added
      }
    else
      {
      // there is already sth at this time point, delete the point (should replace??)
      delete[] beginMesh->second;
      }
    ++beginMesh;
    }

  // Create a new polydata and new actors if it is a new track
  if(!this->m_CurrentElement.Nodes)
    {
    //Create new polydata (new address)
    this->m_CurrentElement.Nodes = vtkPolyData::New();

    UpdateTrackStructurePolyData(this->m_CurrentElement);

    CreateCurrentTrackActors();

    emit CurrentTrackToSave();

    return;
    }

  UpdateTrackStructurePolyData(this->m_CurrentElement);

  emit CurrentTrackToSave();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
CreateCurrentTrackActors()
{
  //Create new actors (new address)
  vtkProperty * trace_property = vtkProperty::New();
  double r = this->m_CurrentElement.rgba[0];
  double g = this->m_CurrentElement.rgba[1];
  double b = this->m_CurrentElement.rgba[2];
  double a = this->m_CurrentElement.rgba[3];

  trace_property->SetColor( r,
                            g,
                            b);
  trace_property->SetOpacity( a );

  // Add contour
  std::vector< vtkActor * > trackActors =
      m_ImageView->AddContour( this->m_CurrentElement.Nodes, trace_property );

  //update container actors addresses
  UpdateCurrentElementActorsFromVisu(trackActors);

  trace_property->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
DeleteListFromCurrentElement( std::list<int> iTimeList )
{
  std::list<int>::iterator begin = iTimeList.begin();
  std::list<int>::iterator end = iTimeList.end();

  while( begin != end )
    {
    bool succeed = DeletePointFromCurrentElement( *begin,
                                                   false ); // update the polydata

    if(!succeed)
      {
      std::cout << "Time point: " << *begin << " can't be deleted" << std::endl;
      }

    ++begin;
    }

  // Reconstruct the polydata
  UpdateTrackStructurePolyData( this->m_CurrentElement );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateTracksReprensentation( bool iGlyph, bool iTube )
{
  MultiIndexContainer::iterator it = m_Container.begin();

  if( iGlyph && iTube )
    {
    while ( it != m_Container.end() )
      {

      bool update = UpdateTrackStructurePolyData( (*it) );

      if( update )
        {
        // Glyph shape
        vtkSphereSource* sphere = vtkSphereSource::New();
        sphere->SetThetaResolution( 8 );
        sphere->SetPhiResolution( 8 );

        vtkGlyph3D* glyph = vtkGlyph3D::New();
        glyph->SetInput( it->Nodes );
        glyph->SetSource( sphere->GetOutput() );
        glyph->Update();

        sphere->Delete();

        vtkTubeFilter* tube = vtkTubeFilter::New();
        tube->SetNumberOfSides( 8 );
        tube->SetInput( it->Nodes );
        tube->SetRadius( .2  );
        tube->Update();

        // append both polydata sets
        vtkAppendPolyData* apd = vtkAppendPolyData::New();;
        apd->AddInput( glyph->GetOutput() );
        apd->AddInput( tube->GetOutput() );
        apd->Update();

        it->Nodes->DeepCopy( apd->GetOutput() );

        glyph->Delete();
        tube->Delete();
        apd->Delete();
        }
      ++it;
    }
  }

  if( iGlyph && !iTube )
    {
    while ( it != m_Container.end() )
      {
      bool update = UpdateTrackStructurePolyData( (*it) );

      if( update )
        {
        // Glyph shape
        vtkSphereSource* sphere = vtkSphereSource::New();
        sphere->SetThetaResolution( 8 );
        sphere->SetPhiResolution( 8 );

        vtkGlyph3D* glyph = vtkGlyph3D::New();
        glyph->SetInput( it->Nodes );
        glyph->SetSource( sphere->GetOutput() );
        glyph->Update();

        sphere->Delete();

        // append both polydata sets
        vtkAppendPolyData* apd = vtkAppendPolyData::New();;
        apd->AddInput( glyph->GetOutput() );
        apd->AddInput( it->Nodes );
        apd->Update();

        it->Nodes->DeepCopy( apd->GetOutput() );

        apd->Delete();
        glyph->Delete();
        }
      ++it;
    }
  }

  if( !iGlyph && iTube )
    {
    while ( it != m_Container.end() )
      {
      bool update = UpdateTrackStructurePolyData( (*it) );

      if( update )
        {
        vtkTubeFilter* tube = vtkTubeFilter::New();
        tube->SetNumberOfSides( 8 );
        tube->SetInput( it->Nodes );
        tube->SetRadius( .2  );
        tube->Update();

        it->Nodes->DeepCopy( tube->GetOutput() );

        tube->Delete();
        }
      ++it;
    }
  }

  if( !iGlyph && !iTube )
    {
    while ( it != m_Container.end() )
    {
    UpdateTrackStructurePolyData( (*it) );

    ++it;
    }
  }

  m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
DeleteListOfTracks(
    std::list< std::pair< unsigned int , std::list< unsigned int > > >
               iPointsToBeDeleted)
{
  if(iPointsToBeDeleted.empty())
    {
    std::cout << "list of tracks to be deleted is empty" << std::endl;
    }

  // iterate through tracks IDs
  std::list< std::pair< unsigned int , std::list< unsigned int > > >::iterator
      trackIDIterator = iPointsToBeDeleted.begin();
  while( trackIDIterator != iPointsToBeDeleted.end() )
    {
    // Get associated structure
    MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find( trackIDIterator->first );

    // if we find the stucture, update it!
    if ( it != m_Container.get< TraceID >().end() )
      {
      std::list<unsigned int>::iterator begin = trackIDIterator->second.begin();
      std::list<unsigned int>::iterator end = trackIDIterator->second.end();

      while( begin != end )
        {
        bool succeed = DeletePointFromElement( it, *begin, false );

        if( !succeed )
          {
          std::cout << "In track: " << trackIDIterator->first << std::endl;
          std::cout << "Time point: " << *begin << " can't be deleted" << std::endl;
          }

        ++begin;
        }

      // Reconstruct the polydata
      UpdateTrackStructurePolyData( *it );
      }

    ++trackIDIterator;
    }
}


//-------------------------------------------------------------------------
bool
TrackContainer::
DeletePointFromElement( MultiIndexContainerTraceIDIterator iTrackStructureIterator,
                        int iTime, bool iReconstructPolyData )
{
  // Create temp structure
  TrackStructure tempStructure(*iTrackStructureIterator);

  //add the point in the map
  bool pointDeleted = tempStructure.DeleteElement( iTime );

  // build the new polydata if a point has been deleted
  if(pointDeleted && iReconstructPolyData)
    {
    UpdateTrackStructurePolyData( tempStructure );
    }

  // Replace
  m_Container.get< TraceID >().replace(iTrackStructureIterator, tempStructure);

  return pointDeleted;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdatePointsFromBBForGivenTrack( unsigned int iTrackID,
                                 std::list<std::vector<unsigned int> > iBoundingBox)
{
  if(iBoundingBox.empty())
      {
      std::cout << "list of points to be added is empty" << std::endl;
      }

  MultiIndexContainerTraceIDIterator
  it = m_Container.get< TraceID >().find( iTrackID );

  // if we find the stucture, update it!
  if ( it != m_Container.get< TraceID >().end() )
    {
    std::list< std::vector<unsigned int> >::iterator begin = iBoundingBox.begin();
    std::list< std::vector<unsigned int> >::iterator end = iBoundingBox.end();

    //add the point in the map
    TrackStructure tempStructure(*it);

    while( begin != end )
      {
      int xyzBB[3] = {(*begin)[0], (*begin)[1], (*begin)[2]};

      unsigned int time = (*begin)[3];

      // convert xyz coordinates
      double* xyz = m_ImageView->GetImageViewer(0)
          ->GetWorldCoordinatesFromImageCoordinates(xyzBB);

      bool added = tempStructure.InsertElement( time, xyz );
      if( !added )
        {
        std::cout << "Element at a time point: " << time
                  << "could not be added." << std::endl;
        }
      ++begin;
      }
    m_Container.get< TraceID >().replace(it, tempStructure);

    // Reconstruct the polydata
    UpdateTrackStructurePolyData( *it );
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
RecomputeCurrentElementMap( std::list< double* > iPoints)
{
  // empty current element map
  std::map< unsigned int,double*>::const_iterator begin
      = this->m_CurrentElement.PointsMap.begin();
  std::map< unsigned int,double*>::const_iterator end
      = this->m_CurrentElement.PointsMap.end();

  if ( begin != end )
    {
    // free memory
    delete[] begin->second;
    ++begin;
    }

  this->m_CurrentElement.PointsMap.clear();

  if( !(this->m_CurrentElement.PointsMap.empty()) )
    {
    std::cout << "Current element map is not empty whereas it should be"
        << std::endl;
    return;
    }

  // add points to the map
  std::list< double* >::iterator beginList = iPoints.begin();
  std::list< double* >::iterator endList = iPoints.end();

  while( beginList != endList)
    {
    int xyzBB[3] = {(*beginList)[0], (*beginList)[1], (*beginList)[2]};
    unsigned int time = (*beginList)[3];
    // convert xyz coordinates
    double* xyz = m_ImageView->GetImageViewer(0)
        ->GetWorldCoordinatesFromImageCoordinates(xyzBB);

    bool addPoint = this->m_CurrentElement.InsertElement( time, xyz );

    if(!addPoint)
      {
      std::cout << "problem while inserting element in the map" << std::endl;
      std::cout << " x: " << xyz[0] << " y: " << xyz[1] << " z: " << xyz[2]
                << " t: " << xyz[3] << std::endl;
      }

    ++beginList;
    }

  // Create a new polydata and new actors if it is a new track
  if(!this->m_CurrentElement.Nodes)
    {
    //Create new polydata (new address)
    this->m_CurrentElement.Nodes = vtkPolyData::New();

    UpdateTrackStructurePolyData(this->m_CurrentElement);

    CreateCurrentTrackActors();

    //emit CurrentTrackToSave();

    return;
    }

  UpdateTrackStructurePolyData(this->m_CurrentElement);

  //emit CurrentTrackToSave();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdatePointsForATrack(unsigned int iTrackID,
std::list< double*> iListCenterBoundingBoxes)
{
  this->UpdateCurrentElementFromExistingOne(iTrackID);
  this->RecomputeCurrentElementMap(iListCenterBoundingBoxes);
}
