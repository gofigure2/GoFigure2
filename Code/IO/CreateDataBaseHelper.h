#ifndef __CreateDataBaseHelper_h_
#define __CreateDataBaseHelper_h_

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"

bool CanConnectToDatabase(
  const char* ServerName, const char* login, const char* Password, const char* DBName );


void CreateDataBase(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateBookmarksTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateCollectionInfoTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateExperimentTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateFigureTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateFigureFlavorTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateLineageTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateLineageFlavorTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateMeshTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateMeshFlavor(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateSeriesGridTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateTrackTable(
  const char* ServerName, const char* login, const char* Password, const char* DBName );
void CreateTrackFlavor(
  const char* ServerName, const char* login, const char* Password, const char* DBName );

void CreateDataBaseMain(
  const char* ServerName, const char* login, const char* Password, const char* DBName );

#endif
