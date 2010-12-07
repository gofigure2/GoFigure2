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

#include "TraceStructure.h"

#include <iostream>
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkActor.h"
#include "vtkMapper.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkLookupTable.h"

//--------------------------------------------------------------------------
TraceStructure::
TraceStructure():TraceID(0),
  ActorXY(NULL), ActorXZ(NULL), ActorYZ(NULL), ActorXYZ(NULL), Nodes(NULL),
  Highlighted(false), Visible(false)
{
  this->rgba[0] = 1.;
  this->rgba[1] = 1.;
  this->rgba[2] = 1.;
  this->rgba[3] = 1.;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TraceStructure::
TraceStructure(const unsigned int & iTraceID,
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
TraceStructure::
TraceStructure( const unsigned int & iTraceID,
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
TraceStructure::
TraceStructure( const unsigned int & iTraceID,
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

//--------------------------------------------------------------------------
TraceStructure::
TraceStructure(const TraceStructure & iE):
  TraceID(iE.TraceID), ActorXY(iE.ActorXY), ActorXZ(iE.ActorXZ),
  ActorYZ(iE.ActorYZ), ActorXYZ(iE.ActorXYZ), Nodes(iE.Nodes),
  Highlighted(iE.Highlighted), Visible(iE.Visible)
{
  for ( int i = 0; i < 4; i++ )
    {
    this->rgba[i] = iE.rgba[i];
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TraceStructure::
~TraceStructure()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void TraceStructure::SetActorProperties( vtkProperty* iProperty ) const
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
void TraceStructure::SetActorVisibility( const bool& iVisible ) const
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
void
TraceStructure::
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
TraceStructure::
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
    this->ActorXZ->GetMapper()->SetScalarVisibility( true );
    }
  if( this->ActorYZ )
    {
    this->ActorYZ->GetMapper()->SetScalarRange( iMin, iMax );
    this->ActorYZ->GetMapper()->SetScalarVisibility( true );
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
TraceStructure::
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
    this->ActorXZ->GetMapper()->SetScalarVisibility( false );
    }
  if( this->ActorYZ )
    {
    this->ActorYZ->GetMapper()->SetScalarVisibility( false );
    }
  if( this->ActorXYZ )
    {
    this->ActorXYZ->GetMapper()->SetScalarVisibility( false );
    }
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void
TraceStructure::
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
      this->ActorXZ->GetMapper()->SetLookupTable( iLut );
      }
    if( this->ActorYZ )
      {
      this->ActorYZ->GetMapper()->SetLookupTable( iLut );
      }
    if( this->ActorXYZ )
      {
      this->ActorXYZ->GetMapper()->SetLookupTable( iLut );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
TraceStructure::ReleaseData() const
{
  if( this->ActorXY )
    {
    this->ActorXY->Delete();
    }
  if( this->ActorXZ )
    {
    this->ActorXZ->Delete();
    }
  if( this->ActorYZ )
    {
    this->ActorYZ->Delete();
    }
  if( this->ActorXYZ )
    {
    this->ActorXYZ->Delete();
    }
  if( this->Nodes )
    {
    this->Nodes->Delete();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
TraceStructure::ResetNodes() const
{
  if( this->Nodes )
    {
    if( this->Nodes->GetPointData() )
      {
      this->Nodes->GetPointData()->Reset();
      }
    if( this->Nodes->GetPoints() )
      {
      this->Nodes->GetPoints()->Reset();
      }
    if( this->Nodes->GetLines() )
      {
      this->Nodes->GetLines()->Reset();
      }
    if( this->Nodes )
      {
      this->Nodes->Reset();
      }
    }
}
