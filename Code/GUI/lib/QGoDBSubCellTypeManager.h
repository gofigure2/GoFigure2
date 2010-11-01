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

#ifndef __QGoDBSubCellTypeManager_h
#define __QGoDBSubCellTypeManager_h

#include "QGoDBNameDescEntityManager.h"
#include "GoDBSubCellTypeRow.h"

/**
\class QGoDBSubCellTypeManager
\brief the QGoDBSubCellTypeManager manages the interactions between the user and the database
for the SubCellularType DBTable.
\ingroup DB GUI
*/
class QGoDBSubCellTypeManager:public QGoDBNameDescEntityManager
{
  Q_OBJECT
public:
  /** \todo Lydie: make a class template for celltype/subcelltype */
  explicit QGoDBSubCellTypeManager (QWidget *iParent = 0);

  ~QGoDBSubCellTypeManager()
  {}
protected:
  GoDBSubCellTypeRow m_NewSubCellType;
  //mother class method
  virtual void SaveNewEntityInDB();

protected slots:
  //mother class method
  virtual void ValidateName(std::string iName, std::string iDescription);
};
#endif
