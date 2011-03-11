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

#include "QGoTraceEditingWidget.h"
#include "QGoTraceSettingsWidget.h"
#include "QGoModesManagerWidget.h"
#include "QGoAlgorithmWidget.h"
#include "QGoSeedBaseWidget.h"
#include "QGoTraceSettingsWidget.h"
#include "ConvertToStringHelper.h"
#include "QGoAlgoParameter.h"




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

  QGoTraceEditingWidget* MeshEditing = new QGoTraceEditingWidget("Mesh", NULL);

  //semi automated mode
  QStringList ChannelName;
  ChannelName.append("Channel 1");
  ChannelName.append("Channel 2");
  QStringList ListTimePoint;
  ListTimePoint.append("t-1");
  ListTimePoint.append("t");
  ListTimePoint.append("t+1");

  
  QGoAlgorithmsManagerWidget* SemiAutomatedMethodsWidget = new QGoAlgorithmsManagerWidget("Semi Automated", 
    ChannelName, ListTimePoint, MeshEditing);

  //levelSet
  QGoAlgorithmWidget* LevelSetWidget = new QGoAlgorithmWidget("LevelSet", SemiAutomatedMethodsWidget);
  
  //def of the parameters:
  QGoAlgoParameter<int> Radius ("Radius", false, 0, 10, 3);
  QGoAlgoParameter<int> Curvature ("Curvature", true, 0, 100, 20);
  QGoAlgoParameter<int> Iteration ("Iterations", true,0, 1000, 100);

  LevelSetWidget->AddGeneralParameter<int>(&Radius);
  LevelSetWidget->AddGeneralParameter<int>(&Curvature);
  LevelSetWidget->AddGeneralParameter<int>(&Iteration);
  
  //Shape3D
  QGoAlgorithmWidget* Shape3D = new QGoAlgorithmWidget("Shape 3D", SemiAutomatedMethodsWidget);

  Shape3D->AddGeneralParameter<int>(&Radius);
  QStringList ShapeList;
  ShapeList.append("Sphere");
  ShapeList.append("Cube");
  QGoAlgoParameter<std::string> Shape("Shape", true, ShapeList, "Sphere");
  Shape3D->AddGeneralParameter<std::string>(&Shape);

  //watershed
  QGoAlgorithmWidget* WaterShedWidget = new QGoAlgorithmWidget("WaterShed", SemiAutomatedMethodsWidget);

  WaterShedWidget->AddGeneralParameter<int>(&Radius);
  QGoAlgoParameter<int> ThresMin("Thres.Min.", true, 0, 10, 20);
  WaterShedWidget->AddGeneralParameter<int>(&ThresMin);
  QGoAlgoParameter<int> ThresMax("Thres.Min.", true, 0, 50, 30);
  WaterShedWidget->AddGeneralParameter<int>(&ThresMax);

  QGoAlgoParameter<double> CorrThres("Corr.Thres.", true, 0, 5, 2, 2);
  WaterShedWidget->AddGeneralParameter<double>(&CorrThres);

  QGoAlgoParameter<double> Alpha("Alpha", true, 0, 5, 2, 1.5);
  WaterShedWidget->AddGeneralParameter<double>(&Alpha);
 
  QGoAlgoParameter<double> Beta("Beta", true, 0, 5, 1, 3);
  WaterShedWidget->AddGeneralParameter<double>(&Beta);
  
  SemiAutomatedMethodsWidget->AddMethod(LevelSetWidget);
  SemiAutomatedMethodsWidget->AddMethod(WaterShedWidget);
  SemiAutomatedMethodsWidget->AddMethod(Shape3D);
  
  QGoModesManagerWidget* SemiAutomatedMode = new QGoModesManagerWidget(NULL);
  SemiAutomatedMode->AddAlgoManagerWidget(SemiAutomatedMethodsWidget);

  MeshEditing->SetModesManager(SemiAutomatedMode);

  //add mode:
  QGoTraceSettingsWidget* TestAddMode = new QGoTraceSettingsWidget(MeshEditing);
  MeshEditing->AddMode("TestAddMode", TestAddMode);

  //QObject::connect( timer, SIGNAL( timeout() ), window, SLOT( close() ) );

  
  //timer->start(1000);

  MeshEditing->show();

  app.processEvents();
  int output = app.exec();

  std::cout<< "Alpha "<< Alpha.GetValue()<<std::endl;
  std::cout<< "Beta "<< Beta.GetValue()<<std::endl;
  std::cout<< "Shape "<< Shape.Getvalue().c_str()<<std::endl;
  app.closeAllWindows();

  //delete timer;
  delete MeshEditing;

  return output;
}
