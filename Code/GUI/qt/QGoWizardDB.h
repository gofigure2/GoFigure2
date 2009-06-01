#ifndef __QGoWizardDB_h
#define __QGoWizardDB_h

#include "GoDBRecordSet.h"

#include <qwizard.h>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>
#include <QFormLayout>
#include <QComboBox>
#include <QStringList>
#include <QString>
#include <iostream>

#include "MegaVTK2Configure.h"


template< class myT >
void RecordValues_inTable(const char* ServerName,const char* User,
          const char* Password,const char* NameDB,const char* TableName, myT& myNewObject )
{
  typedef GoDBRecordSet< myT >   SetType;

  SetType* mySet = new SetType;
  mySet->SetServerName(ServerName);
  mySet->SetDataBaseName(NameDB);
  mySet->SetTableName( TableName );
  mySet->SetUser( User);
  mySet->SetPassword( Password);
  mySet->PopulateFromDB();
  mySet->AddObject( myNewObject );
  mySet->SaveInDB();

  delete mySet;
  return;
};

class QGoWizardDB : public QWizard
{
Q_OBJECT

public:
  //enum { Page_Connect_Server, Page_OpenOrCreateDB, Page_CreateExperiment};

  QMEGAVTKADDON2_EXPORT QGoWizardDB(QWidget *parent = 0);

};

/*class MyQWizardPage : public QWizardPage
{
public:
  void MyMethod()
};*/

class Connect_ServerPage : public QWizardPage
{
Q_OBJECT

public:
  Connect_ServerPage(QWidget *parent = 0);

  //int nextId() const;

private:
  QLineEdit* lineServerName;
  QLineEdit* lineUserName;
  QLineEdit* linePassword;

 };

class OpenOrCreate_Page : public QWizardPage
{
Q_OBJECT

public:
  OpenOrCreate_Page(QWidget *parent = 0);
 // int nextId() const;
  void initializePage();
  bool validatePage();
  QFormLayout* formLayout;
  QLineEdit* lineNewDBName;
  //QComboBox* ChoiceDB;
 // char* Server;
 // char* User;
 // char* Password;
//public slot:

private:
  QRadioButton *openDBRadioButton;
  QRadioButton *createDBRadioButton;
  QLineEdit* DBNametoOpen_fake;
  QComboBox* ChoiceDB;
  QString m_NameDB;
  QStringList m_ListDB;

protected slots:
  void PrintListDB ();
  void EnterNameDB ();


};
class Create_ExperimentPage : public QWizardPage
{
Q_OBJECT

public:
 Create_ExperimentPage( QWidget *parent = 0 );
 void initializePage();
 bool validatePage();

protected:
  QFormLayout* formLayout;
  QComboBox* ChoiceExp;
  QRadioButton* openExpRadioButton;
  QRadioButton* createExpRadioButton;
 // QLineEdit* ExperimentID;
  QLineEdit* Name;
  QLineEdit* Description;
  QLineEdit* TimeInterval;
  QLineEdit* TileHeight;
  QLineEdit* TileWidth;
  QLineEdit* PixelDepth;
  QLineEdit* PixelHeight;
  QLineEdit* PixelWidth;
  QLineEdit* ColorDepth;
  QLineEdit* nTimePoints;
  QLineEdit* nYTiles;
  QLineEdit* nXTiles;
  QLineEdit* nSlices;
  QLineEdit* nRows;
  QLineEdit* nColumns;
  QLineEdit* FilePattern;
  QLineEdit* OpenOrCreateExp_fake;
  QLineEdit* ExpID_fake;
  QLineEdit* NameDB_fake;
  QStringList m_ListExpID;


protected slots:
  void PrintListExp();
  void EnterInfoExp();
  void PrintValuesExpID(QString ExpID);


};
class Import_SerieGridPage : public QWizardPage
{
Q_OBJECT

public:
  Import_SerieGridPage(QWidget *parent = 0);
  void initializePage();

protected:
  QFormLayout* formlayout;
  QLineEdit* RCoord;
  QLineEdit* CCoord;
  QLineEdit* TCoord;
  QLineEdit* YCoord;
  QLineEdit* XCoord;
  QLineEdit* ZCoord;
  QLineEdit* FileName;
  QLineEdit* OpenOrCreateSeriesGrid_fake;
  QLineEdit* ImageID_fake;
  QComboBox* ChoiceSeriesGrid;
  QRadioButton* openSeriesGridRadioButton;
  //QRadioButton* createSeriesGridRadioButton;
  QPushButton* BrowseButton;
  QStringList m_ListImageID;

protected slots:
  void PrintListSeriesGrid();
  void EnterInfoSeriesGrid();
  void PrintValuesImageID(int ImageID);


};

class Import_ManualSegmentationPage : public QWizardPage
{
Q_OBJECT

public:
  Import_ManualSegmentationPage(QWidget *parent = 0);
  void initializePage();

};

#endif
