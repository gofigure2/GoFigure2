/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 590 $  // Revision of last commit
  Date: $Date: 2009-08-21 17:48:12 -0400 (Fri, 21 Aug 2009) $  // Date of last commit
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
#include "GoDBCollectionOfTraces.h"
#include "CreateDataBaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include <QStringList>
#include <QString>
#include <string>


GoDBCollectionOfTraces::GoDBCollectionOfTraces()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::GoDBCollectionOfTraces( QString iCollectionName,
  QString iCollectionIDName,QString iTracesName, QString iTracesIDName )
{
  m_CollectionName = iCollectionName;
  m_CollectionIDName = iCollectionIDName;
  m_TracesName = iTracesName;
  m_TracesIDName = iTracesIDName;

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::~GoDBCollectionOfTraces()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::SetDatabaseVariables( QString Server,
  QString User, QString Password, QString NameDB)
{
  m_Server = Server;
  m_User = User;
  m_Password = Password;
  m_NameDB = NameDB;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::DeleteTraces(QStringList TracesToDelete)
{
  for (int i = 0; i<TracesToDelete.size();i++)
    {
    std::string ID = TracesToDelete.at(i).toStdString();
    DeleteRow(m_Server.toStdString(), m_User.toStdString(), m_Password.toStdString(),
      m_NameDB.toStdString(),m_TracesName.toStdString(), m_TracesIDName.toStdString(), ID);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::AddSelectedTracesToCollection(QStringList ListSelectedTraces,
  int newCollectionID)
{
  std::string newCollectionIDstring = ConvertToString<int>(newCollectionID);
  for (int i=0; i<ListSelectedTraces.size();i++)
    {
    UpdateValueInDB(m_Server.toStdString(), m_User.toStdString(),
      m_Password.toStdString(), m_NameDB.toStdString(),
      m_TracesName.toStdString(), m_CollectionIDName.toStdString(), newCollectionIDstring,
      m_TracesIDName.toStdString(), ListSelectedTraces.at(i).toStdString());
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList GoDBCollectionOfTraces::ListCollectionID()
 {
   QStringList ListIDs; 
   std::vector<std::string> vectListIDs = ListAllValuesForOneColumn(m_Server.toStdString(), m_User.toStdString(),
   m_Password.toStdString(), m_NameDB.toStdString(),m_CollectionIDName.toStdString(),
   m_CollectionName.toStdString());
 
   for( unsigned int i = 0; i < vectListIDs.size(); ++i )
     {
     ListIDs.append( vectListIDs[i].c_str( ) );
     }
 
   return ListIDs;
 }