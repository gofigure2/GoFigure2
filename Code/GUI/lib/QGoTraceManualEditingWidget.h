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

#ifndef __QGoTraceManualEditingWidget_h
#define __QGoTraceManualEditingWidget_h

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "QGoSelectedColorComboBox.h"
#include "QGoCollectionColorComboBox.h"
#include "qtcolorcombobox.h"

#include "ui_QGoTraceManualEditingWidget.h"

#include "QGoGUILibConfigure.h"

class QGOGUILIB_EXPORT QGoTraceManualEditingWidget :
  public QWidget,
  private Ui::QGoTraceManualEditingWidget
  {
  Q_OBJECT

public:
  explicit QGoTraceManualEditingWidget(QWidget* parent = 0);
  ~QGoTraceManualEditingWidget();

  typedef QGoColorComboBox::ItemColorComboboxData ItemColorComboboxData;

  //int GetCurrentCollectionID();
  void SetEnableTraceCollectionColorBoxes(bool Enable);
  std::string GetCurrentColor();
  std::string GetCurrentCellType();
  std::string GetCurrentSubCellType();

  void UpdateTraceAndCollection(
    std::string iTrace, std::string iCollection);

  QComboBox*                m_ChoseCellType;
  QComboBox*                m_ChoseSubCellType;
  QLabel*                   TraceName;
  QLabel*                   CollectionName;

public slots:
  void SetListCollectionID(
    std::list<ItemColorComboboxData> iListExistingID);
  void SetListExistingColors(std::list<ItemColorComboboxData> iListExistingColors);
  void SetListCellTypes(QStringList iListCellTypes);
  void SetListSubCellTypes(QStringList iListSubCellTypes);

  void CheckUserAction(QString iCellTypeText);
  void CheckUserActionSubCell(QString iSubCellTypeText);
  void SetCurrentCellType(std::string iSubCellTypeText);
  void SetCurrentSubCellType(std::string iSubCellTypeText);

signals:
  void AddANewCellType();
  void DeleteCellType();
  void AddANewSubCellType();
  void DeleteSubCellType();
  void ListSubCellTypesReady();
  void ListCellTypesReady();
  void NewCollectionActivated(ItemColorComboboxData);
  void NewSelectedColorActivated(ItemColorComboboxData);
  void NewSubCellTypeActivated(QString);
  void NewCellTypeActivated(QString);
  void NewColorToBeSaved(ItemColorComboboxData);
  void NewCollectionToBeCreated();

protected:
  //QtColorComboBox* ColorComboBox;
  QGoSelectedColorComboBox* m_SelectedColorComboBox;
  QGoCollectionColorComboBox*   m_CollectionColorComboBox;
  std::list<ItemColorComboboxData > m_DataColors;
  void SetSelectedColorComboBox();
  void SetTraceCollectionColorComboBox();
  void SetCellTypeComboBox();
  void SetSubCellTypeComboBox();
  QHBoxLayout* HLayoutForColor;
  QVBoxLayout* VLayoutForCollection;
  };
#endif
