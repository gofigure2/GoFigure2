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

#include "TrackStructure.h"

#include <iostream>
#include "vtkPolyData.h"
#include "vtkActor.h"

#include "vtkDoubleArray.h"
#include "vtkPointData.h"

#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkMath.h"

//--------------------------------------------------------------------------
TrackStructure::
TrackStructure():TraceStructure()
{
}

//--------------------------------------------------------------------------
/*
//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           vtkPolyData *iNodes,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha):
  TraceID(iTraceID), Nodes(iNodes), Highlighted(iHighlighted), Visible(iVisible)
{
  if ( iActors.size() == 4 )
    {
    ActorXY = iActors[0];
    ActorXZ = iActors[1];
    ActorYZ = iActors[2];
    ActorXYZ = iActors[3];
    }
  else
    {
    std::cerr << "iActors.size() != 4" << std::endl;
    return;
    }
  this->rgba[0] = r;
  this->rgba[1] = g;
  this->rgba[2] = b;
  this->rgba[3] = alpha;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           vtkPolyData *iNodes,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           double iRgba[4]):
  TraceID(iTraceID), Nodes(iNodes), Highlighted(iHighlighted), Visible(iVisible)
{
  if ( iActors.size() == 4 )
    {
    ActorXY = iActors[0];
    ActorXZ = iActors[1];
    ActorYZ = iActors[2];
    ActorXYZ = iActors[3];
    }
  else
    {
    std::cout << "iActors.size() != 4" << std::endl;
    return;
    }
  this->rgba[0] = iRgba[0];
  this->rgba[1] = iRgba[1];
  this->rgba[2] = iRgba[2];
  this->rgba[3] = iRgba[3];
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const unsigned int & iTraceID,
                                           vtkActor *iActorXY,
                                           vtkActor *iActorYZ,
                                           vtkActor *iActorXZ,
                                           vtkActor *iActorXYZ,
                                           vtkPolyData *iNodes,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha):
  TraceID(iTraceID), ActorXY(iActorXY), ActorXZ(iActorXZ),
  ActorYZ(iActorYZ), ActorXYZ(iActorXYZ), Nodes(iNodes),
  Highlighted(iHighlighted), Visible(iVisible)
{
  this->rgba[0] = r;
  this->rgba[1] = g;
  this->rgba[2] = b;
  this->rgba[3] = alpha;
}

//--------------------------------------------------------------------------
*/
//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const TrackStructure & iE):
  TraceStructure( iE ), PointsMap(iE.PointsMap)
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::
~TrackStructure()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
TrackStructure::
InsertElement(const unsigned int& iTime, double* iPoint)
{
  // check if there is something at the iTime time point
  PointsMapIterator pointsMapIterator = this->PointsMap.find(iTime);

  // if there is no point, insert it and return true
  if ( pointsMapIterator == this->PointsMap.end() )
    {
    this->PointsMap.insert(
          std::pair< unsigned int, double* >( iTime, iPoint ) );
    return true;
    }

  // else do nothing and return false
  std::cout << "in: " << __FILE__ << " at line: " << __LINE__ << std::endl;
  std::cout << "can't insert a point at this time point" << std::endl;
  return false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
TrackStructure::
DeleteElement(const unsigned int& iTime)
{
  // check if there is something at the iTime time point
  PointsMapIterator pointsMapIterator = this->PointsMap.find(iTime);

  // if there is a point, delete it and return true
  if ( pointsMapIterator != this->PointsMap.end() )
    {
    // free memory
    delete[] pointsMapIterator->second;
    // clear map
    this->PointsMap.erase(pointsMapIterator);
    return true;
    }

  // else do nothing and return false
  std::cout << "in: " << __FILE__ << " at line: " << __LINE__ << std::endl;
  std::cout << "can't delete a point at this time point" << std::endl;
  return false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
TrackStructure::
ReplaceElement(const unsigned int& iTime, double* iPoint)
{
  // delete the existing element
  bool deleteElement = DeleteElement(iTime);

  // if sth has been deleted, insert the point and return true
  if(deleteElement)
    {
    return InsertElement(iTime,iPoint);
    }

  // else do nothing and return false
  std::cout << "in: " << __FILE__ << " at line: " << __LINE__ << std::endl;
  std::cout << "can't replace a point at this time point" << std::endl;
  return deleteElement;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
TrackStructure::ReleaseData() const
{
  TraceStructure::ReleaseData();

  PointsMapConstIterator begin = this->PointsMap.begin();
  PointsMapConstIterator end = this->PointsMap.end();

  while( begin != end )
    {
    delete[] begin->second;
    ++begin;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
TrackStructure::
UpdateTracksRepresentation( bool iGlyph, bool iTube ) const
{
  vtkPolyData* glyph_pd = NULL;
  vtkSphereSource* sphere = NULL;
  vtkGlyph3D* glyph = NULL;

  if( iGlyph )
    {
    // Glyph shape
    sphere = vtkSphereSource::New();
    sphere->SetThetaResolution( 8 );
    sphere->SetPhiResolution( 8 );

    glyph = vtkGlyph3D::New();
    glyph->SetInput( this->Nodes );
    glyph->SetSource( sphere->GetOutput() );
    glyph->Update();

    glyph_pd = glyph->GetOutput();
    }

  vtkPolyData* tube_pd = NULL;
  vtkTubeFilter* tube = NULL;

  if( iTube )
    {
    tube = vtkTubeFilter::New();
    tube->SetNumberOfSides( 8 );
    tube->SetInput( this->Nodes );
    tube->SetRadius( .2  );
    tube->Update();

    tube_pd = tube->GetOutput();
    }
  else
    {
    tube_pd = this->Nodes;
    }

  vtkPolyData* temp = NULL;
  vtkAppendPolyData* apd = NULL;

  if( glyph_pd && tube_pd )
    {
    // append both polydata sets
    apd = vtkAppendPolyData::New();;
    apd->AddInput( glyph_pd );
    apd->AddInput( tube_pd );
    apd->Update();

    temp = apd->GetOutput();
    }
  else
    {
    if( glyph_pd )
      {
      temp = glyph_pd;
      }
    else
      {
      if( tube_pd )
        {
        temp = tube_pd;
        }
      else
        {
        temp = this->Nodes;
        }
      }
    }

  this->Nodes->DeepCopy( temp );

  if( sphere )
    {
    sphere->Delete();
    }
  if( glyph )
    {
    glyph->Delete();
    }
  if( tube )
    {
    tube->Delete();
    }
  if( apd )
    {
    apd->Delete();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*bool TrackMerge( const TrackStructure& iT1,
                 const TrackStructure& iT2,
                 TrackStructure& oMerged )
{
  unsigned int t_min1 = iT1.PointsMap.begin()->first;
  unsigned int t_max1 = iT1.PointsMap.rbegin()->first;

  unsigned int t_min2 = iT2.PointsMap.begin()->first;
  unsigned int t_max2 = iT2.PointsMap.rbegin()->first;

  bool min1 = ( t_min1 <= t_min2 );
  bool min2 = ( t_min2 <= t_min1 );

  bool max1 = ( t_max2 <= t_max1 );
  bool max2 = ( t_max1 <= t_max2 );

  if( ( min1 && max1 ) || ( min2 && max2 ) )
    {
    std::cout << "one track is totally included into the other one" <<std::endl;
    return false;
    }
  else
    {
    if( ( ( min1 && max2 ) && ( t_min2 < t_max1 ) ) ||
        ( ( min2 && max1 ) && ( t_min1 < t_max2 ) ) )
      {
      std::cout << "these two tracks overlap" << std::endl;
      return false;
      }
    else
      {
      std::cout << "optimal case" <<std::endl;

      TrackStructure::PointsMapConstIterator p_start, p_end;
      if( min1 )
        {
        oMerged = iT2;

        p_start = iT2.PointsMap.begin();
        p_end = iT2.PointsMap.end();
        }
      else
        {
        oMerged = iT2;

        p_start = iT1.PointsMap.begin();
        p_end = iT1.PointsMap.end();
        }

      // here the code can be optimized!!!
      while( p_start != p_end )
        {
        oMerged.PointsMap[p_start->first] = p_start->second;
        ++p_start;
        }
      return true;
      }
    }
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*bool TrackSplit( const TrackStructure& iTrack,
                 const unsigned int& iTime,
                 TrackStructure& oT1,
                 TrackStructure& oT2 )
{
  TrackStructure::PointsMapConstIterator
      it = iTrack.PointsMap.lower_bound( iTime );

  if( it != iTrack.PointsMap.end() )
    {
    TrackStructure::PointsMapConstIterator begin = iTrack.PointsMap.begin();

    oT1 = iTrack;

    oT2.Highlighted = iTrack.Highlighted;
    oT2.Visible = iTrack.Visible;
    oT2.rgba[0] = iTrack.rgba[0];
    oT2.rgba[1] = iTrack.rgba[1];
    oT2.rgba[2] = iTrack.rgba[2];
    oT2.rgba[3] = iTrack.rgba[3];

    while( begin != it )
      {
      oT1.PointsMap[ begin->first ] = begin->second;
      ++begin;
      }
    while( begin != iTrack.PointsMap.end() )
      {
      oT2.PointsMap[ begin->first ] = begin->second;
      ++begin;
      }

    return true;
    }

  return false;
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void TrackStructure::ComputeAttributes()
{
  PointsMapConstIterator it = this->PointsMap.begin();
  double total_length = 0.;
  double distance = 0.;
  double avg_speed = 0.;
  double max_speed = 0.;
  unsigned int t0, t1;
  double dist;
  double theta;
  double phi;

  unsigned int tmin = it->first;
  t0 = tmin;
  double* org = it->second;
  double* p = it->second;
  double* q = NULL;
  ++it;

  // shouldnt create a new array
  vtkDoubleArray* newArray = vtkDoubleArray::New();
  newArray->SetNumberOfComponents(1);
  newArray->SetName("SpeedInformation");
  newArray->InsertNextValue(0.0);

  while( it != this->PointsMap.end() )
    {
    t1 = it->first;
    q = it->second;
    dist = sqrt( vtkMath::Distance2BetweenPoints( p, q ) );
    total_length += dist;
    max_speed = std::max( max_speed,
                          dist / (static_cast< double >( t1 - t0 ) ) );

    double speed = dist / (static_cast< double >( t1 - t0 ) );
    newArray->InsertNextValue( speed );
    std::cout << "speed: " << speed << std::endl;

    p = q;
    t0 = t1;
    ++it;
    }

  distance = sqrt( vtkMath::Distance2BetweenPoints( org, q ) );
  avg_speed = total_length / static_cast< double >( t1 - tmin );

  //angles
  theta = vtkMath::DegreesFromRadians( atan2( ( q[1] - org[1] ), ( q[0] - org[0] ) ) );
  phi   = vtkMath::DegreesFromRadians( acos( ( q[2] - org[2] ) / distance ) );

  std::cout << "r, theta, phi: ("
            << std::setprecision(3)
            << distance << ", " << theta << ", " << phi << ") "
            << std::endl;

  this->Nodes->GetPointData()->AddArray(newArray);
  newArray->Delete();
}

//--------------------------------------------------------------------------
