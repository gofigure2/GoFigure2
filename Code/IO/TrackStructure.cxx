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

#include "vtkSmartPointer.h"

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
  // if there is no point, insert it and return true
  DeleteElement(iTime);
  this->PointsMap.insert(
      std::pair< unsigned int, double* >( iTime, iPoint ) );

  return true;
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
  return false;
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
UpdateTracksRepresentation(double iRadius, double iRadius2) const
{
  vtkSmartPointer<vtkAppendPolyData> apd =
      vtkSmartPointer<vtkAppendPolyData>::New();
  apd->AddInput( this->Nodes );

  if(iRadius)
    {
    vtkSmartPointer<vtkSphereSource> sphere =
        vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetThetaResolution( 8 );
    sphere->SetPhiResolution( 8 );
    sphere->SetRadius( iRadius );

    vtkSmartPointer<vtkGlyph3D> glyph =
        vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInput( this->Nodes );
    glyph->SetSource( sphere->GetOutput() );
    glyph->Update();

    apd->AddInput( glyph->GetOutput() );
    }

  if(iRadius2)
    {
    vtkSmartPointer<vtkTubeFilter> tube =
        vtkSmartPointer<vtkTubeFilter>::New();
    tube->SetNumberOfSides( 8 );
    tube->SetInput( this->Nodes );
    tube->SetRadius( iRadius2 );
    tube->Update();

    apd->AddInput( tube->GetOutput() );
    }

  apd->Update();

  this->Nodes->DeepCopy( apd->GetOutput() );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigureTrackAttributes
TrackStructure::
ComputeAttributes() const
{
  GoFigureTrackAttributes attributes;

  attributes.total_length = 0.;
  attributes.distance = 0.;
  attributes.avg_speed = 0.;
  attributes.max_speed = 0.;
  unsigned int t0, t1;
  attributes.theta = 0.;
  attributes.phi = 0.;

  PointsMapConstIterator it = this->PointsMap.begin();

  // check if there are no points in the map
  if( it == this->PointsMap.end())
    {
    return attributes;
    }

  unsigned int tmin = it->first;
  t0 = tmin;
  t1 = tmin;
  double* org = it->second;
  double* p = it->second;
  double* q = it->second; // if we only have one point in the map
  ++it;

  // reset the array
  vtkDoubleArray* newArray =
      dynamic_cast<vtkDoubleArray*>(this->Nodes->GetPointData()->GetArray("SpeedInformation"));
  newArray->Reset();
  newArray->InsertNextValue(0.0);

  while( it != this->PointsMap.end() )
    {
    t1 = it->first;
    q = it->second;
    attributes.distance = sqrt( vtkMath::Distance2BetweenPoints( p, q ) );
    attributes.total_length += attributes.distance;
    attributes.max_speed = std::max( attributes.max_speed,
        attributes.distance / (static_cast< double >( t1 - t0 ) ) );

    double speed = attributes.distance / (static_cast< double >( t1 - t0 ) );
    newArray->InsertNextValue( speed );

    p = q;
    t0 = t1;
    ++it;
    }

  if (t1 == tmin)
    {
      attributes.avg_speed = 0;
    }
  else
    {
    attributes.avg_speed = attributes.total_length /
                           static_cast< double >( t1 - tmin );
    }

  attributes.distance = sqrt( vtkMath::Distance2BetweenPoints( org, q ) );

  if(attributes.distance)
    {
    attributes.theta = vtkMath::DegreesFromRadians( atan2( ( q[1] - org[1] ),
                                                           ( q[0] - org[0] ) ) );
    attributes.phi   = vtkMath::DegreesFromRadians( acos( ( q[2] - org[2] ) /
                                                         attributes.distance ) );
    }

  return attributes;
}

//--------------------------------------------------------------------------
