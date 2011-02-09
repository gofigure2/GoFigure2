/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "QGoGUILibConfigure.h"

class vtkLSMReader;

class QGOGUILIB_EXPORT ConversionLsmToMegaThread:public QThread
{
  Q_OBJECT
public:
  /**
   * \brief Constructor
   */
  ConversionLsmToMegaThread();

  /**
   * \brief Destructor
   */
  virtual ~ConversionLsmToMegaThread();

  /**
   * \brief Set the base name of the LSM file to convert
   * \param[in] iBaseName Name of the LSM file
   */
  void SetBaseName(std::string iBaseName);

  /**
   * \brief Set the path to the LSM file to convert and initialise LSM reader
   * \param[in] iLsmPath Path of the LSM file
   */
  void SetLsmPath(std::string iLsmPath);

  /**
   * \brief Set the path of the MegaCapture file to create
   * \param[in] iMegaPath Path of the MegaCapture file
   */
  void SetMegaPath(std::string iMegaPath);

  /**
   * \brief Set the output file type
   * \param[in] iFileType File type: PNG or TIFF
   */
  void SetOutputFileType(const GoFigure::FileType & iFileType);

  /**
   * \brief Start the conversion to MegaCapture
   * \param[in] iMegaPath path of the output MegaCapture file
   */
  void ExportWithReimplemented(std::string iMegaPath);

  /**
   * \brief Returns the number of signals to be sent for the progress bar
   */
  int  GetNumberOfPoints();

public slots :
signals:
  void ConversionTerminatedSent();

  void InitialisationProgressSent();

  void ProgressSent();

protected:
  /**
   * \brief Start multithread process (call when parameters are set up properly)
   */
  void run();

private:
  ConversionLsmToMegaThread(const ConversionLsmToMegaThread &);
  ConversionLsmToMegaThread operator=(const ConversionLsmToMegaThread &);

  std::string                   m_BaseName;
  std::string                   m_LsmPath;
  std::string                   m_MegaPath;
  GoFigure::FileType            m_FileType;
  std::vector< vtkLSMReader * > m_LSMReaders;
  unsigned int                  m_Plaque;
  unsigned int                  m_Row;
  unsigned int                  m_Column;
  unsigned int                  m_XTile;
  unsigned int                  m_YTile;
  unsigned int                  m_ZTile;
  double                        m_XOverlap;
  double                        m_YOverlap;
  double                        m_ZOverlap;
  unsigned int                  m_NumberOfChannels;
  unsigned int                  m_NumberOfTimePoints;
  unsigned int                  m_Dim;
};

#endif
