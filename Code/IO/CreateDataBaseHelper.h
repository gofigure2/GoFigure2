/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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
#ifndef __CreateDataBaseHelper_h
#define __CreateDataBaseHelper_h

#include <vector>
#include <string>
#include "itkMacro.h"
#include "vtkMySQLDatabase.h"

#include "QGoIOConfigure.h"

QGOIO_EXPORT bool IsDatabaseOfGoFigureType(
  vtkMySQLDatabase* DataBaseConnector);
QGOIO_EXPORT bool DoesDatabaseExit(std::string ServerName, std::string login,
                                   std::string Password, std::string DBName);

QGOIO_EXPORT void Query(vtkMySQLDatabase* DataBaseConnector,
                        std::string queryScript);

QGOIO_EXPORT bool CreateGoFigureDataBase(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName);
QGOIO_EXPORT bool CreateDataBase(vtkMySQLDatabase* DataBaseConnector,
                                 std::string DBName);
QGOIO_EXPORT void CreateTables(
  vtkMySQLDatabase* DataBaseConnector);
QGOIO_EXPORT void CreateForeignKeys(
  vtkMySQLDatabase* DataBaseConnector);

QGOIO_EXPORT std::string CellTypeTable();
QGOIO_EXPORT std::string AuthorTable();
QGOIO_EXPORT std::string SubCellularTypeTable();
QGOIO_EXPORT std::string CoordinateTable();
QGOIO_EXPORT std::string ColorTableScript();
QGOIO_EXPORT std::string MicroscopeTable();
QGOIO_EXPORT std::string ProjectTable();
QGOIO_EXPORT std::string ImagingSessionTable();
QGOIO_EXPORT std::string TrackFamilyTable();
QGOIO_EXPORT std::string TrackTable();
QGOIO_EXPORT std::string MeshTable();
QGOIO_EXPORT std::string ContourTable();
QGOIO_EXPORT std::string ChannelTable();
QGOIO_EXPORT std::string ImageTable();
QGOIO_EXPORT std::string LineageTable();
QGOIO_EXPORT std::string BookmarkTable();
QGOIO_EXPORT std::string IntensityTable();
QGOIO_EXPORT std::string ValueTypeTable();
QGOIO_EXPORT std::string CalculatedValueTable();
QGOIO_EXPORT std::string ValuePerVectorCoordTable();
QGOIO_EXPORT std::string MeshValueTable();
QGOIO_EXPORT std::string TrackValueTable();
QGOIO_EXPORT std::string ImageValueTable();
QGOIO_EXPORT std::string ImagingSessionValueTable();
QGOIO_EXPORT std::string ContourValueTable();
QGOIO_EXPORT std::string LineageValueTable();

std::string ProjectFK();
std::string ImagingSessionFKMicroscopeName();
std::string ImagingSessionFKProjectName();
std::string ImagingSessionFKCoordIDMin();
std::string ImagingSessionFKCoordIDMax();
std::string TrackFamilyFKTrackIDDaughter1();
std::string TrackFamilyFKTrackIDDaughter2();
std::string TrackFamilyFKTrackIDMother();
std::string TrackFKColor();
std::string TrackFKLineage();
std::string TrackFKCoordIDMax();
std::string TrackFKCoordIDMin();
std::string TrackFKTrackFamily();
std::string TrackFKImagingSession();
std::string MeshFKImagingSession();
std::string MeshFKTrackID();
std::string MeshFKColor();
std::string MeshFKCoordIDMin();
std::string MeshFKCoordIDMax();
std::string MeshFKSubCellType();
std::string MeshFKCellType();
std::string ContourFKImagingSession();
std::string ContourFKCoordIDMin();
std::string ContourFKCoordIDMax();
std::string ContourFKMesh();
std::string ChannelFKColor();
std::string ChannelFKImagingSession();
std::string ImageFKChannel();
std::string ImageFKCoordIDMin();
std::string ImageFKImagingSession();
std::string LineageFKImagingSession();
std::string LineageFKTrackRoot();
std::string LineageFKColor();
std::string LineageFKCoordIDMin();
std::string LineageFKCoordIDMax();
std::string BookmarkFKCoord();
std::string BookmarkFKImagingSession();
std::string IntensityFKChannel();
std::string IntensityFKMesh();
std::string ValueperVectorCoordFKCalculatedValue();
std::string CalculatedValueFKValueType();
std::string MeshValueFKCalculatedValue();
std::string MeshValueFKMesh();
std::string TrackValueFKCalculatedValue();
std::string TrackValueFKTrack();
std::string ImageValueFKCalculatedValue();
std::string ImageValueFKImage();
std::string ImagingSessionValueFKCalculatedValue();
std::string ImagingSessionValueFKImagingSession();
std::string ContourValueFKCalculatedValue();
std::string ContourValueFKContour();
std::string LineageValueFKCalculatedValue();
std::string LineageValueFKLineage();

#endif
