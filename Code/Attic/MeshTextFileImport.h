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

#ifndef __MeshTextFileImport_h
#define __MeshTextFileImport_h

#include <string>
#include <list>
#include <vector>

#include "QGoGUILibConfigure.h"

class vtkPolyData;
class vtkMySQLDatabase;

class QGOGUILIB_EXPORT MeshTextFileImport
{
public:
  MeshTextFileImport(const std::string & iServerName, const std::string & iLogin,
                     const std::string & iPassword, const std::string & iDBName,
                     const unsigned int & iImagingSessionId);

  ~MeshTextFileImport();

  void SetDirectory(const std::string &);

  void SetFileName(const std::string &);

  void Read();

private:
  unsigned int m_ImagingSessionId;
  unsigned int m_NumberOfChannels;
  unsigned int m_NumberOfMeshes;
  std::string  m_MegaCaptureHeaderFile;
  std::string  m_Directory;
  std::string  m_FileName;

  vtkMySQLDatabase *m_DBConnector;

  struct InternalMeshStructure {
    InternalMeshStructure(const unsigned int & iNumberOfChannels):
      m_AverageIntensity(iNumberOfChannels, 0) {}

    unsigned int m_TrackId;
    unsigned int m_TCoord;
    double m_Volume;
    unsigned int m_XMin;
    unsigned int m_XMax;
    unsigned int m_YMin;
    unsigned int m_YMax;
    unsigned int m_ZMin;
    unsigned int m_ZMax;
    vtkPolyData *m_Points;
    std::vector< double > m_AverageIntensity;
  };

  std::list< InternalMeshStructure > m_ListOfMeshes;

  void SaveMeshInDataBase(const InternalMeshStructure & iMesh);
};
#endif
