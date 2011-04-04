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

#include "vtkLookupTable.h"
#include <limits>

#include <QDebug>

//-------------------------------------------------------------------------
TrackContainer::TrackContainer(QObject *iParent, QGoImageView3D *iView) : Superclass(iParent, iView)
{
  m_TimeInterval = 0;
  m_ActiveTrackScalars.append("Original");
  m_ActiveDivisionScalars.append("Original");
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
  assert ( iIter != m_Container.get< TraceID >().end() );
  assert ( this->m_ImageView );

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

  iIter->ReleaseData();

  m_Container.get< TraceID >().erase(iIter);
  m_ImageView->UpdateRenderWindows();

  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
TrackContainer::DeleteAllHighlightedElements()
{
  assert ( this->m_ImageView );

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

    it0->ReleaseData();

    it_t = it0;
    ++it0;

    m_Container.get< Highlighted >().erase(it_t);
    }

  m_ImageView->UpdateRenderWindows();

  return oList;
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

  vtkSmartPointer<vtkIntArray> trackIDArray = vtkSmartPointer<vtkIntArray>::New();
  trackIDArray->SetNumberOfComponents(1);
  trackIDArray->SetNumberOfValues(1);
  trackIDArray->SetName("TRACK");
  trackIDArray->SetValue(0,iTrackStructure.TraceID);

  polyData->GetFieldData()->AddArray(trackIDArray);

  iTrackStructure.Nodes->DeepCopy(polyData);
  //update speed information
  iTrackStructure.ComputeAttributes();

  iTrackStructure.Nodes->GetPointData()->SetActiveScalars(NULL);

  // update the lineage if necessary
  //UpdateTrackStructureLineage(const_cast<TrackStructure*>(&(iTrackStructure)));

  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::ImportTrackInCurrentElement(std::map< unsigned int, double * > iMeshes)
{
  std::map< unsigned int, double * >::iterator beginMesh = iMeshes.begin();
  std::map< unsigned int, double * >::iterator endMesh = iMeshes.end();

  while ( beginMesh != endMesh )
    {
    this->m_CurrentElement.InsertElement(beginMesh->first,
                                           beginMesh->second);
    ++beginMesh;
    }

  this->m_CurrentElement.Visible = true;

  this->m_CurrentElement.Nodes = vtkPolyData::New();

  UpdateTrackStructurePolyData(this->m_CurrentElement);

  CreateTrackActors(this->m_CurrentElement);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::CreateTrackActors( TrackStructure& iStructure )
{
  assert( this->m_ImageView );

  //Create new actors (new address)
  vtkProperty *trace_property = vtkProperty::New();
  double       r = iStructure.rgba[0];
  double       g = iStructure.rgba[1];
  double       b = iStructure.rgba[2];
  double       a = iStructure.rgba[3];

  trace_property->SetColor(r,
                           g,
                           b);
  trace_property->SetOpacity(a);

  // Add contour
  std::vector< vtkActor * > trackActors =
    m_ImageView->AddContour(iStructure.Nodes, trace_property);

  //has actor being created?
  assert(trackActors[0]);

  // add actors address to structure
  iStructure.ActorXY = trackActors[0];
  iStructure.ActorXZ = trackActors[1];
  iStructure.ActorYZ = trackActors[2];
  iStructure.ActorXYZ = trackActors[3];

  this->m_ImageView->AddActor(3, trackActors[3]);

  trace_property->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::UpdateTrackActors( TrackStructure& iStructure )
{
  if(iStructure.Nodes->GetNumberOfPoints() < 2 )
  {
  // clean actor
  this->m_ImageView->RemoveActor(0, iStructure.TreeNode.ActorXY);
  this->m_ImageView->RemoveActor(1, iStructure.TreeNode.ActorXZ);
  this->m_ImageView->RemoveActor(2, iStructure.TreeNode.ActorYZ);
  this->m_ImageView->RemoveActor(3, iStructure.TreeNode.ActorXYZ);
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
TrackStructure*
TrackContainer::
UpdatePointsForATrack(unsigned int iTrackID,
                      std::list< double * > iListCenterBoundingBoxes)
{
  assert( iTrackID != 0 );

  assert ( this->m_ImageView );

  // get pointer to the track
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  assert ( motherIt != m_Container.get< TraceID >().end() );

  /*
   * \todo Nicolas- const_cast is OK to modify polydata but better avoid it
   */
  TrackStructure* mother =  const_cast<TrackStructure*>(&(*motherIt));
  RecomputeMap(mother, iListCenterBoundingBoxes);

  // if the element has no polydata create a new address for the polydata
  if ( ! mother->Nodes )
    {
    //Create new polydata (new address)
    mother->Nodes = vtkPolyData::New();
    }

  // update the polydata (which represents the current track)
  UpdateTrackStructurePolyData( *mother );

  // if element has no actors, create it
  if ( ! mother->ActorXY )
    {
    // add actors in the visualization with given property
    CreateTrackActors( *mother );
    return mother;
    }

  UpdateTrackActors( *mother );

  return mother;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::RecomputeMap(TrackStructure* iStructure, std::list< double * > iPoints)
{

  // empty current element map
  PointsMapConstIterator begin = iStructure->PointsMap.begin();
  PointsMapConstIterator end = iStructure->PointsMap.end();

  while ( begin != end )
    {
    // free memory
    delete[] begin->second;
    ++begin;
    }

  iStructure->PointsMap.clear();

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

  bool insertElement = iStructure->InsertElement(time, xyz);

  assert ( insertElement );

  ++beginList;
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
void
TrackContainer::ChangeColorCode(const char *iColorCode)
{
  m_ActiveTrackScalars.clear();
  m_ActiveTrackScalars.append(iColorCode);

  if ( m_ActiveTrackScalars.compare("Original") )
    {
    // get range for the tracks
    double *range = setTrackNodeScalars(iColorCode);

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

//-------------------------------------------------------------------------
void
TrackContainer::ChangeDivisionsColorCode(const char *iColorCode)
{
  m_ActiveDivisionScalars.clear();
  m_ActiveDivisionScalars.append(iColorCode);

  if ( m_ActiveDivisionScalars.compare("Original") )
    {
    // get range for the division
    double *range = setDivisionNodeScalars(iColorCode);

    // associated LUT
    vtkSmartPointer< vtkLookupTable > LUT = vtkSmartPointer< vtkLookupTable >::New();
    LUT->SetTableRange(range);
    LUT->SetNumberOfTableValues(1024);
    LUT->SetHueRange(0, 0.7);
    LUT->SetSaturationRange(1, 1);
    LUT->SetValueRange(1, 1);
    LUT->Build();

    SetScalarRangeForAllDivisions(range[0], range[1]);
    SetLookupTableForAllDivisionsColorCoding(LUT);
    }
  else
    {
    RenderAllDivisionsWithOriginalColors();
    }

  assert ( m_ImageView );
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetScalarRangeForAllDivisions(double iMin, double iMax)
{
  MultiIndexContainerType::index< TraceID >::type::iterator
    it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
   {
    if ( !it->IsLeaf() )
      {
      it->TreeNode.SetScalarRange(iMin, iMax);
      }
   ++it;
   }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetLookupTableForAllDivisionsColorCoding(vtkLookupTable *iLut)
{
  MultiIndexContainerType::index< TraceID >::type::iterator
    it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
   {
    if ( !it->IsLeaf() )
      {
      it->TreeNode.SetLookupTable(iLut);
      }
   ++it;
   }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
RenderAllDivisionsWithOriginalColors()
{
  MultiIndexContainerType::index< TraceID >::type::iterator
    it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
   {
    if ( !it->IsLeaf() )
      {
      it->TreeNode.RenderWithOriginalColors();
      }
   ++it;
   }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double *
TrackContainer::setTrackNodeScalars(const char *iArrayName)
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
double *
TrackContainer::setDivisionNodeScalars(const char *iArrayName)
{
  double *range =  new double[2];

  range[0] = std::numeric_limits< double >::max();
  range[1] = std::numeric_limits< double >::min();

  MultiIndexContainerType::index< TraceID >::type::iterator
    it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
    {
    // does the division have a polydata
    if ( !it->IsLeaf() )
      {
      double *realTime =
        it->TreeNode.Nodes->GetPointData()->GetArray(iArrayName)->GetRange();
      range[0] = std::min(range[0], realTime[0]);
      range[1] = std::max(range[1], realTime[1]);

      //set active scalar
      it->TreeNode.Nodes->GetPointData()->SetActiveScalars(iArrayName);
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
    bool pointsInPolydata = UpdateTrackStructurePolyData( ( *it ) );

    // add glyphs if necessary
    if ( ( iRadius || iRadius2 ) && pointsInPolydata )
      {
      it->UpdateTracksRepresentation(iRadius, iRadius2);
      }
    ++it;
    }

  // update color since active scalar is set to NULL in
  // UpdateTrackStructurePolyData
  QByteArray  bytes  = m_ActiveTrackScalars.toAscii();
  const char *ptr    = bytes.data();
  ChangeColorCode(ptr);

  assert ( this->m_ImageView );

  this->m_ImageView->UpdateRenderWindows();
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
    AddDivision(mother, daughter1, daughter2, false);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
AddDivision( unsigned int iMotherID, unsigned int iDaughter1ID,
    unsigned int iDaughter2ID, bool iVisible)
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
  TrackStructure* mother = const_cast<TrackStructure*>(&(*motherIt));
  mother->TreeNode.m_Child[0] = const_cast<TrackStructure*>(&(*daughter1It));
  mother->TreeNode.m_Child[1] = const_cast<TrackStructure*>(&(*daughter2It));
  // Create Polydata
  CreateDivisionPolydata(iMotherID, iDaughter1ID, iDaughter2ID);
  // Create Actor
  std::vector< vtkActor * > actors =
      CreateDivisionActor(mother->TreeNode.Nodes, iVisible);
  mother->TreeNode.ActorXY = actors[0];
  mother->TreeNode.ActorXZ = actors[1];
  mother->TreeNode.ActorYZ = actors[2];
  mother->TreeNode.ActorXYZ = actors[3];
  mother->TreeNode.Visible = iVisible;

  //------------------------------
  // D1->motherID
  // D2->motherID
  //------------------------------
  TrackStructure* d1 =  const_cast<TrackStructure*>(&(*daughter1It));
  d1->TreeNode.m_Mother = const_cast<TrackStructure*>(&(*motherIt));

  TrackStructure* d2 =  const_cast<TrackStructure*>(&(*daughter2It));
  d2->TreeNode.m_Mother = const_cast<TrackStructure*>(&(*motherIt));

  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------
void
TrackContainer::
CreateDivisionPolydata(unsigned int iMother, unsigned int iDaughter1, unsigned int iDaughter2)
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
   * \todo Nicolas: we might want to extend it to more parameters: color, ...
   * and to the meshes->efficiency of show/hide, picking, boxwidget, ...!
   */
  // add track ID to the polydata so we can get it from the actor later on
  vtkSmartPointer<vtkIntArray> trackIDArray = vtkSmartPointer<vtkIntArray>::New();
  trackIDArray->SetNumberOfComponents(1);
  trackIDArray->SetNumberOfValues(1);
  trackIDArray->SetName("DIVISION");
  trackIDArray->SetValue(0,iMother);

  division->GetFieldData()->AddArray(trackIDArray);

  // update structure
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMother);
  m_Container.get< TraceID >().modify( motherIt , create_node_division(division) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkActor* >
TrackContainer::
CreateDivisionActor(vtkPolyData* iPolyData, bool iVisible)
{
  /*
   * \todo Nicolas: Which color should it be? White as of now
   */
  vtkSmartPointer<vtkProperty> trace_property =
      vtkSmartPointer<vtkProperty>::New();
  double       r = 1.0;
  double       g = 1.0;
  double       b = 1.0;
  double       a = 1.0;

  trace_property->SetColor(r,
                           g,
                           b);
  trace_property->SetOpacity(a);

  std::vector< vtkActor * > divisionActors =
      this->m_ImageView->AddContour( iPolyData, trace_property );

  // add it to visu??
  if( iVisible )
    {
    // add to 3d
    this->m_ImageView->AddActor(3, divisionActors[3]);
    }
  else
    {
    // remove from 2d
    this->m_ImageView->RemoveActor(0, divisionActors[0]);
    this->m_ImageView->RemoveActor(1, divisionActors[1]);
    this->m_ImageView->RemoveActor(2, divisionActors[2]);
    }

  return divisionActors;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
CutLineage(unsigned int iMotherID)
{
  std::cout << "CutLineage can't work because of temp structures - use modify instead"
      << std::endl;
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
  tempMother.TreeNode.ReleaseData();
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
HighlightCollection(unsigned int iRootTrackID, bool iHilighted)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iRootTrackID);

  UpdateCollectionHighlighted(motherIt, iHilighted);

  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
ShowCollection(unsigned int iRootTrackID, bool iVisible)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iRootTrackID);

  UpdateCollectionVisibility(motherIt, iVisible);

  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionVisibility( MultiIndexContainerTraceIDIterator& it, bool iVisible)
{
  if( !it->IsLeaf() )
    {
    ModifyDivisionVisibility(it, iVisible);
    }

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    UpdateCollectionVisibility(childIt,iVisible);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    UpdateCollectionVisibility(childIt,iVisible);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionHighlighted( MultiIndexContainerTraceIDIterator& it, bool iHighlighted)
{
  if( !it->IsLeaf() )
    {
    ModifyDivisionHighlight(it, iHighlighted);
    }

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    UpdateCollectionHighlighted(childIt,iHighlighted);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    UpdateCollectionHighlighted(childIt,iHighlighted);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
TrackContainer::ModifyDivisionVisibility( MultiIndexContainerTraceIDIterator& it, bool iVisible)
{
  m_Container.get< TraceID >().modify( it , change_visible_division(iVisible) );

  typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
  ImageViewMember f;

  if ( iVisible )
    {
    f = &QGoImageView3D::AddActor;
    }
  else
    {
    f = &QGoImageView3D::RemoveActor;
    }

  assert( m_ImageView );
  ( m_ImageView->*f )(0, it->TreeNode.ActorXY);
  ( m_ImageView->*f )(1, it->TreeNode.ActorXZ);
  ( m_ImageView->*f )(2, it->TreeNode.ActorYZ);
  ( m_ImageView->*f )(3, it->TreeNode.ActorXYZ);

  return 1;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
TrackContainer::ModifyDivisionHighlight( MultiIndexContainerTraceIDIterator& it, bool iHighlight )
{
  vtkProperty* temp_property = NULL;
  if ( !iHighlight )
    {
    /*
     * \todo Nicolas - which color for the divisions??
     */
    temp_property = vtkProperty::New();
    temp_property->SetColor(it->TreeNode.rgba[0],
                            it->TreeNode.rgba[1],
                            it->TreeNode.rgba[2]);
    temp_property->SetOpacity(it->TreeNode.rgba[3]);
    temp_property->SetLineWidth(this->m_IntersectionLineWidth);
    }
  else
    {
    temp_property = this->m_HighlightedProperty;
    }

  m_Container.get< TraceID >().modify( it , change_highlighted_division(temp_property, iHighlight) );

  if(!iHighlight)
    {
    temp_property->Delete();
    }

  return 1;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateTrackStructureLineage(TrackStructure* iStructure)
{
  // Modify Mother
  if( ! iStructure->IsRoot() )
  {
    // What if new tracks overlap now???
    std::cout << "Update mother division polydata/actor" << std::endl;
    UpdateDivisionActor( iStructure->TreeNode.m_Mother );
  }

  // Modify Daughters
  if( ! iStructure->IsLeaf() )
  {
    UpdateDivisionActor( iStructure );
    std::cout << "Update daughter divisions polydata/actor" << std::endl;
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionActor(TrackStructure* iStructure)
{
  double* mother = this->GetLastPointOfTheTrack(iStructure->TraceID);
  double* daughter1 = this->GetFirstPointOfTheTrack(iStructure->TreeNode.m_Child[0]->TraceID);
  double* daughter2 = this->GetFirstPointOfTheTrack(iStructure->TreeNode.m_Child[1]->TraceID);

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
   * \todo Nicolas: we might want to extend it to more parameters: color, ...
   * and to the meshes->efficiency of show/hide, picking, boxwidget, ...!
   */
  // add track ID to the polydata so we can get it from the actor later on
  vtkSmartPointer<vtkIntArray> trackIDArray = vtkSmartPointer<vtkIntArray>::New();
  trackIDArray->SetNumberOfComponents(1);
  trackIDArray->SetNumberOfValues(1);
  trackIDArray->SetName("DIVISION");
  trackIDArray->SetValue(0,iStructure->TraceID);

  division->GetFieldData()->AddArray(trackIDArray);

  vtkSmartPointer<vtkProperty> trace_property =
      vtkSmartPointer<vtkProperty>::New();
  double       r = 1.0;
  double       g = 1.0;
  double       b = 1.0;
  double       a = 1.0;

  trace_property->SetColor(r,
                           g,
                           b);
  trace_property->SetOpacity(a);

  // remove actor from visu and delete them
  this->m_ImageView->RemoveActor(0, iStructure->TreeNode.ActorXY);
  this->m_ImageView->RemoveActor(1, iStructure->TreeNode.ActorXZ);
  this->m_ImageView->RemoveActor(2, iStructure->TreeNode.ActorYZ);
  this->m_ImageView->RemoveActor(3, iStructure->TreeNode.ActorXYZ);
  iStructure->TreeNode.ReleaseData();

  // create actors and add it to the visualization
  std::vector< vtkActor * > divisionActors =
        m_ImageView->AddContour( division, trace_property );
  iStructure->TreeNode.ActorXY = divisionActors[0];
  iStructure->TreeNode.ActorXZ = divisionActors[1];
  iStructure->TreeNode.ActorYZ = divisionActors[2];
  iStructure->TreeNode.ActorXYZ = divisionActors[3];

  this->m_ImageView->AddActor(3, divisionActors[3]);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
DeleteADivision( unsigned int iMotherID)
{
  // find iterator
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMotherID);

  assert( motherIt != m_Container.get< TraceID >().end() );

  TrackStructure* mother =  const_cast<TrackStructure*>(&(*motherIt));

 // Delete the actor
  // remove actor from visu and delete them
  this->m_ImageView->RemoveActor(0, mother->TreeNode.ActorXY);
  this->m_ImageView->RemoveActor(1, mother->TreeNode.ActorXZ);
  this->m_ImageView->RemoveActor(2, mother->TreeNode.ActorYZ);
  this->m_ImageView->RemoveActor(3, mother->TreeNode.ActorXYZ);
  mother->TreeNode.ReleaseData();

  // Reset pointers
  // child (to do first)
  mother->TreeNode.m_Child[0]->TreeNode.m_Mother = NULL;
  mother->TreeNode.m_Child[1]->TreeNode.m_Mother = NULL;

  // mother
  mother->TreeNode.m_Child[0] = NULL;
  mother->TreeNode.m_Child[1] = NULL;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int>
TrackContainer::
GetSubLineage( unsigned int iTrackID )
{
  std::list<unsigned int> listOfIDs;

  // find the iterator
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  UpdateSubLineage(motherIt, listOfIDs);

  return listOfIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateSubLineage( MultiIndexContainerTraceIDIterator it, std::list<unsigned int>& iList)
{
  iList.push_back( it->TraceID );

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    UpdateSubLineage(childIt,iList);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    UpdateSubLineage(childIt,iList);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionHighlighting(unsigned int iTraceId)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTraceId);

  GetRootIterator( motherIt );

  bool highlight = !motherIt->TreeNode.Highlighted;

  UpdateCollectionHighlighted(motherIt, highlight);

  this->m_ImageView->UpdateRenderWindows();

  // send signal to track manager with TraceID
  emit GetCollectionIDForHighlgiht( motherIt->TraceID );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
GetRootIterator(MultiIndexContainerTraceIDIterator& iMotherIterator)
{
  if( iMotherIterator->IsRoot() )
  {
    return;
  }

  iMotherIterator
      = m_Container.get< TraceID >().find( iMotherIterator->TreeNode.m_Mother->TraceID );
  GetRootIterator( iMotherIterator );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionScalars( unsigned int iTrackID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  UpdateDivisionScalar(motherIt, 0); // 0=depth of the root
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionScalar(MultiIndexContainerTraceIDIterator& it, unsigned int iDepth)
{
  if( !it->IsLeaf() )
    {
    //add array
    vtkSmartPointer< vtkIntArray > depthArray =
        vtkSmartPointer< vtkIntArray >::New();
    depthArray->SetNumberOfComponents(1);
    depthArray->SetName("DepthInformation");
    depthArray->InsertNextValue(iDepth);
    depthArray->InsertNextValue(iDepth);
    depthArray->InsertNextValue(iDepth);
    m_Container.get< TraceID >().modify( it , add_array_division(depthArray) );
    }

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    UpdateDivisionScalar(childIt,iDepth+1);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    UpdateDivisionScalar(childIt,iDepth+1);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionColors( unsigned int iTrackID, double* color)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  UpdateDivisionColor(motherIt, color);

  m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionColor(MultiIndexContainerTraceIDIterator& it, double* iColor)
{
  if( !it->IsLeaf() )
    {
    m_Container.get< TraceID >().modify( it , change_color_division(iColor) );
    }

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    UpdateDivisionColor(childIt,iColor);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    UpdateDivisionColor(childIt,iColor);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int
TrackContainer::
GetCollectionDepth( unsigned int iTrackRootID )
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackRootID);

  unsigned int depth = 0;
  UpdateCollectionDepth(motherIt, 0, depth); //0: root depth

  return depth;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionDepth(MultiIndexContainerTraceIDIterator& it,
    unsigned int iDivisionDepth, unsigned int& iLineageDepth)
{
  if( !it->IsLeaf() )
    {
    if( iDivisionDepth > iLineageDepth )
      {
      iLineageDepth = iDivisionDepth;
      }
    }

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    UpdateCollectionDepth(childIt,iDivisionDepth+1,iLineageDepth);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    UpdateCollectionDepth(childIt,iDivisionDepth+1, iLineageDepth);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkMutableDirectedGraph*
TrackContainer::
ExportLineage(unsigned int iTrackID)
{

  std::cout<<"root id: " << iTrackID << std::endl;

  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  vtkMutableDirectedGraph* graph = vtkMutableDirectedGraph::New();

  unsigned int pedigree = 0;
  UpdateLineage(motherIt, graph, pedigree, 0);

  return graph;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateLineage(MultiIndexContainerTraceIDIterator& it,
    vtkMutableDirectedGraph* iGraph, unsigned int iPedrigree, vtkIdType mother)
{
  vtkIdType motherPedigree = iPedrigree;

  // update tree
  if( iPedrigree == 0 )
    {
    std::cout << "add vertex" << std::endl;
    motherPedigree = iGraph->AddVertex();
    }

  if( it->IsLeaf() )
    {
    return;
    }

  if(it->TreeNode.m_Child[0])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[0]->TraceID);
    // add edge
    std::cout << "add child" << std::endl;
    iPedrigree = iGraph->AddChild(motherPedigree);
    UpdateLineage(childIt,iGraph, iPedrigree, motherPedigree);
    }

  if(it->TreeNode.m_Child[1])
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find(it->TreeNode.m_Child[1]->TraceID);
    // add edge
    std::cout << "add child" << std::endl;
    iPedrigree = iGraph->AddChild(motherPedigree);
    UpdateLineage(childIt,iGraph, iPedrigree, motherPedigree);
    }
}
//-------------------------------------------------------------------------
