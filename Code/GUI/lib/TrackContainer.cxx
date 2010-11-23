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
#include "vtkFieldData.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"


//////////
#include "VisualizePolydataHelper.h"
//////////
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
    this->m_CurrentElement = *it;
    //Don't have to delete since we keep the same addresses for all the variables
    //this->DeleteElement(it);
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

    if ( it->ActorXY )
      {
      it->ActorXY->SetVisibility(!it->Visible);
      }
    if ( it->ActorXZ )
      {
      it->ActorXZ->SetVisibility(!it->Visible);
      }
    if ( it->ActorYZ )
      {
      it->ActorYZ->SetVisibility(!it->Visible);
      }
    if ( it->ActorXYZ )
      {
      it->ActorXYZ->SetVisibility(!it->Visible);
      }

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
  /*if ( it != m_Container.get< TraceID >().end() )
    {
    if ( it->ActorXY )
      {
      this->m_ImageView->RemoveActor(0, it->ActorXY);
      it->ActorXY->Delete();
      }
    if ( it->ActorXZ )
      {
      this->m_ImageView->RemoveActor(1, it->ActorXZ);
      it->ActorXZ->Delete();
      }
    if ( it->ActorYZ )
      {
      this->m_ImageView->RemoveActor(2, it->ActorYZ);
      it->ActorYZ->Delete();
      }
    if ( it->ActorXYZ )
      {
      this->m_ImageView->RemoveActor(3, it->ActorXYZ);
      it->ActorXYZ->Delete();
      }

    if ( it->Nodes )
      {
      it->Nodes->Delete();
      }

    m_Container.get< TraceID >().erase(it);

    m_ImageView->UpdateRenderWindows();
    return true;
    }
  return false;*/
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
      if ( it0->ActorXY )
        {
        it0->ActorXY->SetProperty(this->m_HighlightedProperty);
        }
      if ( it0->ActorXZ )
        {
        it0->ActorXZ->SetProperty(this->m_HighlightedProperty);
        }
      if ( it0->ActorYZ )
        {
        it0->ActorYZ->SetProperty(this->m_HighlightedProperty);
        }
      if ( it0->ActorXYZ )
        {
        it0->ActorXYZ->SetProperty(this->m_HighlightedProperty);
        }
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
void
TrackContainer::
AddPointToCurrentElement(double* iPoint)
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

    newArray->InsertValue( 0, iPoint[3] );
    newPoints->InsertPoint( 0, iPoint );

    //add the points to the dataset
    this->m_CurrentElement.Nodes->SetPoints(newPoints);

    //add the temporal information
    this->m_CurrentElement.Nodes->GetFieldData()->AddArray(newArray);

   // ShowPolyData(this->m_CurrentElement.Nodes);

    delete iPoint;

    emit CurrentTrackToSave();

    return;
    }
  else
    {
    if(this->m_CurrentElement.ActorXY)
      {
      // remove and delete the actors from th visu
      this->m_ImageView->RemoveActor(0, this->m_CurrentElement.ActorXY);
      this->m_CurrentElement.ActorXY->Delete();
      this->m_ImageView->RemoveActor(1, this->m_CurrentElement.ActorXZ);
      this->m_CurrentElement.ActorXZ->Delete();
      this->m_ImageView->RemoveActor(2, this->m_CurrentElement.ActorYZ);
      this->m_CurrentElement.ActorYZ->Delete();
      this->m_ImageView->RemoveActor(3, this->m_CurrentElement.ActorXYZ);
      this->m_CurrentElement.ActorXYZ->Delete();
      }
    }

  vtkIdType N = this->m_CurrentElement.Nodes->GetNumberOfPoints();
  vtkSmartPointer<vtkPoints> points = this->m_CurrentElement.Nodes->GetPoints();

  std::map<int, double*> orderedPoints;
  double* pt = NULL;
  int    time = 0;

  vtkSmartPointer<vtkIntArray> temporalArray =
      vtkIntArray::SafeDownCast(this->m_CurrentElement.Nodes->GetFieldData()
          ->GetArray("TemporalInformation"));

  // fill a map so the points will be ordered automatically
  for ( vtkIdType i = 0; i < N; i++ )
    {
    pt = points->GetPoint(i);
    time = temporalArray->GetValue(i);
    orderedPoints.insert( std::pair<int,double*>(time, pt) );
    }

  // insert the new mesh
  orderedPoints.insert( std::pair<int,double*>(iPoint[3], iPoint) );

  //Reconstruct from the map
  // read map and fill points
  vtkSmartPointer< vtkPoints > newPoints = vtkSmartPointer< vtkPoints >::New();
  vtkSmartPointer<vtkIntArray> newArray = vtkSmartPointer<vtkIntArray>::New();
  newArray->SetNumberOfComponents(1);
  newArray->SetName("TemporalInformation");
  std::map<int, double*>::iterator it = orderedPoints.begin();

  while(it != orderedPoints.end())
    {
    newArray->InsertNextValue( it->first );
    newPoints->InsertNextPoint(it->second);
    ++it;
    }

  // Clean the map
  for (it = orderedPoints.begin(); it != orderedPoints.end(); ++it)
    {
    delete[] it->second;
    }
  orderedPoints.clear();

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
  vtkPolyData* polyData = vtkPolyData::New();

  //add the points to the dataset
  polyData->SetPoints(newPoints);

  //add the lines to the dataset
  polyData->SetLines(cells);

  //add the temporal information
  polyData->GetFieldData()->AddArray(newArray);

  // MOVE TO SMARTPOINTER AND SHALLOW COPY....
  this->m_CurrentElement.Nodes->DeepCopy(polyData);
  polyData->Delete();

  //ShowPolyData(this->m_CurrentElement.Nodes);

  emit CurrentTrackToSave();
}
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
  //need to be reimplemented
  /*if( !iList.empty() )
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
        if ( it->TCoord != m_TCoord )
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
RenderAllElementsWithOriginalColors()
{
  MultiIndexContainerIterator t_it = m_Container.begin();
  while( t_it != m_Container.end() )
    {
    t_it->RenderWithOriginalColors();
    ++t_it;
    }
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetScalarRangeForAllElements( const double& iMin, const double& iMax )
{
  // Let's set the scalar range (in order to get nice colors)
  MultiIndexContainerIterator t_it = m_Container.begin();
  while( t_it != m_Container.end() )
    {
    t_it->SetScalarRange( iMin, iMax );
    ++t_it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetRandomColor( const std::string& iColumnName,
                const std::map< unsigned int, std::string >& iValues )
{
  typedef std::map< unsigned int, std::string > MapType;
  typedef MapType::const_iterator MapConstIterator;

  std::map< std::string, double > stringmap;

  if( iColumnName.empty() || iValues.empty() )
    {
    this->RenderAllElementsWithOriginalColors();
    return;
    }

  MapConstIterator it = iValues.begin();

  double temp = 0.;
  try
    {
    temp = boost::lexical_cast< double >( it->second );
    }
  catch( boost::bad_lexical_cast& )
    {
    if( stringmap.empty() )
      {
      stringmap[ it->second ] = 0.;
      }
    else
      {
      std::map< std::string, double >::iterator m_it = stringmap.find( it->second );

      if( m_it != stringmap.end() )
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

  unsigned int val = static_cast< unsigned int >( temp );
  unsigned int modulo = val % 30;

  temp = static_cast< double >( modulo );

  double min_value = temp;
  double max_value = temp;

  while( it != iValues.end() )
    {
    MultiIndexContainerTraceIDIterator
        trace_it = this->m_Container.get<TraceID>().find( it->first );

    if( trace_it != this->m_Container.get<TraceID>().end() )
      {
        if (trace_it->Nodes) //make sure the trace has points !!!
        {
        // Here let's make sure you are not passing crazy values!
        try
          {
          temp = boost::lexical_cast< double >( it->second );
          }
        catch( boost::bad_lexical_cast& )
          {
          // stringmap is not empty and has at least one element
          std::map< std::string, double >::iterator m_it = stringmap.find( it->second );

          if( m_it != stringmap.end() )
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

        val = static_cast< unsigned int >( temp );
        modulo = val % 30;

        temp = static_cast< double >( modulo );

        if( temp > max_value )
          {
          max_value = temp;
          }
        if( temp < min_value )
          {
          min_value = temp;
          }
        trace_it->SetScalarData( iColumnName, temp );
        }
      } //end make sure the trace has points !!!
    ++it;
    }

  SetScalarRangeForAllElements( min_value, max_value );

  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetLookupTableForColorCoding( vtkLookupTable* iLut )
{
  if( iLut )
    {
    MultiIndexContainerIterator it = m_Container.begin();

    while( it != m_Container.end() )
      {
      it->SetLookupTable( iLut );
      ++it;
      }
    this->m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
TrackContainer::
SetColorCode( const std::string& iColumnName,
              const std::map< unsigned int, std::string >& iValues )
{
  typedef std::map< unsigned int, std::string > MapType;
  typedef MapType::const_iterator MapConstIterator;

  std::map< std::string, double > stringmap;

  if( iColumnName.empty() || iValues.empty() )
    {
    this->RenderAllElementsWithOriginalColors();
    return;
    }

  MapConstIterator it = iValues.begin();

  double temp = 0.;
  try
    {
    temp = boost::lexical_cast< double >( it->second );
    }
  catch( boost::bad_lexical_cast& )
    {
    if( stringmap.empty() )
      {
      stringmap[ it->second ] = 0.;
      }
    else
      {
      std::map< std::string, double >::iterator m_it = stringmap.find( it->second );

      if( m_it != stringmap.end() )
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

  while( it != iValues.end() )
    {
    MultiIndexContainerTraceIDIterator
        trace_it = this->m_Container.get<TraceID>().find( it->first );

    if( trace_it != this->m_Container.get<TraceID>().end() )
      {
        if (trace_it->Nodes) //make sure the trace has points !!!
        {
        // Here let's make sure you are not passing crazy values!
        try
          {
          temp = boost::lexical_cast< double >( it->second );
          }
        catch( boost::bad_lexical_cast& )
          {
          // stringmap is not empty and has at least one element
          std::map< std::string, double >::iterator m_it = stringmap.find( it->second );

          if( m_it != stringmap.end() )
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

        if( temp > max_value )
          {
          max_value = temp;
          }
        if( temp < min_value )
          {
          min_value = temp;
          }
        trace_it->SetScalarData( iColumnName, temp );
        }
      } //end make sure the trace has points !!!
    ++it;
    }

  SetScalarRangeForAllElements( min_value, max_value );

  this->m_ImageView->UpdateRenderWindows();
}
