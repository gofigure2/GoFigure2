/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include <vector>

#include "ContourToMeshFilter.h"

void CreateCircle(const double & iZ,
                  const double & iRadius,
                  const int & iResolution,
                  vtkPolyData *ioContour)
{
  vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
  points->SetNumberOfPoints(iResolution);

  vtkSmartPointer< vtkCellArray > lines = vtkSmartPointer< vtkCellArray >::New();
  lines->Allocate(1, iResolution);

  vtkIdType *lineIndices = new vtkIdType[iResolution + 1];

  for ( int i = 0; i < iResolution; ++i )
    {
    double theta = vtkMath::RadiansFromDegrees( 360. * i / double(iResolution) );
    double x = iRadius * cos(theta);
    double y = iRadius * sin(theta);
    points->SetPoint(i, x, y, iZ);
    lineIndices[i] = static_cast< vtkIdType >( i );
    }
  lineIndices[iResolution] = 0;

  lines->InsertNextCell(iResolution + 1, lineIndices);
  delete[] lineIndices;

  ioContour->Initialize();
  ioContour->SetLines(lines);
  ioContour->SetPoints(points);
}

int main(int argc, char *argv[])
{
  if ( argc != 2 )
    {
    std::cerr << "./ContourToMeshFilter(.exe) takes 1 arguments" << std::endl;
    std::cerr << "1- boolean value (test)" << std::endl;
    return EXIT_FAILURE;
    }

  int    resolution = 100;
  double z = 0.;
  double lastz = 0.;
  double sphereRadius = 1.;
  double zmin = -0.9 * sphereRadius;
  double zmax = -zmin;

  std::vector< vtkSmartPointer< vtkPolyData > > contours(20);

  for ( int i = 0; i < 20; i++ )
    {
    if ( ( i < 4 ) || ( i > 10 ) )
      {
      lastz = z;
      double u = static_cast< double >( i ) / 20.;
      z = ( 1. - u ) * zmin + u * zmax;
      double radius = sqrt(sphereRadius * sphereRadius - z * z);
      contours[i] = vtkSmartPointer< vtkPolyData >::New();
      CreateCircle(z, radius, resolution, contours[i]);
      }
    else
      {
      contours[i] = NULL;
      }
    }

  typedef itk::ContourToMeshFilter< std::vector< vtkSmartPointer< vtkPolyData > > > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->ProcessContours(contours);

  vtkSmartPointer< vtkPolyDataMapper > mapper =
    vtkSmartPointer< vtkPolyDataMapper >::New();
  mapper->SetInput( filter->GetOutput() );

  vtkSmartPointer< vtkActor > actor =
    vtkSmartPointer< vtkActor >::New();
  actor->SetMapper(mapper);

  vtkSmartPointer< vtkRenderer > renderer =
    vtkSmartPointer< vtkRenderer >::New();
  renderer->AddActor(actor);

  vtkSmartPointer< vtkRenderWindow > renwin =
    vtkSmartPointer< vtkRenderWindow >::New();
  renwin->AddRenderer(renderer);

  vtkSmartPointer< vtkRenderWindowInteractor > iren =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  iren->SetRenderWindow(renwin);

  renwin->Render();

  if ( atoi(argv[1]) == 1 )
    {
    iren->CreateOneShotTimer(1);
    }
  else
    {
    iren->Start();
    }

  filter->GetOutput()->Delete();

  return EXIT_SUCCESS;
}