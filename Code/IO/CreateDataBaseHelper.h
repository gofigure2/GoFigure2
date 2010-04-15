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
#ifndef __CreateDataBaseHelper_h
#define __CreateDataBaseHelper_h

#include <vector>
#include <string>
#include "itkMacro.h"
#include "vtkMySQLDatabase.h"

#include "GoFigureLibsConfigure.h"

GOFIGURELIBS_EXPORT bool IsDatabaseOfGoFigureType(
  vtkMySQLDatabase* DataBaseConnector);
GOFIGURELIBS_EXPORT bool DoesDatabaseExit(std::string ServerName, std::string login, 
  std::string Password, std::string DBName);

GOFIGURELIBS_EXPORT void Query(vtkMySQLDatabase* DataBaseConnector,
           std::string queryScript);

GOFIGURELIBS_EXPORT bool CreateGoFigureDataBase(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName );
GOFIGURELIBS_EXPORT bool CreateDataBase(vtkMySQLDatabase* DataBaseConnector,
  std::string DBName);
GOFIGURELIBS_EXPORT void CreateTables(
  vtkMySQLDatabase* DataBaseConnector );
GOFIGURELIBS_EXPORT void CreateForeignKeys(
  vtkMySQLDatabase* DataBaseConnector );

GOFIGURELIBS_EXPORT std::string CellTypeTable();
GOFIGURELIBS_EXPORT std::string AuthorTable();
GOFIGURELIBS_EXPORT std::string SubCellularTypeTable();
GOFIGURELIBS_EXPORT std::string CoordinateTable();
GOFIGURELIBS_EXPORT std::string ColorTableScript();
GOFIGURELIBS_EXPORT std::string MicroscopeTable();
GOFIGURELIBS_EXPORT std::string ProjectTable();
GOFIGURELIBS_EXPORT std::string ImagingSessionTable();
GOFIGURELIBS_EXPORT std::string TrackFamilyTable();
GOFIGURELIBS_EXPORT std::string TrackTable();
GOFIGURELIBS_EXPORT std::string MeshTable();
GOFIGURELIBS_EXPORT std::string ContourTable();
GOFIGURELIBS_EXPORT std::string ChannelTable();
GOFIGURELIBS_EXPORT std::string ImageTable();
GOFIGURELIBS_EXPORT std::string LineageTable();
GOFIGURELIBS_EXPORT std::string BookmarkTable();
GOFIGURELIBS_EXPORT std::string IntensityTable();
GOFIGURELIBS_EXPORT std::string ValueTypeTable();
GOFIGURELIBS_EXPORT std::string CalculatedValueTable();
GOFIGURELIBS_EXPORT std::string ValuePerVectorCoordTable();
GOFIGURELIBS_EXPORT std::string MeshValueTable();
GOFIGURELIBS_EXPORT std::string TrackValueTable();
GOFIGURELIBS_EXPORT std::string ImageValueTable();
GOFIGURELIBS_EXPORT std::string ImagingSessionValueTable();
GOFIGURELIBS_EXPORT std::string ContourValueTable();
GOFIGURELIBS_EXPORT std::string LineageValueTable();

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
