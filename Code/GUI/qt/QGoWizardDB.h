/*=========================================================================
  Author: $Author:$  // Author of last commit
  Version: $Rev:$  // Revision of last commit
  Date: $Date:$  // Date of last commit
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
#ifndef __QGoWizardDB_h
#define __QGoWizardDB_h

#include "GoDBRecordSet.h"

#include <qwizard.h>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>
#include <QFormLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QStringList>
#include <QString>
#include <iostream>
#include <QCheckBox>

#include "MegaVTK2Configure.h"
#include "GoFigureFileInfoHelper.h"


template< class T >
void RecordValues_inTable( const std::string& ServerName, const std::string& User,
  const std::string& Password, const std::string& NameDB,
  const std::string& TableName, T& myNewObject )
{
  typedef GoDBRecordSet< T >   SetType;

  SetType mySet;
  mySet.SetServerName( ServerName );
  mySet.SetDataBaseName( NameDB );
  mySet.SetTableName( TableName );
  mySet.SetUser( User );
  mySet.SetPassword( Password );
  mySet.PopulateFromDB();
  mySet.AddObject( myNewObject );
  mySet.SaveInDB();

  return;
};

class QGoWizardDB : public QWizard
{
  Q_OBJECT

public:
  enum { Connect_Server, OpenOrCreateDB,Create_Experiment, Import};

  QMEGAVTKADDON2_EXPORT QGoWizardDB(QWidget *parent = 0);
  QMEGAVTKADDON2_EXPORT FileListType ListFilenames();
  QMEGAVTKADDON2_EXPORT QString NameDB();
  QMEGAVTKADDON2_EXPORT int ExpID();
  QMEGAVTKADDON2_EXPORT QString ExpName();
  QMEGAVTKADDON2_EXPORT QString Server();
  QMEGAVTKADDON2_EXPORT QString login();
  QMEGAVTKADDON2_EXPORT QString Password();
  QMEGAVTKADDON2_EXPORT int IsLsm();

  QPushButton* nextButton;

};

class Connect_ServerPage : public QWizardPage
{
  Q_OBJECT

public:
  Connect_ServerPage(QWidget *parent = 0);
  bool validatePage();


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
  void initializePage();
  bool validatePage();
  QGridLayout* gridLayout;
  QLineEdit* lineNewDBName;

private:
  QCheckBox *openDBCheckBox;
  QCheckBox *createDBCheckBox;
  QLineEdit* DBNametoOpen_fake;
  QComboBox* ChoiceDB;
  QLabel* textChoiceDB;
  QLabel* textNewDBName;
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
 int nextId() const;

protected:
  QComboBox* ChoiceExp;
  QRadioButton* openExpRadioButton;
  QRadioButton* createExpRadioButton;
 // QLineEdit* ExperimentID;
  QLineEdit* Name;
  QLabel* textName;
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
  QLineEdit* NameDB_fake;
  QLabel* textChoiceExp;
  QLabel* ExpID;
  QLineEdit* ID;
  QStringList m_ListExpName;
  QStringList m_ListExpID;


protected slots:
  bool PrintListExp();
  void EnterInfoExp();
  void PrintValuesExpName(QString ExpName);


};
class Import_SerieGridPage : public QWizardPage
{
  Q_OBJECT

public:
  Import_SerieGridPage(QWidget *parent = 0);
  void initializePage();
  bool isComplete() const;
  QLabel* Explanation;
  QPushButton* BrowseButton;

protected:
  QGridLayout* gridlayout;
  QLineEdit* OpenOrCreateSeriesGrid_fake;
  QString newfilename;
  QLabel* IsLsm;
  

protected slots:
  void SelectSeriesGrid();

};


#endif
