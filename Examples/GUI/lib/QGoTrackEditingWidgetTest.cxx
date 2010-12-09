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

#include <QApplication>
#include <QTimer>

#include "QGoTrackEditingWidget.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QGoTrackEditingWidget *win = new QGoTrackEditingWidget();

  // Define some tracks manually

  typedef std::pair< unsigned int, std::pair< unsigned int,double*> > Mesh;
  typedef std::pair< unsigned int, std::list<Mesh> > Track;

  // Create 10 meshes
  Mesh mesh0;
  mesh0.first = 1;
  mesh0.second.first = 2;
  double* position0 = new double[3];
  position0[0] = 10;
  position0[1] = 10;
  position0[2] = 10;
  mesh0.second.second = position0;

  Mesh mesh1;
  mesh1.first = 2;
  mesh1.second.first = 4;
  double* position1 = new double[3];
  position1[0] = 20;
  position1[1] = 20;
  position1[2] = 20;
  mesh1.second.second = position1;

  Mesh mesh2;
  mesh2.first = 3;
  mesh2.second.first = 6;
  double* position2 = new double[3];
  position2[0] = 30;
  position2[1] = 30;
  position2[2] = 30;
  mesh2.second.second = position2;

  Track track0;
  track0.first = 3;
  track0.second.push_back(mesh0);
  track0.second.push_back(mesh1);
  track0.second.push_back(mesh2);

  std::list<Track> trackList;
  trackList.push_back(track0);
/*
  Mesh mesh0;
  mesh0.first = 1;
  mesh0.second.first = 2;
  double* position0 = new double[3];
  position0[0] = 10;
  position0[1] = 10;
  position0[2] = 10;
  mesh0.second.second = 2;

  Mesh mesh0;
  mesh0.first = 1;
  mesh0.second.first = 2;
  double* position0 = new double[3];
  position0[0] = 10;
  position0[1] = 10;
  position0[2] = 10;
  mesh0.second.second = 2;

  Mesh mesh0;
  mesh0.first = 1;
  mesh0.second.first = 2;
  double* position0 = new double[3];
  position0[0] = 10;
  position0[1] = 10;
  position0[2] = 10;
  mesh0.second.second = 2;


*/

  win->setTracks(trackList);
  win->generateTrackRepresentation();
  win->show();

//  if( atoi( argv[1] ) == 1 )
//  {
// timer->start( 1000 );
// }

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();
//  delete timer;
  delete win;

  return output;
}
