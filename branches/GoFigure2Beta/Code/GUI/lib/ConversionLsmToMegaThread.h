/*=========================================================================
  Author: $Author: nr52 $  // Author of last commit
  Version: $Rev: 585 $  // Revision of last commit
  Date: $Date: 2009-08-20 21:25:19 -0400 (Thu, 20 Aug 2009) $  // Date of last commit
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
#ifndef __ConversionLsmToMegaThread_h
#define __ConversionLsmToMegaThread_h

#include <qstring.h>
#include <qthread.h>
#include <vector>

#include "fstream"
#include "GoFigureGlobalDefinition.h"

class vtkLSMReader;

class ConversionLsmToMegaThread : public QThread
{
  Q_OBJECT

public:
  ConversionLsmToMegaThread(){}
  virtual ~ConversionLsmToMegaThread(){}
  void SetLsmPath( std::string iLsmPath);
  void SetMegaPath( std::string iMegaPath);
  void SetOutputFileType( const GoFigure::FileType& iFileType );
  void ExportWithReimplemented( std::string iMegaPath );

public slots:

signals:
  void ConversionTerminatedSent();

protected:
  void run();

private:
  ConversionLsmToMegaThread( const ConversionLsmToMegaThread& );
  ConversionLsmToMegaThread operator = ( const ConversionLsmToMegaThread& );
  std::string m_LsmPath;
  std::string m_MegaPath;
  GoFigure::FileType m_FileType;
  std::vector< vtkLSMReader* > m_LSMReaders;
  unsigned int m_Plaque;
  unsigned int m_Row;
  unsigned int m_Column;
  unsigned int m_XTile;
  unsigned int m_YTile;
  unsigned int m_ZTile;
  double m_XOverlap;
  double m_YOverlap;
  double m_ZOverlap;
};

#endif
