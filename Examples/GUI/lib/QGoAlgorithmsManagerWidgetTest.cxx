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
#include <QStringList>

#include "QGoAlgorithmsManagerWidget.h"
#include "QGoAlgorithmWidget.h"




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
  QTimer *     timer = new QTimer;
  timer->setSingleShot(true);
  
  QGoAlgorithmsManagerWidget* SemiAutoModeMeshEditingAlgoWidget = new QGoAlgorithmsManagerWidget("SemiAutomatedWidget", NULL);

  QGoAlgorithmWidget* LevelSetWidget = new QGoAlgorithmWidget("LevelSet", SemiAutoModeMeshEditingAlgoWidget);
  QStringList ChannelName;
  ChannelName.append("Channel 1");
  ChannelName.append("Channel 2");
  LevelSetWidget->AddParameter("Channel", ChannelName);
  LevelSetWidget->AddParameter("Radius", 0, 10, 3);
  LevelSetWidget->AddAdvParameter("Curvature", 0, 100, 20);
  LevelSetWidget->AddAdvParameter("Iterations", 0, 1000, 100);


  QGoAlgorithmWidget* Shape3D = new QGoAlgorithmWidget("Shape 3D", SemiAutoModeMeshEditingAlgoWidget);
  Shape3D->AddParameter("Channel", ChannelName);
  Shape3D->AddParameter("Radius", 0, 10, 3);

  QStringList ShapeList;
  ShapeList.append("Sphere");
  ShapeList.append("Cube");

  Shape3D->AddAdvParameter("Shape", ShapeList);

  QGoAlgorithmWidget* WaterShedWidget = new QGoAlgorithmWidget("WaterShed", SemiAutoModeMeshEditingAlgoWidget);
  WaterShedWidget->AddParameter("Channel", ChannelName);
  WaterShedWidget->AddParameter("Radius", 0, 10, 3);
  WaterShedWidget->AddAdvParameter("Thres.Min.", 0, 10, 20);
  WaterShedWidget->AddAdvParameter("Thres.Min.", 0, 30, 50);
  WaterShedWidget->AddAdvParameter("Corr.Thres.", 0, 5, 2, 2);
  WaterShedWidget->AddAdvParameter("Alpha", 0, 5, 1.50, 2);
  WaterShedWidget->AddAdvParameter("Beta", 0, 5, 3, 1);

  SemiAutoModeMeshEditingAlgoWidget->AddMethod(LevelSetWidget);
  SemiAutoModeMeshEditingAlgoWidget->AddMethod(WaterShedWidget);
  SemiAutoModeMeshEditingAlgoWidget->AddMethod(Shape3D);

  SemiAutoModeMeshEditingAlgoWidget->SetCurrentIndex(0);

  QObject::connect( timer, SIGNAL( timeout() ), SemiAutoModeMeshEditingAlgoWidget, SLOT( close() ) );

  
  timer->start(1000);

  SemiAutoModeMeshEditingAlgoWidget->show();

  app.processEvents();
  int output = app.exec();

  app.closeAllWindows();

  delete timer;
  delete SemiAutoModeMeshEditingAlgoWidget;

  return output;
}
