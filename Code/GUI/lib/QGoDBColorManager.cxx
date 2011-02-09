/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "QGoDBColorManager.h"
#include <QColorDialog>
#include <QInputDialog>
#include "SelectQueryDatabaseHelper.h"

QGoDBColorManager::QGoDBColorManager (QWidget *iParent) :
  QGoDBNameDescEntityManager(iParent, "color", 0)
{
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QGoDBColorManager::~QGoDBColorManager()
{
  //this->m_DatabaseConnector->Close();
  //this->m_DatabaseConnector->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QGoDBColorManager::ItemColorComboboxData QGoDBColorManager::AddANewColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_NewColorData.first.clear();
  QColor col = QColorDialog::getColor(Qt::white, NULL, "Pick a new color",
                                      QColorDialog::ShowAlphaChannel);
  if ( col.isValid() )
    {
    this->m_NewColorData.second = col;
    this->m_NewColorData.first = QGoDBNameDescEntityManager::AddAnEntity(iDatabaseConnector);
    }
  return this->m_NewColorData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBColorManager::SaveNewEntityInDB()
{
  if ( !this->CheckEntityAlreadyExists< GoDBColorRow >(this->m_NewColor) )
    {
    QColor col = this->m_NewColorData.second;
    this->m_NewColor.SetField< int >( "Red", col.red() );
    this->m_NewColor.SetField< int >( "Green", col.green() );
    this->m_NewColor.SetField< int >( "Blue", col.blue() );
    this->m_NewColor.SetField< int >( "Alpha", col.alpha() );
    this->m_NewColor.SaveInDB(this->m_DatabaseConnector);
    }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoDBColorManager::ValidateName(std::string iName, std::string iDescription)
{
  this->ValidateNameTemplate< GoDBColorRow >(this->m_NewColor, iName,
                                             iDescription);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool QGoDBColorManager::DeleteEntity(vtkMySQLDatabase *iDatabaseConnector)
{
  QGoDeleteFromListDialog *Dialog = new QGoDeleteFromListDialog(
      this->GetListExistingColors(iDatabaseConnector),
      this, this->m_EntityName);

  this->m_DatabaseConnector = iDatabaseConnector;
  QObject::connect(Dialog,
                   SIGNAL( ListEntitiesToDelete(std::vector< std::string > ) ),
                   this,
                   SLOT( DeleteEntitiesFromList(std::vector< std::string > ) )
                   );

  Dialog->show();
  return Dialog->exec();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::list< QGoDBColorManager::ItemColorComboboxData >
QGoDBColorManager::GetListExistingColors(vtkMySQLDatabase *iDatabaseConnector)
{
  std::list< ItemColorComboboxData > oInfoColors;
  std::vector< std::string >         ResultsQuery  = ListAllValuesForOneColumn(
      iDatabaseConnector, "*", "color", "name");
  unsigned int i = 0;
  while ( i < ResultsQuery.size() )
    {
    ItemColorComboboxData temp;
    temp.first = ResultsQuery[i + 1];
    QColor tempColor( atoi( ResultsQuery[i + 2].c_str() ), atoi( ResultsQuery[i + 3].c_str() ),
                      atoi( ResultsQuery[i + 4].c_str() ), atoi( ResultsQuery[i + 5].c_str() ) );
    temp.second = tempColor;
    oInfoColors.push_back(temp);
    i = i + 7;
    }
  return oInfoColors;
}