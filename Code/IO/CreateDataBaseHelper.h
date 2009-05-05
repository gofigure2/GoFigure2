#ifndef __CreateDataBaseHelper_h_
#define __CreateDataBaseHelper_h_

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"

bool CanConnectToDatabase( 
  char* ServerName, char* login, char* Password, char * DBName );

void CreateDataBase(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateBookmarksTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateCollectionInfoTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateExperimentTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateFigureTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateFigureFlavorTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateLineageTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateLineageFlavorTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateMeshTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateMeshFlavor(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateSeriesGridTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateTrackTable(
  char* ServerName, char* login, char* Password, char * DBName );
void CreateTrackFlavor(
  char* ServerName, char* login, char* Password, char * DBName );

void CreateDataBaseMain(
  char* ServerName, char* login, char* Password, char * DBName );

#endif
