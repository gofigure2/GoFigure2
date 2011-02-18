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

#include "QGoTraceEditingWidgetManager.h"
#include "QGoTraceSettingsWidget.h"
#include "QGoSeedBaseWidget.h"
#include "QGoContourManualSegmentationWidget.h"
#include "QGoContourSemiAutoLevelsetWidget.h"
#include "QGoContourSemiAutoWatershedWidget.h"
#include "QGoAdvancedParametersWidget.h"

#include "QGoContourManualSegmentationWidget.h"
#include "QGoAlgorithmsWidget.h"
#include "QGoContourSemiAutoShapeWidget.h"



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
  //QStringList ListFilters;
  //ListFilters.append("Shape");
  //ListFilters.append("LevelSet");
  //ListFilters.append("WaterShed");

  QGoTraceEditingWidgetManager* MeshEditing = new QGoTraceEditingWidgetManager("Mesh", NULL);

  //semi automated mode
  QGoAlgorithmsWidget* SemiAutomatedMethodsWidget = new QGoAlgorithmsWidget(MeshEditing);

  QGoSeedBaseWidget* LevelSetParamWidget = new QGoSeedBaseWidget(MeshEditing);
  QGoContourSemiAutoLevelsetWidget* LevelSetAdvParamWidget = new QGoContourSemiAutoLevelsetWidget(MeshEditing);
  SemiAutomatedMethodsWidget->AddMethod("Level Set 3D", LevelSetParamWidget, LevelSetAdvParamWidget);

  QGoSeedBaseWidget* WaterShedParamWidget = new QGoSeedBaseWidget(MeshEditing);
  QGoContourSemiAutoWatershedWidget* WaterShedAdvParamWidget = new QGoContourSemiAutoWatershedWidget(MeshEditing);
  SemiAutomatedMethodsWidget->AddMethod("WaterShed 3D", WaterShedParamWidget, WaterShedAdvParamWidget);
  
  //manual mode
  QGoAlgorithmsWidget* ManualMethodsWidget = new QGoAlgorithmsWidget(MeshEditing);

  QGoSeedBaseWidget* ParamMeshToContours = new QGoSeedBaseWidget(MeshEditing);
  QGoContourSemiAutoShapeWidget* AdvParamMeshToContours = new QGoContourSemiAutoShapeWidget(MeshEditing);
  ManualMethodsWidget->AddMethod("2D Shapes in 1 mesh", ParamMeshToContours, AdvParamMeshToContours);

  QGoSeedBaseWidget* ParamMeshToContoursLevelSet = new QGoSeedBaseWidget(MeshEditing);
  QGoContourSemiAutoLevelsetWidget* AdvParamMeshToContoursLevelSet = new QGoContourSemiAutoLevelsetWidget(MeshEditing);
  ManualMethodsWidget->AddMethod("2D Levelset in 1 mesh", ParamMeshToContoursLevelSet, AdvParamMeshToContoursLevelSet);

  QGoSeedBaseWidget* ParamMeshToContoursWaterShed = new QGoSeedBaseWidget(MeshEditing);
  QGoContourSemiAutoWatershedWidget* AdvParamMeshToContoursWaterShed = new QGoContourSemiAutoWatershedWidget(MeshEditing);
  ManualMethodsWidget->AddMethod("2D WaterShed within 1 mesh", ParamMeshToContoursWaterShed, AdvParamMeshToContoursWaterShed);


  MeshEditing->AddMode("Manual", ManualMethodsWidget);
  MeshEditing->AddMode("Semi Automated", SemiAutomatedMethodsWidget);

  //QObject::connect( timer, SIGNAL( timeout() ), window, SLOT( close() ) );

  
  //timer->start(1000);

  MeshEditing->show();

  app.processEvents();
  int output = app.exec();

  app.closeAllWindows();

  //delete timer;
  delete MeshEditing;

  return output;
}
