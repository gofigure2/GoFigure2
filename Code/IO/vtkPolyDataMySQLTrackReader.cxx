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

#include "vtkPolyDataMySQLTrackReader.h"

#include "vtkObjectFactory.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

#include <sstream>

vtkCxxRevisionMacro(vtkPolyDataMySQLTrackReader, "$Revision$");
vtkStandardNewMacro(vtkPolyDataMySQLTrackReader);

//--------------------------------------------------------------------------
vtkPolyDataMySQLTrackReader::vtkPolyDataMySQLTrackReader()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyDataMySQLTrackReader::
~vtkPolyDataMySQLTrackReader()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer< vtkPolyData >
vtkPolyDataMySQLTrackReader::GetPolyData(const std::string & iString)
{
  std::stringstream str(iString);

  // Might not be useful
  vtkIdType N;

  str >> N;

  if ( N != 0 )
    {
    std::map< unsigned int, double * > orderedPoints = GetMap(iString);

    vtkSmartPointer< vtkIntArray > temporalArray = vtkSmartPointer< vtkIntArray >::New();
    temporalArray->SetNumberOfComponents(1);
    temporalArray->SetName("TemporalInformation");

    // read map and fill points
    vtkSmartPointer< vtkPoints >                 points = vtkSmartPointer< vtkPoints >::New();
    std::map< unsigned int, double * >::iterator it = orderedPoints.begin();

    while ( it != orderedPoints.end() )
      {
      temporalArray->InsertNextValue(it->first);
      points->InsertNextPoint(it->second);
      ++it;
      }

    // Clean the map
    it = orderedPoints.begin();
    while ( it != orderedPoints.end() )
      {
      delete[] it->second;
      ++it;
      }
    orderedPoints.clear();

    // Create a line from points
    vtkSmartPointer< vtkPolyLine > polyLine =
      vtkSmartPointer< vtkPolyLine >::New();

    polyLine->GetPointIds()->SetNumberOfIds( points->GetNumberOfPoints() );
    for ( vtkIdType i = 0; i < points->GetNumberOfPoints(); i++ )
      {
      polyLine->GetPointIds()->SetId(i, i);
      }

    //Create a cell array to store the lines in and add the lines to it
    vtkSmartPointer< vtkCellArray > cells =
      vtkSmartPointer< vtkCellArray >::New();
    cells->InsertNextCell(polyLine);

    //Create a polydata to store everything in
    vtkSmartPointer< vtkPolyData > polyData =
      vtkSmartPointer< vtkPolyData >::New();

    //add the points to the dataset
    polyData->SetPoints(points);

    //add the lines to the dataset
    polyData->SetLines(cells);

    //add the temporal information
    polyData->GetPointData()->AddArray(temporalArray);

    vtkSmartPointer< vtkDoubleArray > speedArray =
      vtkSmartPointer< vtkDoubleArray >::New();
    speedArray->SetNumberOfComponents(1);
    speedArray->SetName("SpeedInformation");
    polyData->GetPointData()->AddArray(speedArray);

    return polyData;
    }

  return NULL;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::map< unsigned int, double * >
vtkPolyDataMySQLTrackReader::GetMap(const std::string & iString)
{
  std::stringstream str(iString);

  std::map< unsigned int, double * > orderedPoints;

  // Might not be useful
  vtkIdType N;
  str >> N;

  if ( N != 0 )
    {
    double *     pt = NULL;
    unsigned int time = 0;

    // fill a map so the points will be ordered automatically
    for ( vtkIdType i = 0; i < N; i++ )
      {
      pt = new double[3];
      str >> pt[0] >> pt[1] >> pt[2];
      str >> time;
      orderedPoints.insert( std::pair< unsigned int, double * >(time, pt) );
      }
    }
  return orderedPoints;
}
