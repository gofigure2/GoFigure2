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
#include "QGoDBCellTypeManager.h"
#include "GoDBCellTypeRow.h"
#include <QMessageBox>

QGoDBCellTypeManager::QGoDBCellTypeManager (QWidget* iParent) :
  QGoDBEntityManager(iParent, "celltype", 0)
  {
  }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoDBCellTypeManager::SaveNewEntityInDB(std::string iName, std::string iDescription)
{
  //this->m_NewCellType.SetField("Name",this->m_NameDescDialog->GetInputTextForName());
  //this->m_NewCellType.SetField("Description",
                               //this->m_NameDescDialog->GetInputTextForDescription());
  this->m_NewCellType.SetField("Description",iDescription);

  std::string CellTypeName = this->m_NewCellType.GetMapValue("Name");
  if (this->m_NewCellType.DoesThisEntityAlreadyExists(
        this->m_DatabaseConnectorForNewEntity, CellTypeName) != -1)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("This bookmark already exists, its name is: ' %1 ' ").arg(CellTypeName.c_str()));
    msgBox.exec();
    }
  else
    {
    this->m_NewCellType.SaveInDB(this->m_DatabaseConnectorForNewEntity);
    this->m_NameNewEntity = CellTypeName;
    //emit ListEntitiesChanged();
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoDBCellTypeManager::ValidateName(std::string iName, std::string iDescription)
{
  //this->m_NewCellType.SetField("Name",this->m_NameDescDialog->GetInputTextForName());
  this->m_NewCellType.SetField("Name",iName);
  //if (this->DoesThisBookmarkNameAlreadyExistsInTheDB(
  //this->m_DatabaseConnectorForNewBkmrk,iName))
  if (this->m_NewCellType.DoesThisNameAlreadyExists(
        this->m_DatabaseConnectorForNewEntity) != -1)
    {
    this->m_NameDescDialog->NameAlreadyExists();
    }
  else
    {
    this->m_NameDescDialog->accept();
    this->SaveNewEntityInDB(iName,iDescription);
    }
}
