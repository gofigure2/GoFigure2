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
#include <iostream>
#include "QGoMeshEditingWidgetManager.h"
#include "QGoTraceSettingsWidget.h"
#include "QGoModesManagerWidget.h"
#include "QGoAlgorithmWidget.h"
#include "QGoTraceSettingsWidget.h"
#include "QGoAlgoParameter.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"

//**************************************************************************//
//                               MAIN                                       //
//**************************************************************************//

int main(int argc, char *argv[])
{
  if ( argc != 1 )
    {
    return EXIT_FAILURE;
    }

  QApplication app(argc, argv);
  //QTimer *     timer = new QTimer;
  //timer->setSingleShot(true);

  std::vector<QString> ChannelName;
  ChannelName.push_back("Channel 1");
  ChannelName.push_back("Channel 2");
  QStringList ListTimePoint;
  ListTimePoint.append("t-1");
  ListTimePoint.append("t");
  ListTimePoint.append("t+1");
  vtkPoints* Seeds = NULL;

  std::vector< vtkSmartPointer< vtkImageData > >* Images = NULL;
  QGoMeshEditingWidgetManager* MeshEditing = new QGoMeshEditingWidgetManager(ChannelName, ListTimePoint,
    Seeds, Images, NULL);
  
  //QObject::connect( timer, SIGNAL( timeout() ), window, SLOT( close() ) );
  //timer->start(1000);

  MeshEditing->showWidget();

  app.processEvents();
  int output = app.exec();

  
  app.closeAllWindows();

  //delete timer;
  delete MeshEditing;

  return output;
}
