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

#include "vtkStringArray.h"

#include <limits>

#include <QDebug>

//-------------------------------------------------------------------------
TrackContainer::
TrackContainer(QObject *iParent, QGoImageView3D *iView):
  Superclass(iParent, iView)
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
TrackContainer::
DeleteElement(const unsigned int & iId)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  return DeleteElement(it);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
TrackContainer::
DeleteElement(MultiIndexContainerTraceIDIterator iIter)
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
TrackContainer::
DeleteAllHighlightedElements()
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
TrackContainer::
UpdateTrackStructurePolyData(const TrackStructure & iTrackStructure)
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

  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
ImportTrackInCurrentElement(std::map<unsigned int, double*>& iMeshes)
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
TrackContainer::
CreateTrackActors(TrackStructure& iStructure)
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
TrackContainer::
UpdateTrackActors(TrackStructure& iStructure )
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
UpdatePointsForATrack(const unsigned int& iTrackID,
                      std::list< double * >& iListCenterBoundingBoxes)
{
  assert( iTrackID != 0 );

  assert ( this->m_ImageView );

  // get pointer to the track
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  assert ( motherIt != m_Container.get< TraceID >().end() );

  /*
   * \note Nicolas- const_cast is OK to modify polydata in the container since
    we don't sort on it but better avoid it
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
TrackContainer::
RecomputeMap(TrackStructure* iStructure, std::list< double * >& iPoints)
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
TrackContainer::
UpdateElementHighlightingWithGivenTraceIDs(const QStringList & iList,
                                           const Qt::CheckState & iCheck)
{
 
  Superclass::UpdateElementHighlightingWithGivenTraceIDsBase(iList,
                                                               iCheck);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateElementVisibilityWithGivenTraceIDs(const QStringList & iList,
                                         const Qt::CheckState & iCheck)
{
  Superclass::UpdateElementVisibilityWithGivenTraceIDsBase(iList, iCheck);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
ChangeColorCode(const QString& iColorCode)
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
TrackContainer::
ChangeDivisionsColorCode(const QString& iColorCode)
{
  m_ActiveDivisionScalars.clear();
  m_ActiveDivisionScalars.append(iColorCode);

  if ( m_ActiveDivisionScalars.compare("Original") )
    {
    // get range for the division
    double *range = setDivisionNodeScalars(iColorCode);

    // associated LUT
    vtkSmartPointer< vtkLookupTable > LUT =
        vtkSmartPointer< vtkLookupTable >::New();
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
    this->RenderAllDivisionsWithOriginalColors();
    }

  assert ( m_ImageView );
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetScalarRangeForAllDivisions(const double& iMin, const double& iMax)
{
  MultiIndexContainerTraceIDIterator
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
SetLookupTableForAllDivisionsColorCoding(const vtkLookupTable *iLut)
{
  MultiIndexContainerTraceIDIterator
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
  MultiIndexContainerTraceIDIterator
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
TrackContainer::
setTrackNodeScalars(const QString& iArrayName)
{
  double *range =  new double[2];

  range[0] = std::numeric_limits< double >::max();
  range[1] = std::numeric_limits< double >::min();

  MultiIndexContainerTraceIDIterator
      it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
    {
    // does the track have a polydata
    if ( it->Nodes )
      {
      // convert qstring to const char*
      double *realTime =
        it->Nodes->GetPointData()->GetArray(
            iArrayName.toLocal8Bit().data())->GetRange();
      range[0] = std::min(range[0], realTime[0]);
      range[1] = std::max(range[1], realTime[1]);

      //set active scalar
      // convert qstring to const char*
      it->Nodes->GetPointData()->SetActiveScalars(
            iArrayName.toLocal8Bit().data());
      }
    ++it;
    }

  return range;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double *
TrackContainer::
setDivisionNodeScalars(const QString& iArrayName)
{
  double *range =  new double[2];

  range[0] = std::numeric_limits< double >::max();
  range[1] = std::numeric_limits< double >::min();

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().begin();

  while ( it != m_Container.get< TraceID >().end() )
    {
    // does the division have a polydata
    if ( !it->IsLeaf() )
      {
      // convert qstring to const char*
      double *realTime =
        it->TreeNode.Nodes->GetPointData()->GetArray(
            iArrayName.toLocal8Bit().data())->GetRange();
      range[0] = std::min(range[0], realTime[0]);
      range[1] = std::max(range[1], realTime[1]);

      //set active scalar
      // convert qstring to const char*
      it->TreeNode.Nodes->GetPointData()->SetActiveScalars(
            iArrayName.toLocal8Bit().data());
      }

    ++it;
    }

  return range;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
setTimeInterval(const int& iTimeInterval)
{
  m_TimeInterval = iTimeInterval;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
TrackContainer::
getTimeInterval()
{
  return m_TimeInterval;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateTracksRepresentation(const double& iRadius, const double& iRadius2)
{
  MultiIndexContainerType::iterator it = m_Container.begin();

  while ( it != m_Container.end() )
    {
    // restore original polydata
    bool pointsInPolydata = UpdateTrackStructurePolyData( ( *it ) );

    // add glyphs if necessary
    if ( ( iRadius > 0 || iRadius2 > 0 ) && pointsInPolydata )
      {
      it->UpdateTracksRepresentation(iRadius, iRadius2);
      }
    ++it;
    }

  // update color since active scalar is set to NULL in
  // UpdateTrackStructurePolyData
  ChangeColorCode(m_ActiveTrackScalars);

  assert ( this->m_ImageView );

  this->m_ImageView->UpdateRenderWindows();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double*
TrackContainer::GetBorderOfTheTrack( const unsigned int& iTrackID,
                                     const BorderType& iBorder)
{
  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iTrackID);

  if( it != m_Container.get< TraceID >().end() )
    {
    vtkPoints* points = it->Nodes->GetPoints();
    if(!iBorder)
      {
      return points->GetPoint(0);
      }
    else
      {
      vtkIdType nbOfPoints = points->GetNumberOfPoints();
      return points->GetPoint(nbOfPoints-1);
      }
    }

  return NULL;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetListOfDivisions( std::list<unsigned int>& iListOfDivisions)
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
AddDivision( const unsigned int& iMotherID, const unsigned int& iDaughter1ID,
    const unsigned int& iDaughter2ID, const bool& iVisible)
{
  // get address of the structures of interest
  //------------------------------
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMotherID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    MultiIndexContainerTraceIDIterator daughter1It
        = m_Container.get< TraceID >().find(iDaughter1ID);

    if( daughter1It != m_Container.get< TraceID >().end() )
      {
      MultiIndexContainerTraceIDIterator daughter2It
          = m_Container.get< TraceID >().find(iDaughter2ID);

      if( daughter2It != m_Container.get< TraceID >().end() )
        {
        // create connections
        //------------------------------
        // motherID->D1
        //         ->D2
        //------------------------------
        TrackStructure* mother = const_cast<TrackStructure*>(&(*motherIt));
        mother->TreeNode.m_Child.push_back(
            const_cast<TrackStructure*>(&(*daughter1It)));
        mother->TreeNode.m_Child.push_back(
            const_cast<TrackStructure*>(&(*daughter2It)));

        // Create Polydata
        CreateDivisionPolydata(iMotherID);

        // Create Actor
        /*
         \todo Nicolas - should do it through a modify
         */
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
        /*
         \todo Nicolas - should do it through a modify
         */
        TrackStructure* d1 =  const_cast<TrackStructure*>(&(*daughter1It));
        d1->TreeNode.m_Mother = const_cast<TrackStructure*>(&(*motherIt));

        TrackStructure* d2 =  const_cast<TrackStructure*>(&(*daughter2It));
        d2->TreeNode.m_Mother = const_cast<TrackStructure*>(&(*motherIt));

        this->m_ImageView->UpdateRenderWindows();
        }
      }
    }
}
//-------------------------------------------------------------------------
void
TrackContainer::
CreateDivisionPolydata(const unsigned int& iMother)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMother);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
  // Arnaud: what about if any of the parameter is NULL?
  // Arnaud: what about if one daughter is in the field of view,
  // and not the other one?
  // Nicolas: dont deal with any of that yet

  // Get points of interest:
  // Mother: last point
  // D1 & D2: first point
  double* mother    = this->GetBorderOfTheTrack(iMother, LAST);
  double* daughter1 = this->GetBorderOfTheTrack(
        motherIt->TreeNode.m_Child[0]->TraceID, FIRST);
  double* daughter2 = this->GetBorderOfTheTrack(
        motherIt->TreeNode.m_Child[1]->TraceID, FIRST);

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

  m_Container.get< TraceID >().modify( motherIt , create_node_division(division) );
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkActor* >
TrackContainer::
CreateDivisionActor(vtkPolyData* iPolyData, const bool& iVisible)
{
  /*
   * \todo Nicolas: to be enhanced
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

  /*
    \todo Nicolas- clear all that. Should be requiered or should be consistent at least
    */
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
HighlightCollection(const unsigned int& iRootTrackID, const bool& iHilighted)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iRootTrackID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    UpdateCollectionHighlighted(motherIt, iHilighted);

    this->m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
ShowCollection(const unsigned int& iRootTrackID, const bool& iVisible)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iRootTrackID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    UpdateCollectionVisibility(motherIt, iVisible);

    this->m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionVisibility(MultiIndexContainerTraceIDIterator& it,
                           const bool& iVisible)
{
  if( !it->IsLeaf() )
    {
    ModifyDivisionVisibility(it, iVisible);
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);

    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateCollectionVisibility(childIt,iVisible);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
TrackContainer::
DeleteCollection(unsigned int iRootTrackID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iRootTrackID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    UpdateCollectionDelete(motherIt);

    this->m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionDelete(MultiIndexContainerTraceIDIterator& it)
{
  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);

    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateCollectionDelete(childIt);
      }
    ++itDivision;
    }

  if( !it->IsLeaf() )
    {
    DeleteADivision(it->TraceID);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionHighlighted(MultiIndexContainerTraceIDIterator& it,
                            const bool& iHighlighted)
{
  if( !it->IsLeaf() )
    {
    ModifyDivisionHighlight(it, iHighlighted);
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);

    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateCollectionHighlighted(childIt,iHighlighted);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
TrackContainer::
ModifyDivisionVisibility(MultiIndexContainerTraceIDIterator& it,
                         const bool& iVisible)
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
TrackContainer::
ModifyDivisionHighlight(MultiIndexContainerTraceIDIterator& it,
                        const bool& iHighlight )
{
  vtkProperty* temp_property = vtkProperty::New();
  if (!iHighlight)
    {
    temp_property->SetColor(it->TreeNode.rgba[0],
                            it->TreeNode.rgba[1],
                            it->TreeNode.rgba[2]);
    temp_property->SetOpacity(it->TreeNode.rgba[3]);
    temp_property->SetLineWidth(this->m_IntersectionLineWidth);
    }
  else
    {
    temp_property->DeepCopy(this->m_HighlightedProperty);
    }

  m_Container.get< TraceID >().modify(
        it, change_highlighted_division(temp_property, iHighlight));

  temp_property->Delete();

  return 1;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
DeleteADivision( const unsigned int& iMotherID)
{
  // find iterator
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iMotherID);

  if(motherIt!= m_Container.get< TraceID >().end())
    {
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
    /*
     \todo Nicolas - do a loop
     */
    mother->TreeNode.m_Child[0]->TreeNode.m_Mother = NULL;
    mother->TreeNode.m_Child[1]->TreeNode.m_Mother = NULL;

    // mother
    mother->TreeNode.m_Child.resize(0);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int>
TrackContainer::
GetSubLineage( const unsigned int& iTrackID )
{
  std::list<unsigned int> listOfIDs;

  // find the iterator
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    UpdateSubLineage(motherIt, listOfIDs);
    }

  return listOfIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateSubLineage(MultiIndexContainerTraceIDIterator it,
                 std::list<unsigned int>& iList)
{
  iList.push_back( it->TraceID );

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateSubLineage(childIt,iList);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionHighlighting(const unsigned int& iTraceId)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTraceId);

  if(motherIt!=m_Container.get<TraceID>().end())
    {
    GetRootIterator( motherIt );

    bool highlight = !motherIt->TreeNode.Highlighted;

    UpdateCollectionHighlighted(motherIt, highlight);

    this->m_ImageView->UpdateRenderWindows();

    // send signal to lineage container
    emit UpdateLineageHighlightingFromTrackRootID( motherIt->TraceID );
    }
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
GoFigureLineageAttributes 
TrackContainer::
UpdateDivisionsForALineage(unsigned int iTrackIDRoot, double* color)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackIDRoot);
  GoFigureLineageAttributes Attributes;
  if(motherIt!=m_Container.get<TraceID>().end())
    {
    UpdateDivisionScalar(motherIt, 0); // 0=depth of the root
    Attributes = this->GetLineageAttributes(iTrackIDRoot);
    UpdateDivisionColor(motherIt, color);
    m_ImageView->UpdateRenderWindows();
    }

  return Attributes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureLineageAttributes
TrackContainer::
UpdateCollectionScalars(const unsigned int& iTrackID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);
  GoFigureLineageAttributes oAttributes;
  if( motherIt != m_Container.get< TraceID >().end() )
    {
    UpdateDivisionScalar(motherIt, 0); 
    oAttributes = this->GetLineageAttributes(iTrackID);
      // 0=depth of the root
    }
  return oAttributes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionScalar(MultiIndexContainerTraceIDIterator& it,
                     const unsigned int& iDepth)
{
  if(!it->IsLeaf())
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

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      unsigned int depth = iDepth+1;
      UpdateDivisionScalar(childIt,depth);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionColors(const unsigned int& iTrackID, const double* color)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  if(motherIt != m_Container.get< TraceID >().end())
    {
    UpdateDivisionColor(motherIt, color);

    m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionColor(MultiIndexContainerTraceIDIterator& it,
                    const double* iColor)
{
  if(!it->IsLeaf())
    {
    m_Container.get< TraceID >().modify( it , change_data_color_division(iColor) );
    m_Container.get< TraceID >().modify( it , change_actor_color_division(iColor) );
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateDivisionColor(childIt,iColor);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionColorsData(const unsigned int& iTrackID, const double* color)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    UpdateDivisionColorData(motherIt, color);

    m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionColorData(MultiIndexContainerTraceIDIterator& it,
                        const double* iColor)
{
  if(!it->IsLeaf())
    {
    m_Container.get< TraceID >().modify( it , change_data_color_division(iColor) );
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateDivisionColorData(childIt,iColor);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int
TrackContainer::
GetCollectionMaxDepth(const unsigned int& iTrackRootID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackRootID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    unsigned int depth = 0;
    UpdateCollectionMaxDepth(motherIt, 0, depth); //0: root depth

    return depth;
    }
  else
    {
    return 0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionMaxDepth(MultiIndexContainerTraceIDIterator& it,
                         const unsigned int& iDivisionDepth,
                         unsigned int& iLineageDepth)
{
  if( it->IsLeaf() )
    {
    if( iDivisionDepth > iLineageDepth )
      {
      iLineageDepth = iDivisionDepth;
      }
    }
  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      unsigned int depth = iDivisionDepth+1;
      UpdateCollectionMaxDepth(childIt,depth,iLineageDepth);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int
TrackContainer::
GetCollectionMinDepth(const unsigned int& iTrackRootID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackRootID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    unsigned int depth = std::numeric_limits<unsigned int>::max();
    UpdateCollectionMinDepth(motherIt, 0, depth); //0: root depth

    return depth;
    }
  else
    {
    return 0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionMinDepth(MultiIndexContainerTraceIDIterator& it,
                         const unsigned int& iDivisionDepth,
                         unsigned int& iLineageDepth)
{
  if( it->IsLeaf() )
    {
    if( iDivisionDepth < iLineageDepth )
      {
      iLineageDepth = iDivisionDepth;
      }
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      unsigned int depth = iDivisionDepth+1;
      UpdateCollectionMinDepth(childIt,depth,iLineageDepth);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int
TrackContainer::
GetCollectionNumberOfDivisions(const unsigned int& iTrackRootID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackRootID);

  if( motherIt != m_Container.get< TraceID >().end() )
    {
    unsigned int numberOfDivisions = 0;
    UpdateCollectionNumberOfDivisions(motherIt,numberOfDivisions);

    return numberOfDivisions;
    }
  else
    {
    return 0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionNumberOfDivisions(MultiIndexContainerTraceIDIterator& it,
                                  unsigned int& iNumberOfDivisions)
{
  if(!it->IsLeaf())
    {
    ++iNumberOfDivisions;
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateCollectionNumberOfDivisions(childIt,iNumberOfDivisions);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int
TrackContainer::
GetCollectionNumberOfLeaves(const unsigned int& iTrackRootID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackRootID);

  if(motherIt != m_Container.get< TraceID >().end())
    {
    unsigned int numberOfLeaves = 1;
    UpdateCollectionNumberOfDivisions(motherIt,numberOfLeaves);

    return numberOfLeaves;
    }
  else
    {
    return 0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureLineageAttributes
TrackContainer::
GetLineageAttributes(unsigned int iTrackRootID)
{
  //check if possible to iterate on the tree only once
  GoFigureLineageAttributes LineageAttributes;
  
  LineageAttributes.MaxDepth = this->GetCollectionMaxDepth(iTrackRootID);
  LineageAttributes.MinDepth = this->GetCollectionMinDepth(iTrackRootID);
  LineageAttributes.NumberOfDivisions = this->GetCollectionNumberOfDivisions(iTrackRootID);
  LineageAttributes.NumberOfLeaves = this->GetCollectionNumberOfLeaves( iTrackRootID);

  return LineageAttributes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateCollectionNumberOfLeaves(MultiIndexContainerTraceIDIterator& it,
                               unsigned int& iNumberOfLeaves)
{
  if(it->IsLeaf())
    {
    ++iNumberOfLeaves;
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateCollectionNumberOfLeaves(childIt,iNumberOfLeaves);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkMutableDirectedGraph*
TrackContainer::
ExportLineage(const unsigned int& iTrackID)
{
  MultiIndexContainerTraceIDIterator motherIt
      = m_Container.get< TraceID >().find(iTrackID);

  // graph to be exported
  vtkMutableDirectedGraph* graph = vtkMutableDirectedGraph::New();
  unsigned int pedigree = graph->AddVertex();

  // arrays we want to export
  vtkDoubleArray* id = vtkDoubleArray::New();
  id->SetName("Track ID");

  // arrays we want to export
  vtkDoubleArray* depth = vtkDoubleArray::New();
  depth->SetName("Lineage Depth");

  vtkStringArray* cellType = vtkStringArray::New();
  depth->SetName("Cell Type");

  UpdateLineage(motherIt,
                   graph,
                pedigree,
                       0,  // mother vtkIDtype
                       0,depth,// depth information
                       id); // id info

  graph->GetVertexData()->AddArray(id);
  graph->GetVertexData()->AddArray(depth);

  // delete array
  id->Delete();
  depth->Delete();

  return graph;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateLineage(MultiIndexContainerTraceIDIterator& it,
              vtkMutableDirectedGraph* iGraph, unsigned int iPedrigree,
              vtkIdType mother, unsigned int iDepth,
              vtkDoubleArray* iDepthArray, vtkDoubleArray* iIDArray)
{
  // Update mother ID
  vtkIdType motherPedigree = iPedrigree;

  //add info
  iIDArray->InsertValue(iPedrigree, it->TraceID);
  // add info
  iDepthArray->InsertValue(iPedrigree, iDepth);

  if( it->IsLeaf() )
    {
    return;
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while(itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    // add edge
    iPedrigree = iGraph->AddChild(motherPedigree);
    //go through tree
    UpdateLineage(childIt,iGraph, iPedrigree, motherPedigree, iDepth+1, iDepthArray,iIDArray);
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetCollectionColorCode(const std::string& iColumnName,
                       const std::map< unsigned int, std::string >& iValues)
{
  typedef  std::map< unsigned int, std::string > MapType;
  typedef  MapType::const_iterator               MapConstIterator;

  std::map< std::string, double > stringmap;

  if ( iColumnName.empty() || iValues.empty() )
    {
    this->RenderAllDivisionsWithOriginalColors();
    assert ( m_ImageView );
    this->m_ImageView->UpdateRenderWindows();
    return;
    }

  MapConstIterator it = iValues.begin();

  double temp = 0.;
  try
    {
    temp = boost::lexical_cast< double >(it->second);
    }
  catch(boost::bad_lexical_cast &)
    {
    if ( stringmap.empty() )
      {
      stringmap[it->second] = 0.;
      }
    else
      {
      std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

      if ( m_it != stringmap.end() )
        {
        temp = m_it->second;
        }
      else
        {
        std::map< std::string, double >::reverse_iterator r_it = stringmap.rbegin();
        temp = r_it->second;
        temp += 1.0;
        }
      }
    }

  double min_value = temp;
  double max_value = temp;

  while ( it != iValues.end() )
    {

    MultiIndexContainerTraceIDIterator
      trace_it = this->m_Container.get< TraceID >().find(it->first);

    // convert value
    // Here let's make sure you are not passing crazy values!
    try
      {
      temp = boost::lexical_cast< double >(it->second);
      }
    catch(boost::bad_lexical_cast &)
      {
      // stringmap is not empty and has at least one elements
      std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

      if ( m_it != stringmap.end() )
        {
        temp = m_it->second;
        }
      else
        {
        std::map< std::string, double >::reverse_iterator r_it = stringmap.rbegin();
        temp = r_it->second;
        temp += 1.0;
        }
      }

    UpdateDivisionScalarData(trace_it, it->second, temp, min_value, max_value);
    ++it;
    }

  SetScalarRangeForAllDivisions(min_value, max_value);

  assert ( m_ImageView );
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateDivisionScalarData(MultiIndexContainerTraceIDIterator& it,
                         const std::string& iColumnName, const double& iValue,
                         double& iMin, double& iMax)
{

  if( !it->IsLeaf() )
    {
    if ( iValue > iMax )
      {
      iMax = iValue;
      }
    if ( iValue < iMin )
      {
      iMin = iValue;
      }

    it->TreeNode.SetScalarData(iColumnName, iValue);
    }

  std::vector<TrackStructure*>::const_iterator itDivision =
      it->TreeNode.m_Child.begin();

  while( itDivision != it->TreeNode.m_Child.end())
    {
    // find the iterator
    MultiIndexContainerTraceIDIterator childIt
        = m_Container.get< TraceID >().find((*itDivision)->TraceID);
    if( childIt != m_Container.get< TraceID >().end() )
      {
      UpdateDivisionScalarData(childIt, iColumnName, iValue, iMin, iMax);
      }
    ++itDivision;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetDivisionRandomColor(const std::string & iColumnName,
                       const std::map< unsigned int, std::string > & iValues)
{
  typedef std::map< unsigned int, std::string > MapType;
  typedef MapType::const_iterator               MapConstIterator;

  std::map< std::string, double > stringmap;

  if ( iColumnName.empty() || iValues.empty() )
    {
    this->RenderAllDivisionsWithOriginalColors();
    return;
    }

  MapConstIterator it = iValues.begin();

  double temp = 0.;
  try
    {
    temp = boost::lexical_cast< double >(it->second);
    }
  catch(boost::bad_lexical_cast &)
    {
    if ( stringmap.empty() )
      {
      stringmap[it->second] = 0.;
      }
    else
      {
      std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

      if ( m_it != stringmap.end() )
        {
        temp = m_it->second;
        }
      else
        {
        std::map< std::string, double >::reverse_iterator r_it = stringmap.rbegin();
        temp = r_it->second;
        temp += 1.0;
        }
      }
    }

  unsigned int val = static_cast< unsigned int >(temp);
  unsigned int modulo = val % 30;

  temp = static_cast< double >(modulo);

  double min_value = temp;
  double max_value = temp;

  using boost::multi_index:: get;

  while ( it != iValues.end() )
    {
    MultiIndexContainerTraceIDIterator
      trace_it = this->m_Container.get< TraceID >().find(it->first);

    if ( trace_it != this->m_Container.get< TraceID >().end() )
      {
      if ( trace_it->TreeNode.Nodes ) //make sure the trace has points !!!
        {
        // Here let's make sure you are not passing crazy values!
        try
          {
          temp = boost::lexical_cast< double >(it->second);
          }
        catch(boost::bad_lexical_cast &)
          {
          // stringmap is not empty and has at least one element
          std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

          if ( m_it != stringmap.end() )
            {
            temp = m_it->second;
            }
          else
            {
            std::map< std::string, double >::reverse_iterator r_it = stringmap.rbegin();
            temp = r_it->second;
            temp += 1.0;
            }
          }

        val = static_cast< unsigned int >(temp);
        modulo = val % 30;

        temp = static_cast< double >(modulo);

        UpdateDivisionScalarData(trace_it, it->second, temp, min_value, max_value);
        }
      } //end make sure the trace has points !!!
    ++it;
    }

  SetScalarRangeForAllDivisions(min_value, max_value);

  assert ( m_ImageView );
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
UpdateAverageVolume(const unsigned int& iTrackID, const double& iVolume)
{
  MultiIndexContainerTraceIDIterator
    trace_it = this->m_Container.get< TraceID >().find(iTrackID);

  if ( trace_it != m_Container.get< TraceID >().end() )
    {
    m_Container.get< TraceID >().modify( trace_it , modify_avg_volume(iVolume) );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
AddVolume(const unsigned int& iTrackID, const double& iVolume)
{
  MultiIndexContainerTraceIDIterator
    trace_it = this->m_Container.get< TraceID >().find(iTrackID);

  if ( trace_it != m_Container.get< TraceID >().end() )
    {
    m_Container.get< TraceID >().modify( trace_it , add_avg_volume(iVolume) );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
AverageVolume()
{
  MultiIndexContainerTraceIDIterator
    trace_it = this->m_Container.get< TraceID >().begin();

  while ( trace_it != m_Container.get< TraceID >().end() )
    {
    m_Container.get< TraceID >().modify( trace_it , compute_avg_volume() );
    ++trace_it;
    }
}
