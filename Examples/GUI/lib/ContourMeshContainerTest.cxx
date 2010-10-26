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

#include "QGoImageView3D.h"
#include "ContourMeshStructure.h"
#include "ContourMeshContainer.h"

#include "vtkActor.h"
#include "vtkPolyData.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain("http://gofigure2.sourceforge.net");

  QGoImageView3D *viewer = new QGoImageView3D;

  ContourMeshContainer container(NULL, viewer);
  unsigned             TimePoint = 0;
  bool                 highlighted = true;
  bool                 visible = true;
  unsigned int         i = 0;

  for (; i < 20; i++ )
    {
    ContourMeshStructure element(i,
                                 vtkActor::New(),
                                 vtkActor::New(),
                                 vtkActor::New(),
                                 vtkActor::New(),
                                 vtkPolyData::New(),
                                 TimePoint,
                                 highlighted,
                                 visible,
                                 1., 0., 0., 1.);
    container.Insert(element);
    }

  TimePoint = 1;
  for (; i < 30; i++ )
    {
    ContourMeshStructure element(i,
                                 vtkActor::New(),
                                 vtkActor::New(),
                                 vtkActor::New(),
                                 vtkActor::New(),
                                 vtkPolyData::New(),
                                 TimePoint,
                                 !highlighted,
                                 visible,
                                 0., 1., 0., 1.);
    container.Insert(element);
    }

  container.Print();

  container.RemoveActorsWithGivenTimePoint(0);
  container.AddActorsWithGivenTimePoint(1);

  container.DeleteAllHighlightedElements();

  for ( i = 0; i < 30; i++ )
    {
    container.DeleteElement(i);
    }

  delete viewer;

  return EXIT_SUCCESS;
}
