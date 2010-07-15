/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#include <iostream>
#include <cstdlib>
#include "LSMToMegaCapture.h"
#include <qstring.h>
#include <qthread.h>
#include <vector>

#include "fstream"
#include "GoFigureGlobalDefinition.h"

#include "ConversionLsmToMegaThread.h"

#include "vtksys/SystemTools.hxx"

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "LSMToMegaCapture.h"

int main(int argc, char** argv)
{

  QApplication app(argc, argv);

  if (argc != 5)
    {
    std::cerr << "lsmtomegacapture requires 3 argument:" << std::endl;
    std::cerr << "1-lsm filename (without.lsm)" << std::endl;
    std::cerr << "2-lsm full name (full path with .lsm)" << std::endl;
    std::cerr << "3-lsm output path (output  directory with folder/)" << std::endl;
    std::cerr << "4-test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  // Note: EXIT_SUCCESS = 0
  int err = EXIT_SUCCESS;

  ConversionLsmToMegaThread* ConversionLsmToMegaThreadSend =
    new ConversionLsmToMegaThread;

  QString BaseName(argv[1]);
  QString LsmPath(argv[2]);
  QString MegaPath(argv[3]);

  vtksys::SystemTools::MakeDirectory(argv[3]);

  ConversionLsmToMegaThreadSend->SetBaseName(BaseName.toStdString());
  ConversionLsmToMegaThreadSend->SetLsmPath(LsmPath.toStdString());
  ConversionLsmToMegaThreadSend->SetOutputFileType(GoFigure::PNG);
  ConversionLsmToMegaThreadSend->SetMegaPath(MegaPath.toStdString());

  ConversionLsmToMegaThreadSend->start();

  // Wait until thread finishes running
  while (ConversionLsmToMegaThreadSend->isRunning())
    {
    // waiting loop
    }

  if (atoi(argv[4]) == 1)
    {
    // check if the directory containing Meg files is empty
    unsigned long length = 0;
    length = vtksys::SystemTools::FileLength(argv[3]);

    if (length == 0)
      {
      err = EXIT_FAILURE;
      std::cerr << "ERROR: Test failing because directory is empty..." << std::endl;
      }

    // other test will use resulting megacapture as inputs
    // these files will be removed via cmake
    // vtksys::SystemTools::RemoveADirectory( argv[3] );
    }

  delete ConversionLsmToMegaThreadSend;

  // err = 0 means that the test succeeded
  return err;
}
