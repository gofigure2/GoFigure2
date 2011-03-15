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

// division polydata
#include "vtkLine.h"

// to convert coordinates
#include "vtkViewImage2D.h"

// FOR TESTING
#include "VisualizePolydataHelper.h"
#include "vtkPolyDataWriter.h"

#include "vtkLookupTable.h"
#include <limits>

#include <QDebug>

//-------------------------------------------------------------------------
TrackContainer::TrackContainer(QObject *iParent, QGoImageView3D *iView) : Superclass(iParent, iView)
{
  m_TimeInterval = 0;
  m_ActiveScalars.append("Original");
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
TrackContainer::
~TrackContainer()
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
void
TrackContainer::UpdateCurrentElementFromVisu(std::vector< vtkActor * > iActors,
                                             vtkPolyData *iNodes,
                                             const bool & iHighlighted,
                                             const bool & iVisible)
{
  this->UpdateCurrentElementFromVisuBase(iActors, iNodes,
                                         iHighlighted, iVisible);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::DeleteElement(const unsigned int & iId)
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
    if ( this->m_ImageView )
      {
      if ( iIter->ActorXY )
        {
        this->m_ImageView->RemoveActor(0, iIter->ActorXY);
        }
      if ( iIter->ActorXZ )
        {
        this->m_ImageView->RemoveActor(1, iIter->ActorXZ);
        }
      if ( iIter->ActorYZ )
        {
        this->m_ImageView->RemoveActor(2, iIter->ActorYZ);
        }
      if ( iIter->ActorXYZ )
        {
        this->m_ImageView->RemoveActor(3, iIter->ActorXYZ);
        }
      }
    iIter->ReleaseData();

    m_Container.get< TraceID >().erase(iIter);

    if ( this->m_ImageView )
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
TrackContainer::DeleteAllHighlightedElements()
{
  MultiIndexContainerHighlightedIterator it0, it1, it_t;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  std::list< unsigned int > oList;

  while ( it0 != it1 )
    {
    oList.push_back(it0->TraceID);

    if ( this->m_ImageView )
      {
      if ( it0->ActorXY )
        {
        this->m_ImageView->RemoveActor(0, it0->ActorXY);
        }
      if ( it0->ActorXZ )
        {
        this->m_ImageView->RemoveActor(1, it0->ActorXZ);
        }
      if ( it0->ActorYZ )
        {
        this->m_ImageView->RemoveActor(2, it0->ActorYZ);
        }
      if ( it0->ActorXYZ )
        {
        this->m_ImageView->RemoveActor(3, it0->ActorXYZ);
        }
      }

    it0->ReleaseData();

    it_t = it0;
    ++it0;

    m_Container.get< Highlighted >().erase(it_t);
    }

  if ( this->m_ImageView )
    {
    m_ImageView->UpdateRenderWindows();
    }

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::DeletePointFromCurrentElement(unsigned int iTime, bool iReconstructPolyData)
{
  //add the point in the map
  bool pointDeleted = this->m_CurrentElement.DeleteElement(iTime);

  // build the new polydata if a point has been deleted
  if ( pointDeleted && iReconstructPolyData )
    {
    UpdateTrackStructurePolyData(this->m_CurrentElement);
    }

  return pointDeleted;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::UpdateTrackStructurePolyData(const TrackStructure & iTrackStructure)
{
  if ( iTrackStructure.PointsMap.empty() )
    {
    qDebug() << "No points in the map, reset nodes";

    iTrackStructure.ResetNodes();

    return false;
    }

  // read map and fill points
  vtkSmartPointer< vtkPoints >   newPoints = vtkSmartPointer< vtkPoints >::New();
  vtkSmartPointer< vtkIntArray > newArray = vtkSmartPointer< vtkIntArray >::New();
  newArray->SetNumberOfComponents(1);
  newArray->SetName("TemporalInformation");

  // Create a line from points
  vtkSmartPointer< vtkPolyLine > polyLine = vtkSmartPointer< vtkPolyLine >::New();
  polyLine->GetPointIds()->SetNumberOfIds( iTrackStructure.PointsMap.size() );

  std::map< unsigned int, double * >::const_iterator it =
    iTrackStructure.PointsMap.begin();

  vtkIdType i = 0;

  while ( it != iTrackStructure.PointsMap.end() )
    {
    newArray->InsertNextValue(it->first);
    newPoints->InsertNextPoint(it->second);

    polyLine->GetPointIds()->SetId(i, i);

    ++i;
    ++it;
    }

  //Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer< vtkCellArray > cells =
    vtkSmartPointer< vtkCellArray >::New();
  cells->InsertNextCell(polyLine);

  //Create a polydata to store everything in
  vtkSmartPointer< vtkPolyData > polyData = vtkSmartPointer< vtkPolyData >::New();

  //add the points to the dataset
  polyData->SetPoints(newPoints);
  //add the lines to the dataset
  polyData->SetLines(cells);
  //add the temporal information
  polyData->GetPointData()->AddArray(newArray);

  vtkSmartPointer< vtkDoubleArray > speedArray =
    vtkSmartPointer< vtkDoubleArray >::New();
  speedArray->SetNumberOfComponents(1);
  speedArray->SetName("SpeedInformation");
  polyData->GetPointData()->AddArray(speedArray);

  iTrackStructure.Nodes->DeepCopy(polyData);
  //update speed information
  iTrackStructure.ComputeAttributes();

  iTrackStructure.Nodes->GetPointData()->SetActiveScalars(NULL);

  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdateCurrentElementActorsFromVisu(std::vector< vtkActor * > iActors)
{
  this->m_CurrentElement.ActorXY = iActors[0];
  this->m_CurrentElement.ActorXZ = iActors[1];
  this->m_CurrentElement.ActorYZ = iActors[2];
  this->m_CurrentElement.ActorXYZ = iActors[3];
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdateCurrentElementMap(std::map< unsigned int, double * > iMeshes)
{
  // add points to existing map, not erasing
  std::map< unsigned int, double * >::iterator beginMesh = iMeshes.begin();
  std::map< unsigned int, double * >::iterator endMesh = iMeshes.end();

  while ( beginMesh != endMesh )
    {
    bool addPoint =
      this->m_CurrentElement.InsertElement(beginMesh->first,
                                           beginMesh->second);

    if ( addPoint )
      {
      // Point has been added
      }
    else
      {
      // there is already sth at this time point, delete the point (should
      // replace??)
      delete[] beginMesh->second;
      }
    ++beginMesh;
    }

  // Create a new polydata and new actors if it is a new track
  if ( !this->m_CurrentElement.Nodes )
    {
    //Create new polydata (new address)
    this->m_CurrentElement.Nodes = vtkPolyData::New();

    UpdateTrackStructurePolyData(this->m_CurrentElement);

    CreateCurrentTrackActors();

    return;
    }

  UpdateTrackStructurePolyData(this->m_CurrentElement);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::CreateCurrentTrackActors()
{
  if ( this->m_ImageView )
    {
    //Create new actors (new address)
    vtkProperty *trace_property = vtkProperty::New();
    double       r = this->m_CurrentElement.rgba[0];
    double       g = this->m_CurrentElement.rgba[1];
    double       b = this->m_CurrentElement.rgba[2];
    double       a = this->m_CurrentElement.rgba[3];

    trace_property->SetColor(r,
                             g,
                             b);
    trace_property->SetOpacity(a);

    // Add contour
    std::vector< vtkActor * > trackActors =
      m_ImageView->AddContour(this->m_CurrentElement.Nodes, trace_property);

    //update container actors addresses
    UpdateCurrentElementActorsFromVisu(trackActors);

    trace_property->Delete();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::DeleteListFromCurrentElement(const std::list< unsigned int > & iTimeList)
{
  std::list< unsigned int >::const_iterator begin = iTimeList.begin();
  std::list< unsigned int >::const_iterator end = iTimeList.end();

  bool succeed = true;

  while ( begin != end )
    {
    succeed = DeletePointFromCurrentElement(*begin,
                                            false);   // update the polydata

    if ( !succeed )
      {
      qDebug() << "Time point: " << *begin << " can't be deleted";
      }

    ++begin;
    }

  // Reconstruct the polydata
  UpdateTrackStructurePolyData(this->m_CurrentElement);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::DeletePointFromElement(MultiIndexContainerTraceIDIterator iIterator,
                                       unsigned int iTime,
                                       bool iReconstructPolyData)
{
  // Create temp structure
  TrackStructure tempStructure(*iIterator);

  //add the point in the map
  bool pointDeleted = tempStructure.DeleteElement(iTime);

  // build the new polydata if a point has been deleted
  if ( pointDeleted && iReconstructPolyData )
    {
    UpdateTrackStructurePolyData(tempStructure);
    }

  // Replace
  m_Container.get< TraceID >().replace(iIterator, tempStructure);

  return pointDeleted;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdatePointsFromBBForGivenTrack(unsigned int iTrackID,
                                                std::list< std::vector< unsigned int > > iBoundingBox)
{
  if ( this->m_ImageView )
    {
    if ( iBoundingBox.empty() )
      {
      qDebug() << "list of points to be added is empty";
      }

    MultiIndexContainerTraceIDIterator
      it = m_Container.get< TraceID >().find(iTrackID);

    // if we find the stucture, update it!
    if ( it != m_Container.get< TraceID >().end() )
      {
      std::list< std::vector< unsigned int > >::iterator begin = iBoundingBox.begin();
      std::list< std::vector< unsigned int > >::iterator end = iBoundingBox.end();

      //add the point in the map
      TrackStructure tempStructure(*it);

      while ( begin != end )
        {
        int xyzBB[3] = { static_cast< int >( ( *begin )[0] ),
                         static_cast< int >( ( *begin )[1] ),
                         static_cast< int >( ( *begin )[2] ) };

        unsigned int time = ( *begin )[3];

        // convert xyz coordinates
        double *xyz = m_ImageView->GetImageViewer(0)
          ->GetWorldCoordinatesFromImageCoordinates(xyzBB);

        bool added = tempStructure.InsertElement(time, xyz);
        if ( !added )
          {
          std::cout << "Element at a time point: " << time
                    << "could not be added." << std::endl;
          }
        ++begin;
        }
      m_Container.get< TraceID >().replace(it, tempStructure);

      // Reconstruct the polydata
      UpdateTrackStructurePolyData(*it);
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::RecomputeCurrentElementMap(std::list< double * > iPoints)
{
  if ( this->m_ImageView )
    {
    // empty current element map
    PointsMapConstIterator begin = this->m_CurrentElement.PointsMap.begin();
    PointsMapConstIterator end = this->m_CurrentElement.PointsMap.end();

    while ( begin != end )
      {
      // free memory
      delete[] begin->second;
      ++begin;
      }

    this->m_CurrentElement.PointsMap.clear();

    // add points to the map
    std::list< double * >::iterator beginList = iPoints.begin();
    std::list< double * >::iterator endList = iPoints.end();

    while ( beginList != endList )
      {
      int xyzBB[3] = {
        static_cast< int >( ( *beginList )[0] ),
        static_cast< int >( ( *beginList )[1] ),
        static_cast< int >( ( *beginList )[2] )
        };

      unsigned int time = static_cast< unsigned int >( ( *beginList )[3] );

      // convert xyz coordinates
      double *xyz = m_ImageView->GetImageViewer(0)
        ->GetWorldCoordinatesFromImageCoordinates(xyzBB);

      bool addPoint = this->m_CurrentElement.InsertElement(time, xyz);

      if ( !addPoint )
        {
        std::cout << "problem while inserting element in the map" << std::endl;
        std::cout << " x: " << xyz[0] << " y: " << xyz[1] << " z: " << xyz[2]
                  << " t: " << xyz[3] << std::endl;
        }

      ++beginList;
      }

    bool IsANewTrack = ( this->m_CurrentElement.Nodes == NULL );
    // Create a new polydata and new actors if it is a new track
    if ( IsANewTrack )
      {
      //Create new polydata (new address)
      this->m_CurrentElement.Nodes = vtkPolyData::New();
      }

    // update the polydata (which represents the current track)
    UpdateTrackStructurePolyData(this->m_CurrentElement);

    // if it is a new track, we need to add the actors in viewer / scene
    if ( IsANewTrack )
      {
      // add actors in the visualization with given property
      CreateCurrentTrackActors();
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdatePointsForATrack(unsigned int iTrackID,
                                      std::list< double * > iListCenterBoundingBoxes)
{
  if ( iTrackID != 0 )
    {
    bool updateCurrentElement = this->UpdateCurrentElementFromExistingOne(iTrackID);
    if ( updateCurrentElement )
      {
      this->RecomputeCurrentElementMap(iListCenterBoundingBoxes);
      }
    else
      {
      std::cout << "TrackID: " << iTrackID << " not found" << std::endl;
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdateElementHighlightingWithGivenTraceIDs(const QStringList & iList,
                                                           const Qt::CheckState & iCheck)
{
  Superclass::UpdateElementHighlightingWithGivenTraceIDsBase(iList,
                                                             iCheck);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdateElementVisibilityWithGivenTraceIDs(const QStringList & iList,
                                                         const Qt::CheckState & iCheck)
{
  Superclass::UpdateElementVisibilityWithGivenTraceIDsBase(iList, iCheck);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< vtkActor * >
TrackContainer::AddTrace(vtkPolyData *iNode, vtkProperty *iProperty)
{
  if ( this->m_ImageView )
    {
    return this->m_ImageView->AddContour(iNode, iProperty);
    }
  else
    {
    return std::vector< vtkActor * >();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::ChangeColorCode(const char *iColorCode)
{
  m_ActiveScalars.clear();
  m_ActiveScalars.append(iColorCode);

  if ( m_ActiveScalars.compare("Original") )
    {
    // get range for the tracks
    double *range = setNodeScalars(iColorCode);

    // associated LUT
    vtkSmartPointer< vtkLookupTable > LUT = vtkSmartPointer< vtkLookupTable >::New();
    LUT->SetTableRange(range);
    LUT->SetNumberOfTableValues(1024);
    LUT->SetHueRange(0, 0.7);
    LUT->SetSaturationRange(1, 1);
    LUT->SetValueRange(1, 1);
    LUT->Build();

    SetScalarRangeForAllElements(range[0], range[1]);
    SetLookupTableForColorCoding(LUT);

    delete[] range;
    }
  else
    {
    this->RenderAllElementsWithOriginalColors();
    }
}

//-------------------------------------------------------------------------
double *
TrackContainer::setNodeScalars(const char *iArrayName)
{
  double *range =  new double[2];

  range[0] = std::numeric_limits< double >::max();
  range[1] = std::numeric_limits< double >::min();

  MultiIndexContainerType::index< TraceID >::type::iterator
    it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
    {
    // does the track have a polydata
    if ( it->Nodes )
      {
      double *realTime =
        it->Nodes->GetPointData()->GetArray(iArrayName)->GetRange();
      range[0] = std::min(range[0], realTime[0]);
      range[1] = std::max(range[1], realTime[1]);

      //set active scalar
      it->Nodes->GetPointData()->SetActiveScalars(iArrayName);
      }

    ++it;
    }

  return range;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::setTimeInterval(int iTimeInterval)
{
  m_TimeInterval = iTimeInterval;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
TrackContainer::getTimeInterval()
{
  return m_TimeInterval;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdateTracksRepresentation(double iRadius, double iRadius2)
{
  MultiIndexContainerType::iterator it = m_Container.begin();

  while ( it != m_Container.end() )
    {
    // restore original polydata
    UpdateTrackStructurePolyData( ( *it ) );

    // add glyphs if necessary
    if ( ( iRadius || iRadius2 ) && it->Nodes )
      {
      it->UpdateTracksRepresentation(iRadius, iRadius2);
      }
    ++it;
    }

  // update color since active scalar is set to NULL in
  // UpdateTrackStructurePolyData
  QByteArray  bytes  = m_ActiveScalars.toAscii();
  const char *ptr    = bytes.data();
  ChangeColorCode(ptr);

  if ( this->m_ImageView )
    {
    m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double*
TrackContainer::GetLastPointOfTheTrack(unsigned int iTrackID)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iTrackID);

  vtkPoints* points = it->Nodes->GetPoints();
  vtkIdType nbOfPoints = points->GetNumberOfPoints();
  std::cout <<"LAST POINT: X: " << points->GetPoint(nbOfPoints-1)[0]
                      << " Y: " << points->GetPoint(nbOfPoints-1)[1]
                      << " Z: " << points->GetPoint(nbOfPoints-1)[2]
  << std::endl;
  return points->GetPoint(nbOfPoints-1);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double*
TrackContainer::GetFirstPointOfTheTrack(unsigned int iTrackID)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iTrackID);

  vtkPoints* points = it->Nodes->GetPoints();
  std::cout <<"FIRST POINT: X: " << points->GetPoint(0)[0]
                      << " Y: " << points->GetPoint(0)[1]
                      << " Z: " << points->GetPoint(0)[2]
  << std::endl;
  return points->GetPoint(0);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetListOfDivisions( std::list<unsigned int> iListOfDivisions)
{
  // Create list iterator
  std::list<unsigned int>::iterator it = iListOfDivisions.begin();

  while( it != iListOfDivisions.end() )
    {
    // get ids
    unsigned int mother = *it;
    ++it;
    unsigned int daughter1 = *it;
    ++it;
    unsigned int daughter2 = *it;
    ++it;
    // create connections and actor for the division of interest
    AddDivision(mother, daughter1, daughter2);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
AddDivision( unsigned int iMotherID, unsigned int iDaughter1ID,
    unsigned int iDaughter2ID)
{
  // get address of the structures of interest
  //------------------------------
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMotherID);
  MultiIndexContainerTraceIDIterator daughter1It
      = m_Container.get< TraceID >().find(iDaughter1ID);
  MultiIndexContainerTraceIDIterator daughter2It
      = m_Container.get< TraceID >().find(iDaughter2ID);

  // create connections
  //------------------------------
  // motherID->D1
  //         ->D2
  //------------------------------
  // Create temporary structures so we can modify it
  TrackStructure tempMother(*motherIt);
  // Update daughters pointers
  tempMother.TreeNode.m_Child[0] = &(*daughter1It);
  tempMother.TreeNode.m_Child[1] = &(*daughter2It);
  // Create Actor
  tempMother.TreeNode.m_DivisionActor =
      CreateDivisionActor(iMotherID, iDaughter1ID, iDaughter2ID);
  // Push current element
  m_Container.get< TraceID >().replace(motherIt, tempMother);

  //------------------------------
  // D1->motherID
  // D2->motherID
  //------------------------------
  // Create temporary structures so we can modify it
  TrackStructure tempDaughter1(*daughter1It);
  // Update daughters pointers
  tempDaughter1.TreeNode.m_Mother = &(*motherIt);
  // Push current element
  m_Container.get< TraceID >().replace(daughter1It, tempDaughter1);

  // Create temporary structures so we can modify it
  TrackStructure tempDaughter2(*daughter2It);
  // Update daughters pointers
  tempDaughter2.TreeNode.m_Mother = &(*motherIt);
  // Push current element
  m_Container.get< TraceID >().replace(daughter1It, tempDaughter2);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkActor* >
TrackContainer::
CreateDivisionActor( unsigned int iMother, unsigned int iDaughter1, unsigned int iDaughter2)
{
  // Arnaud: what about if any of the parameter is NULL?
  // Arnaud: what about if one daughter is in the field of view,
  // and not the other one?
  // Nicolas: dont deal with any of that yet

  // Get points of interest:
  // Mother: last point
  // D1 & D2: first point
  double* mother = this->GetLastPointOfTheTrack(iMother);
  double* daughter1 = this->GetFirstPointOfTheTrack(iDaughter1);
  double* daughter2 = this->GetFirstPointOfTheTrack(iDaughter2);

  //setup points (geometry)
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint ( daughter1[0], daughter1[1], daughter1[2] );
  points->InsertNextPoint ( mother[0], mother[1], mother[2] );
  points->InsertNextPoint ( daughter2[0], daughter2[1], daughter2[2] );

  // create the lines
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  for(int i=0; i<2; ++i)
    {
    //Create the first line (between Origin and P0)
    vtkSmartPointer<vtkLine> line =
        vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetId(0,i);
    line->GetPointIds()->SetId(1,i+1);
    lines->InsertNextCell(line);
    }

  // create polydata
  vtkSmartPointer<vtkPolyData> division = vtkSmartPointer<vtkPolyData>::New();
  division->SetPoints(points);
  division->SetLines(lines);
  /*
   * \todo Nicolas: add array in polydata containing trackID
   * so we have the track ID from the actor directly
   */

  /*
   * \todo Nicolas: Which color should it be? White as of now
   */
  vtkProperty *trace_property = vtkProperty::New();
  double       r = 1.0;
  double       g = 1.0;
  double       b = 1.0;
  double       a = 1.0;

  trace_property->SetColor(r,
                           g,
                           b);
  trace_property->SetOpacity(a);

  // create actors and add it to the visualization
  std::vector< vtkActor * > divisionActors =
        m_ImageView->AddContour( division, trace_property );

  return divisionActors;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
CutLineage(unsigned int iMotherID)
{
  // update connections
  //------------------------------
  // motherID->NULL
  //         ->NULL
  //------------------------------
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMotherID);
  // Create temporary structures so we can modify it
  TrackStructure tempMother(*motherIt);
  // Get daughters IDs
  unsigned int daughter1ID = (*(tempMother.TreeNode.m_Child[0])).TraceID;
  unsigned int daughter2ID = (*(tempMother.TreeNode.m_Child[2])).TraceID;
  // Update daughters pointers
  tempMother.TreeNode.m_Child[0] = NULL;
  tempMother.TreeNode.m_Child[1] = NULL;
  tempMother.TreeNode.DeleteActors();
  // Push current element
  m_Container.get< TraceID >().replace(motherIt, tempMother);

  // Find the daughter1 and update mother connection to NULL
  //------------------------------
  MultiIndexContainerTraceIDIterator daughter1It
      = m_Container.get< TraceID >().find(daughter1ID);
  // Create temporary structures so we can modify it
  TrackStructure tempDaughter1(*daughter1It);
  // Update daughters pointers
  tempDaughter1.TreeNode.m_Mother = NULL;
  // Push current element
  m_Container.get< TraceID >().replace(daughter1It, tempDaughter1);

  // Find the daughter2 and update mother connection to NULL
  //------------------------------
  MultiIndexContainerTraceIDIterator daughter2It
      = m_Container.get< TraceID >().find(daughter2ID);
  // Create temporary structures so we can modify it
  TrackStructure tempDaughter2(*daughter2It);
  // Update daughters pointers
  tempDaughter2.TreeNode.m_Mother = NULL;
  // Push current element
  m_Container.get< TraceID >().replace(daughter2It, tempDaughter2);

}
