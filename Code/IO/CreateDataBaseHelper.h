#ifndef __CreateDataBaseHelper_h_
#define __CreateDataBaseHelper_h_

#include <vector>
#include <string>

std::vector<std::string> ListDataBases(
  const char* ServerName, const char* login,
  const char* Password);

std::vector<std::string> ListTables(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

bool CanConnectToDatabase(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

void DropDatabase( 
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

void DropTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName,
  const char* TableName );

bool DoesDataBaseExist(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

bool DoesTableExist(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName,
  const char* TableName );

bool IsDatabaseOfGoFigureType(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

void CreateDataBase(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateBookmarksTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateCollectionInfoTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateExperimentTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateFigureTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateFigureFlavorTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateLineageTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateLineageFlavorTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateMeshTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateMeshFlavor(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateSeriesGridTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateTrackTable(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );
void CreateTrackFlavor(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

void CreateDataBaseMain(
  const char* ServerName, const char* login,
  const char* Password, const char* DBName );

#endif
