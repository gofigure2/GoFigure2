/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 542 $  // Revision of last commit
  Date: $Date: 2009-08-06 16:08:10 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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
#ifndef __QGoOpenCreateProjectPage_h
#define __QGoOpenCreateProjectPage_h


#include <QWizardPage>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QRadioButton>
#include <QComboBox>
#include <QStringList>
#include <QString>
#include <string>
#include "vtkMySQLDatabase.h"

class QGoOpenCreateProjectPage : public QWizardPage
{
  Q_OBJECT

public:
  QGoOpenCreateProjectPage(QWidget *parent = 0);
  mutable vtkMySQLDatabase* m_DatabaseConnector;

  void initializePage();
  bool validatePage();  
  void cleanupPage();
  int nextId() const;
  
private:

   /** \brief update the m_ListProject and return true if the list is not empty,
  false if there is no existing project.*/
  bool GetListProject()const;

  /** \brief insert the new project in the database with the information 
  filled by the user*/
  void CreateProject();

  /** \brief return the QStringList of the authors to be visualized by the user
  (concatenation of the firstname, middle name and last name and fill the map 
  mapping the concatenated names with the AuthorID.*/
  QStringList GetListAuthors();

  /** \brief return the AuthorID corresponding to the one in the database based
  on the name selected by the user in the combobox.*/
  int AuthorIDForNewProject();

   /** \brief open a connection to the database.*/
  void OpenDatabaseConnection()const;

  /** \brief close the connection to the database.*/
  void CloseDatabaseConnection();

  /** \brief get the list of the existing imaging session for the selected project
  and return true if the list is not empty.*/
  bool DoesProjectHaveExistingImgSession()const;

  void BackFromNextPage()const;
  
  std::string   m_DatabaseVersion;
  QLabel*       textNewProjectName;
  QLineEdit*    lineNewProjectName;
  QLabel*       textDescription;
  QTextEdit*    lineDescription;
  QLabel*       textChoiceProject;
  QComboBox*    ChoiceProject;
  QLabel*       textChoiceAuthor;
  QComboBox*    ChoiceAuthor;
  QLabel*       textAuthor;
  QLineEdit*    lineAuthor;
  QString       OpenOrCreateProject;
  QRadioButton* OpenProjectRadioButton;
  QRadioButton* CreateProjectRadioButton;
 
  mutable QStringList m_ListProject; 
  std::map<std::string,int> m_MapAuthorIDName;
  bool ExistingImgSession;
  mutable bool LeavingPage;


protected slots:
  /** \brief  hides/shows the related QLabel,QLineEdit...and enables
  the fields where the user has to enter information to create a project*/
  void ChangeToCreateProjectDisplay(); 

  /** \brief  hides/shows the related QLabel,QLineEdit...and display
  the related information of the first existing project on the combobox */
  void ChangeToOpenProjectDisplay();

  /** \brief  display the information related to the given existing project*/
  void DisplayInfoProject(QString ProjectName);

  /** \brief Prevent the user to enter more than 200 characters in the 
  QTextEdit and return the text entered */
  std::string GetDescription();

};
#endif