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

#include "ContourMeshStructure.h"

#include <iostream>
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkMapper.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkLookupTable.h"

//--------------------------------------------------------------------------
ContourMeshStructure::ContourMeshStructure():TraceStructure(),
  TCoord(0)
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
ContourMeshStructure::ContourMeshStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           vtkPolyData *iNodes,
                                           const unsigned int & iT,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha) :
  TraceStructure( iTraceID, iActors, iNodes, iHighlighted, iVisible, r, g, b, alpha ),
  TCoord(iT)
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
ContourMeshStructure::ContourMeshStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           vtkPolyData *iNodes,
                                           const unsigned int & iT,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           double iRgba[4]):
  TraceStructure(iTraceID, iActors, iNodes, iHighlighted, iVisible, iRgba ),
  TCoord(iT)
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
ContourMeshStructure::ContourMeshStructure(const unsigned int & iTraceID,
                                           vtkActor *iActorXY,
                                           vtkActor *iActorYZ,
                                           vtkActor *iActorXZ,
                                           vtkActor *iActorXYZ,
                                           vtkPolyData *iNodes,
                                           const unsigned int & iT,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha):
  TraceStructure( iTraceID, iActorXY, iActorYZ, iActorXZ, iActorXYZ, iNodes,
                  iHighlighted, iVisible, r, g, b, alpha ),
  TCoord(iT)
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
ContourMeshStructure::ContourMeshStructure(const ContourMeshStructure & iE):
  TraceStructure( iE ),
  TCoord( iE.TCoord )
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
ContourMeshStructure::~ContourMeshStructure()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool ContourMeshStructure::IsAContour()
{
  return ( this->GetDirection() != -1 );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int ContourMeshStructure::GetDirection()
{
  double bounds[6];

  this->Nodes->GetBounds(bounds);

  int oDir = -1;

  for ( int i = 0; i < 3; i++ )
    {
    if ( bounds[2 * i] == bounds[2 * i + 1] )
      {
      oDir = 2 - i;
      }
    }

  return oDir;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void ContourMeshStructure::SetActorProperties( vtkProperty* iProperty ) const
  {
  if( iProperty )
    {
    if( this->ActorXY )
      {
      this->ActorXY->SetProperty( iProperty );
      }
    if( this->ActorXZ )
      {
      this->ActorXZ->SetProperty( iProperty );
      }
    if( this->ActorYZ )
      {
      this->ActorYZ->SetProperty( iProperty );
      }
    if( this->ActorXYZ )
      {
      this->ActorXYZ->SetProperty( iProperty );
      }
    }
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void ContourMeshStructure::SetActorVisibility( const bool& iVisible ) const
{
  if ( this->ActorXY )
    {
    this->ActorXY->SetVisibility(iVisible);
    }
  if ( this->ActorXZ )
    {
    this->ActorXZ->SetVisibility(iVisible);
    }
  if ( this->ActorYZ )
    {
    this->ActorYZ->SetVisibility(iVisible);
    }
  if ( this->ActorXYZ )
    {
    this->ActorXYZ->SetVisibility(iVisible);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
ContourMeshStructure::
SetScalarData( const std::string& iName,
               const double& iValue ) const
{
  if( this->Nodes )
    {
    vtkIdType NbOfPoints = this->Nodes->GetNumberOfPoints();
    vtkDoubleArray* data = vtkDoubleArray::New();
    data->SetNumberOfComponents( 1 );
    data->SetName( iName.c_str() );

    for( vtkIdType i = 0; i < NbOfPoints; ++i )
      {
      data->InsertNextValue( iValue );
      }

    this->Nodes->GetPointData()->SetScalars( data );
    this->Nodes->GetPointData()->SetActiveScalars( iName.c_str() );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
ContourMeshStructure::
SetScalarRange( const double& iMin, const double& iMax ) const
{
  if( this->ActorXY )
    {
    this->ActorXY->GetMapper()->SetScalarRange( iMin, iMax );
    this->ActorXY->GetMapper()->SetScalarVisibility( true );
    }
  if( this->ActorXZ )
    {
    this->ActorXZ->GetMapper()->SetScalarRange( iMin, iMax );
    this->ActorXY->GetMapper()->SetScalarVisibility( true );
    }
  if( this->ActorYZ )
    {
    this->ActorYZ->GetMapper()->SetScalarRange( iMin, iMax );
    this->ActorXY->GetMapper()->SetScalarVisibility( true );
    }
  if( this->ActorXYZ )
    {
    this->ActorXYZ->GetMapper()->SetScalarRange( iMin, iMax );
    this->ActorXYZ->GetMapper()->SetScalarVisibility( true );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
ContourMeshStructure::
RenderWithOriginalColors() const
{
  if( this->Nodes )
    {
    this->Nodes->GetPointData()->SetActiveScalars( NULL );
    }

  if( this->ActorXY )
    {
    this->ActorXY->GetMapper()->SetScalarVisibility( false );
    }
  if( this->ActorXZ )
    {
    this->ActorXY->GetMapper()->SetScalarVisibility( false );
    }
  if( this->ActorYZ )
    {
    this->ActorXY->GetMapper()->SetScalarVisibility( false );
    }
  if( this->ActorXYZ )
    {
    this->ActorXY->GetMapper()->SetScalarVisibility( false );
    }
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void
ContourMeshStructure::
SetLookupTable( vtkLookupTable* iLut ) const
{
  if( iLut )
    {
    if( this->ActorXY )
      {
      this->ActorXY->GetMapper()->SetLookupTable( iLut );
      }
    if( this->ActorXZ )
      {
      this->ActorXY->GetMapper()->SetLookupTable( iLut );
      }
    if( this->ActorYZ )
      {
      this->ActorXY->GetMapper()->SetLookupTable( iLut );
      }
    if( this->ActorXYZ )
      {
      this->ActorXY->GetMapper()->SetLookupTable( iLut );
      }
    }
}
//--------------------------------------------------------------------------
