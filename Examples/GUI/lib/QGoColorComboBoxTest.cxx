/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
  Version: $Rev: 1803 $  // Revision of last commit
  Date: $Date: 2010-07-15 13:48:08 -0400 (Thu, 15 Jul 2010) $  // Date of last commit
=========================================================================*/

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

#include "QGoSelectedColorComboBox.h"
#include "QGoPrintDatabase.h"

int main(int argc, char * argv[])
{
  if (argc != 3)
    {
    std::cerr << "QGoColorComboTest requires 2 arguments:" << std::endl;
    std::cerr << "1-test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain("http://gofigure2.sourceforge.net");

  QGoSelectedColorComboBox* win = new QGoSelectedColorComboBox("Add a new color...");
  QGoPrintDatabase* DBTables = new QGoPrintDatabase;
  DBTables->SetDatabaseVariables("gofiguredatabase", "localhost", "gofigure",
    "gofigure", atoi(argv[1]), argv[2]);
  win->setItemsWithColorFromDB(DBTables->GetColorComboBoxInfofromDB());
  //QGoPrintDatabase* win = new QGoPrintDatabase;
  // win.FillTableFromDatabase(argv[1],"localhost","gofigure",
  //    "gofigure",atoi(argv[2]), argv[3]);
  //win->SetDatabaseVariables(
  //  "gofiguredatabase", "localhost", "gofigure",
 //   "gofigure", 8, "LSM_Converter");
 // win->FillTableFromDatabase(2);

  win->show();

/*  QTimer* timer = new QTimer;
  timer->setSingleShot(true);
  QObject::connect(timer, SIGNAL(timeout()), win, SLOT(close()));

  if (atoi(argv[1]) == 0)
    {
    timer->start(1000);
    }*/

  int output = app.exec();

  app.closeAllWindows();

  delete win;
 // delete timer;

  return output;
}