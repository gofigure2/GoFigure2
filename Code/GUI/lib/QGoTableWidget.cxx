/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1972 $  // Revision of last commit
  Date: $Date: 2010-08-16 12:23:05 -0400 (Mon, 16 Aug 2010) $  // Date of last commit
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

#include "QGoTableWidget.h"
#include <iostream>
#include <sstream>
#include <QTableWidgetSelectionRange>
#include <QHeaderView>
#include <QSettings>
#include <QApplication>
#include <QClipboard>
#include <QToolButton>

QGoTableWidget::QGoTableWidget(QWidget* iParent) : QTableWidget(iParent)
  {
  PrevCol = -1;
  PrevOrder = -1;
  QObject::connect(this,
                   SIGNAL(cellClicked(int, int)),
                   this, SLOT(UpdateColumnsWithCheckBoxes(int, int)));
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTableWidget::
QGoTableWidget(int rows, int columns, QWidget * iParent) :
  QTableWidget(rows, columns, iParent), PrevCol(-1), PrevOrder(-1)
  {
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTableWidget::~QGoTableWidget()
  {
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::sortItems(int iColumn, Qt::SortOrder iOrder)
{
  if ((iColumn != PrevCol) && (iOrder != PrevOrder))
    {
    PrevCol = iColumn;
    PrevOrder = iOrder;
    QTableWidget::sortItems(iColumn, iOrder);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::findValueGivenColumn(int Value, QString Column)
{
  int ColumnIndex = findColumnName(Column);
  if (ColumnIndex == -1)
    {
    std::cout << "The column figureID has not been found";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    std::cout << std::endl;
    return -1;
    }
  else
    {
    for (int i = 0; i < rowCount(); i++)
      {
      if (this->item(i, ColumnIndex)->text().toInt() == Value)
        {
        return i;
        }
      }
    }
  return -1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::findColumnName(QString ColumnName)
{
  QStringList ColumnsHeader = recordHeaderNamesOrder();
  if (ColumnsHeader.isEmpty())
    {
    std::cout << "The QStringlist ColumnsHeader is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return -1;
    }
  return ColumnsHeader.indexOf(ColumnName, 0);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QGoTableWidget::recordHeaderNamesOrder()
{
  QStringList ColumnNamesOrder;
  for (int i = 0; i < this->columnCount(); i++)
    {
    ColumnNamesOrder.append(this->horizontalHeaderItem(i)->text());
    }

  return ColumnNamesOrder;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetVisibleStateForTraceID(unsigned int iTraceID,
                                            std::string iTraceName,
                                            Qt::CheckState iState,
                                            bool EmitSignal)
{
  unsigned int ColumnIndex = this->findColumnName("Show");
  int RowIndex = this->GetRowForTraceID(iTraceID,iTraceName);
  this->setVisibleStateCheckBox(this->item(RowIndex,ColumnIndex),
    iState,EmitSignal);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetCheckStateForTraceID(unsigned int iTraceID,
                                             std::string iTraceName,
                                             Qt::CheckState iState,
                                             bool EmitSignal)
{
  unsigned int ColumnIndex = this->findColumnName("");
  int RowIndex = this->GetRowForTraceID(iTraceID,iTraceName);
  this->setCheckedUncheckedStateCheckBox(
    this->item(RowIndex,ColumnIndex),iState,EmitSignal);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::GetRowForTraceID(unsigned int iTraceID,
                                     std::string iTraceName)
{
  //get the row index based on iTraceName:
  std::stringstream TraceIDName;
  TraceIDName << iTraceName;
  TraceIDName << "ID";
  int oRowIndex = this->findValueGivenColumn(iTraceID, TraceIDName.str().c_str());
  if (oRowIndex == -1)
    {
    //std::cerr << "The trace " << TraceID << "has not been found in TW";
    std::cerr << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cerr << std::endl;
    return oRowIndex;
    }
  return oRowIndex;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QGoTableWidget::ValuesForSelectedRows(QString ColumnName)
{
  QList<QTableWidgetSelectionRange> Selection = this->selectedRanges();
  int                               ColumnIndex = findColumnName(ColumnName);

  QList<QString> Values;
  for (int i = 0; i < Selection.size(); i++)
    {
    int TopRowSelected = Selection[i].topRow();
    int BottomRowSelected = Selection[i].bottomRow();

    for (int j = TopRowSelected; j < BottomRowSelected + 1; j++)
      {
      Values.append(this->item(j, ColumnIndex)->text());
      }
    }
  return Values;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::DisplayColumnNames(QString TableName,
                                           std::list<std::string> ColumnNames)
{
  size_t numberCol = ColumnNames.size();
  this->setColumnCount(static_cast<int>(numberCol));
  int i = 0;
  for (std::list<std::string>::iterator iter = ColumnNames.begin();
       iter != ColumnNames.end();
       ++iter, ++i)
    {
    QTableWidgetItem* HeaderCol = new QTableWidgetItem;
    std::string       NameHeader;
    NameHeader = *iter;

    HeaderCol->setText(NameHeader.c_str());
    if (NameHeader.empty())
      {
      HeaderCol->setToolTip(tr("Check/Uncheck %1").arg(TableName));
      }
    if (NameHeader.find("T.I.") != std::string::npos)
      {
      HeaderCol->setToolTip(tr("T.I = Total Intensity For the Channel"));
      }
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    this->setHorizontalHeaderItem(i, HeaderCol);
    this->resizeColumnToContents(i);
    }

  this->horizontalHeader()->setSortIndicatorShown(true);
  /*Need to disabled the Sorting while printing the values from the database in
  the table widget as the sorting is making trouble*/
  this->setSortingEnabled(false);
  this->horizontalHeader()->setMovable(true);

  QObject::connect(this->horizontalHeader(),
                   SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
                   this, SLOT(sortItems(int, Qt::SortOrder)));

  // QSettings settings( "MegasonLab", "Gofigure2" );
  QSettings  settings;
  QByteArray stateTableWidget = settings.value("StateTableWidget").toByteArray();
  //QTabTableName->horizontalHeader()->restoreState(stateTableWidget);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::DisplayContent(TWContainerType iTWRowContainer,
                                    std::vector<int> iIndexColorTraceRowContainer,
                                    std::vector<int> iIndexColorCollectionRowContainer,
                                    std::string iTraceName, std::string iCollectionName,
                                    std::list<std::string> iColumnNames)
{
  this->DisplayColumnNames(iTraceName.c_str(),iColumnNames);
  if (iTWRowContainer.empty())
    {
    std::cout << "The Row Container is totally empty ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    size_t NbofRows = 0;
    for (size_t i = 0; i < iTWRowContainer.size(); i++)
      {
      bool RowsCountSet = false;
      //check that the column has to be displayed in the table widget and that there are
      //some values to be displayed:
      if (iTWRowContainer[i].first.ColumnNameTableWidget != "None" && !iTWRowContainer[i].second.empty())
        {
        if (NbofRows == 0)
          {
          NbofRows = iTWRowContainer[i].second.size();
          this->setRowCount(static_cast<int>(NbofRows));
          RowsCountSet = true;
          }
        for (int j = 0; j < this->columnCount(); j++)
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if (HeaderCol == iTWRowContainer[i].first.ColumnNameTableWidget)
            {
            std::vector<std::string>::iterator iter = iTWRowContainer[i].second.begin();
            int                                k = 0;
            while (iter != iTWRowContainer[i].second.end())
              {
              //QTableWidgetItem* CellTable = new QTableWidgetItem;
              std::string       Value = *iter;
              //if (Value != "")
              if (this->CheckValueToDisplayData(Value,HeaderCol))
                {
                QTableWidgetItem* CellTable = new QTableWidgetItem;
                CellTable->setData(0, QString::fromStdString(Value).toDouble());
                CellTable->setTextAlignment(Qt::AlignCenter);
                this->setItem(k, j, CellTable);
                }
              ++iter;
              ++k;
              } //ENDWHILE
            } //ENDIF
          } //ENDFOR
        } //ENDIF
      } //ENDFOR
    SetSelectedColumn(static_cast<unsigned int>(NbofRows), 0);
    SetVisibleColumn(static_cast<unsigned int>(NbofRows), 0);
    this->SetColorForTable(iTWRowContainer,iIndexColorTraceRowContainer, iTraceName, 0);
    this->SetColorForTable(iTWRowContainer,iIndexColorCollectionRowContainer,iCollectionName, 0);
    } //ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetSelectedColumn(unsigned int iNbOfRows,
                                          unsigned int StartedRow)
{
  int indexCol = findColumnName("");
  for (unsigned int i = StartedRow; i < iNbOfRows + StartedRow; i++)
    {
    QTableWidgetItem* Checkbox = new QTableWidgetItem;
    //Checkbox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
    //  Qt::ItemIsSelectable );
    Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   // Checkbox->setFlags( Qt::ItemIsUserCheckable);
    QColor WhiteColor(Qt::white);
    Checkbox->setTextColor(WhiteColor);
    this->setCheckedUncheckedStateCheckBox(Checkbox,Qt::Unchecked,false);
    this->setItem(i, indexCol, Checkbox);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetVisibleColumn(unsigned int iNbOfRows,
                                         unsigned int StartedRow)
{
  int indexCol = findColumnName("Show");
  for (unsigned int i = StartedRow; i < iNbOfRows + StartedRow; i++)
    {
    QTableWidgetItem* Checkbox = new QTableWidgetItem;
    //Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QColor WhiteColor(Qt::white);
    Checkbox->setTextColor(WhiteColor);
    this->setItem(i, indexCol, Checkbox);
    this->setVisibleStateCheckBox(Checkbox,Qt::Checked,false);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetColorForTable(TWContainerType iTWRowContainer,
                                      std::vector<int> iIndexColorRowContainer,
                                      std::string NameGroupColor, unsigned int StartRow)
{
  //for the collection color:
  std::string ColumnNameID = NameGroupColor;
  ColumnNameID += "ID";
  int indexGroupIDInTableWidget = findColumnName(ColumnNameID.c_str());
  for (unsigned int i = StartRow; i < iTWRowContainer[1].second.size() + StartRow; i++)
    {
    QColor Color;
    QColor TextColor;
    int Alpha = atoi(iTWRowContainer[iIndexColorRowContainer[3]].second[i - StartRow].c_str());
    if (iTWRowContainer[indexGroupIDInTableWidget].second[i - StartRow] == "0" || Alpha == 0)
      {
      Color.setRgb(255, 255, 255, 255);
      int rgb = 255 - (255 * 3) / 3;
      TextColor.setRgb(rgb, rgb, rgb, 255);
      }
    else
      {
      int Red   = atoi(iTWRowContainer[iIndexColorRowContainer[0]].second[i - StartRow].c_str());
      int Green = atoi(iTWRowContainer[iIndexColorRowContainer[1]].second[i - StartRow].c_str());
      int Blue  = atoi(iTWRowContainer[iIndexColorRowContainer[2]].second[i - StartRow].c_str());
      Color.setRgb(Red, Green, Blue, Alpha);
      int rgb = 255 - (Red + Green + Blue) / 3;
      TextColor.setRgb(rgb, rgb, rgb, 255);
      }
    this->item(i, indexGroupIDInTableWidget)->setBackgroundColor(Color);
    this->item(i, indexGroupIDInTableWidget)->setTextColor(TextColor);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::InsertNewRow(TWContainerType iTWRowContainer,
                                  std::vector<int> iIndexColorTraceRowContainer,
                                  std::vector<int> iIndexColorCollectionRowContainer,
                                  std::string TraceName, std::string CollectionName)
{
  if (iTWRowContainer.size() == 0 || iTWRowContainer[1].second.size() != 1)
    {
    std::cout << "The New Trace Row Container is totally empty or there is more than 1 trace in it";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    int NewRow = this->rowCount() + 1;
    int NbRow = NewRow;
    this->setRowCount(NbRow);
    for (unsigned int i = 0; i < iTWRowContainer.size(); i++)
      {
      if (iTWRowContainer[i].first.ColumnNameTableWidget != "None" && !iTWRowContainer[i].second.empty())
        {
        for (int j = 0; j < this->columnCount(); j++)
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if (HeaderCol == iTWRowContainer[i].first.ColumnNameTableWidget)
            {
            //QTableWidgetItem* CellTable = new QTableWidgetItem;
            std::string       Value = iTWRowContainer[i].second[0];
            if (this->CheckValueToDisplayData(Value,HeaderCol))//(Value != "")
              {
              QTableWidgetItem* CellTable = new QTableWidgetItem;
              CellTable->setData(0,QString::fromStdString(Value).toDouble());
              CellTable->setTextAlignment(Qt::AlignCenter);
              this->setItem(NewRow - 1, j, CellTable);
              }
            } //ENDIF
          } //ENDFOR
        } //ENDIF
      } //ENDFOR
    SetSelectedColumn(1, NewRow - 1);
    SetVisibleColumn(1, NewRow - 1);
    this->SetColorForTable(iTWRowContainer, iIndexColorTraceRowContainer,TraceName, NewRow - 1);
    this->SetColorForTable(iTWRowContainer, iIndexColorCollectionRowContainer,CollectionName, NewRow - 1);
    } //ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool QGoTableWidget::CheckValueToDisplayData(std::string iValue,std::string HeaderCol)
{
  if(iValue == "")
    {
    return false;
    }
  if(iValue == "-1" && HeaderCol.compare("T.I.Channel")>0)
    {
    return false;
    }
  return true;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::UpdateRow(TWContainerType iTWRowContainer,
                               std::vector<int> iIndexColorTraceRowContainer,
                               std::vector<int> iIndexColorCollectionRowContainer,
                               std::string iTraceName, std::string iCollectionName,
                               int iTraceID)
{
  if (iTWRowContainer.size() == 0 || iTWRowContainer[1].second.size() != 1)
    {
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    std::cout << "The Update Trace Row Container is totally empty or there is more than 1 trace in it";
    std::cout << std::endl;
    }
  else
    {
    QString TraceNameID = QString("%1ID").arg(iTraceName.c_str());
    int     IndexUpdateRow = this->findValueGivenColumn(iTraceID, TraceNameID);
    if (IndexUpdateRow != -1)
      {
      QTableWidgetItem* t_item = NULL;

      for (unsigned int i = 0; i < iTWRowContainer.size(); i++)
        {
        if (iTWRowContainer[i].first.ColumnNameTableWidget != "None" &&
            !iTWRowContainer[i].second.empty())
          {
          for (int j = 0; j < this->columnCount(); j++)
            {
            std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
            if (HeaderCol == iTWRowContainer[i].first.ColumnNameTableWidget)
              {
              std::string Value = iTWRowContainer[i].second[0];

              t_item = this->item(IndexUpdateRow, j);

              if( t_item )
                {
                t_item->setData(0, QString::fromStdString(Value).toDouble());
                }
              else
                {
                  if (this->CheckValueToDisplayData(Value,HeaderCol))
                  {
                  QTableWidgetItem* CellTable = new QTableWidgetItem;
                  CellTable->setData(0,QString::fromStdString(Value).toDouble());
                  CellTable->setTextAlignment(Qt::AlignCenter);
                  this->setItem(IndexUpdateRow, j, CellTable);
                  }
                }
              } //ENDIF
            } //ENDFOR
          } //ENDIF
        } //ENDFOR
      this->SetColorForTable(
        iTWRowContainer,iIndexColorTraceRowContainer,iTraceName, IndexUpdateRow);
      this->SetColorForTable(
        iTWRowContainer,iIndexColorCollectionRowContainer,iCollectionName, IndexUpdateRow);
      }
    else
      {
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      std::cout << "the row does't exist";
      std::cout << std::endl;
      }
    } //ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::UpdateIDs(unsigned int iNewCollectionID,
                                  std::string iCollectionIDName, QColor ColorNewCollection,
                                  std::string TraceIDName, std::list<int> TraceIDToUpdate)
{
  int                      IndexCollectionID = this->findColumnName(iCollectionIDName.c_str());
  std::list<int>::iterator iter = TraceIDToUpdate.begin();
  while (iter != TraceIDToUpdate.end())
    {
    int RowIndex = this->findValueGivenColumn(*iter, TraceIDName.c_str());
    this->item(RowIndex, IndexCollectionID)->
    setText(ConvertToString<unsigned int>(iNewCollectionID).c_str());
    this->item(RowIndex, IndexCollectionID)->setBackgroundColor(ColorNewCollection);
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::DeleteCheckedRows(std::string iTraceNameID,
                                        std::list<unsigned int> iTraceIDs)
{
  std::list<unsigned int>::iterator iter = iTraceIDs.begin();
  while (iter != iTraceIDs.end())
    {
    int RowToDelete = this->findValueGivenColumn(*iter,iTraceNameID.c_str());
    removeRow(RowToDelete);
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::UpdateColumnsWithCheckBoxes(int Row, int Column)
{
  if (this->horizontalHeaderItem(Column)->text() == "")
    {
    //force the checkbox to change state, even if the user didn't click
    //directly in the checkbox but in the cell containing it:
    if (this->item(Row, Column)->checkState() == 0)
      {
      this->setCheckedUncheckedStateCheckBox(this->item(Row,Column),Qt::Checked,true);
      }
    else
      {
      this->setCheckedUncheckedStateCheckBox(this->item(Row,Column),Qt::Unchecked,true);
      }
    }
  if (this->horizontalHeaderItem(Column)->text() == "Show")
    {
    if (this->item(Row, Column)->checkState() == 0)
      {
      this->setVisibleStateCheckBox(this->item(Row,Column),Qt::Checked,true);
      }
    else
      {
      this->setVisibleStateCheckBox(this->item(Row,Column),Qt::Unchecked,true);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::CopySelection()
{
  //Get only the first selected Range:
  QList<QTableWidgetSelectionRange> SelectedRanges = this->selectedRanges();
  //QTableWidgetSelectionRange range = SelectedRanges.first();
  QString str;
  for (int k = 0; k < SelectedRanges.size(); k++)
    {
    QTableWidgetSelectionRange range = SelectedRanges.at(k);
    this->PrepareRangeToCopy(range, str);
    str += "\n";
    }
  QApplication::clipboard()->setText(str);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::CopyTable()
{
  //first, copy the column names:
  QStringList ColumnsNames = this->recordHeaderNamesOrder();
  QString     str;
  str += "IsSelected";
  str += "\t";
  for (int i = 1; i < ColumnsNames.size() - 1; i++)
    {
    str += ColumnsNames.at(i);
    str += "\t";
    }
  str += ColumnsNames.at(ColumnsNames.size() - 1);
  str += "\n";
  //second, get the range for the total selection:
  QTableWidgetSelectionRange RangeForAllTableSelected(
    0, 0, this->rowCount() - 1, this->columnCount() - 1);
  //third, copy the range:
  this->PrepareRangeToCopy(RangeForAllTableSelected, str);
  QApplication::clipboard()->setText(str);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::PrepareRangeToCopy(QTableWidgetSelectionRange Range,
                                           QString& str)
{
  QTableWidgetItem* t_item = NULL;

  for (int i = 0; i < Range.rowCount(); ++i)
    {
    if (i > 0)
      {
      str += "\n";
      }
    for (int j = 0; j < Range.columnCount() - 1; ++j)
      {
      int k = Range.leftColumn() + j;
      if (k == 0)
        {
        //for the selected column:
        if (item(i, 0)->checkState() == 0)
          {
          str += "No \t";
          }
        else
          {
          str += "Yes \t";
          }
        }
      else
        {
        t_item = this->item(Range.topRow() + i, Range.leftColumn() + j);
        if( t_item )
          {
          str += t_item->text();
          }
        else
          {
          str += "*";
          }
        str += "\t";
        }
      }
    t_item =
        this->item(Range.topRow() + i, Range.leftColumn() + Range.columnCount() - 1);

    if( t_item )
      {
      str += t_item->text();
      }
    else
      {
      str += "*";
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::CheckSelectedRows(std::string iTraceName,
                                          std::string iTraceNameID)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str());
  if (!ListSelectedTracesID.empty())
    {
    for (int i = 0; i < ListSelectedTracesID.size(); i++)
      {
      this->SetCheckStateForTraceID(ListSelectedTracesID.at(i).toUInt(),
                                    iTraceName,Qt::Checked);
      }
    }
  else
    {
    std::cout << "The list of selected Traces ID is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::UncheckSelectedRows(std::string iTraceName,
                                            std::string iTraceNameID)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str());
  if (!ListSelectedTracesID.empty())
    {
    for (int i = 0; i < ListSelectedTracesID.size(); i++)
      {
      this->SetCheckStateForTraceID(ListSelectedTracesID.at(i).toUInt(),
                                    iTraceName,Qt::Unchecked);
      }
    }
  else
    {
    std::cout << "The list of selected Traces ID is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::ShowSelectedRows(std::string iTraceName,
                                         std::string iTraceNameID)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str());
  if (!ListSelectedTracesID.empty())
    {
    for (int i = 0; i < ListSelectedTracesID.size(); i++)
      {
      this->SetVisibleStateForTraceID(ListSelectedTracesID.at(i).toUInt(),iTraceName,
                                   Qt::Checked);
      }
    }
  else
    {
    std::cout << "The list of selected Traces ID is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::HideSelectedRows(std::string iTraceName,
                                         std::string iTraceNameID)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str());
  if (!ListSelectedTracesID.empty())
    {
    for (int i = 0; i < ListSelectedTracesID.size(); i++)
      {
      this->SetVisibleStateForTraceID(ListSelectedTracesID.at(i).toUInt(),iTraceName,
                                   Qt::Unchecked);
      }
    }
  else
    {
    std::cout << "The list of selected Traces ID is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::AddValuesForID(std::vector<std::string> iColumnsNames,
                                       std::vector<std::string> iValues, unsigned int iID,
                                       std::string iColumnNameForTraceID)
{
  int RowIndex = this->findValueGivenColumn(iID, iColumnNameForTraceID.c_str());
  if (RowIndex != -1)
    {
    for (unsigned int i = 0; i < iColumnsNames.size(); i++)
      {
      int ColumnIndex = this->findColumnName(iColumnsNames.at(i).c_str());
      if (ColumnIndex != -1)
        {
        QTableWidgetItem* CellTable = new QTableWidgetItem;
        CellTable->setData(0, QString::fromStdString(iValues.at(i)).toDouble());
//QString::fromStdString( iValues.at(i)));
        CellTable->setTextAlignment(Qt::AlignCenter);
        this->setItem(RowIndex, ColumnIndex, CellTable);
        }
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow QGoTableWidget::GetCoordinateCenterBoundingBox(
  unsigned int iTraceID,std::string iTraceName)
{
  GoDBCoordinateRow CenterCoord;
  int RowIndex = this->GetRowForTraceID(iTraceID,iTraceName);
  if (RowIndex != -1)
    {
    CenterCoord.SetField("TCoord",
                             this->GetValueForItem("TimePoint", RowIndex));
    CenterCoord.SetField("XCoord",
                             this->GetMeanValue("XMax", "XMin", RowIndex));
    CenterCoord.SetField("YCoord",
                             this->GetMeanValue("YMax", "YMin", RowIndex));
    CenterCoord.SetField("ZCoord",
                             this->GetMeanValue("ZMax", "ZMin", RowIndex));
    }
  return CenterCoord;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::GetValueForItem(std::string iColumnName, int iRowIndex)
{
  return
    this->item(
      iRowIndex, this->findColumnName(iColumnName.c_str()))->text().toInt();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoTableWidget::GetMeanValue(std::string iColumnNameOne,
                                            std::string iColumnNameTwo, unsigned int iRowIndex)
{
  int ValueOne = this->GetValueForItem(iColumnNameOne, iRowIndex);
  int ValueTwo = this->GetValueForItem(iColumnNameTwo, iRowIndex);
  int meanValue = (ValueOne + ValueTwo) / 2;
  return ConvertToString<int>(meanValue);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::setCheckedUncheckedStateCheckBox(QTableWidgetItem * iItem,
                                              Qt::CheckState iState,
                                              bool EmitSignal)
{
  if (this->setCheckStateCheckBox(iItem,iState) && EmitSignal)
    {
    int Row = iItem->row();
    emit CheckedRowsChanged(this->item(Row, 1)->text().toInt());
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::setVisibleStateCheckBox(QTableWidgetItem * iItem,
                                              Qt::CheckState iState,
                                              bool EmitSignal)
{
  if(this->setCheckStateCheckBox(iItem,iState))
    {
    QIcon Icon;
    if (iState == Qt::Checked)
      {
      Icon.addPixmap(QPixmap(QString::fromUtf8(":/fig/EyeIcon.png")),
                        QIcon::Normal, QIcon::Off);
      }
  else
      {
      Icon.addPixmap(QPixmap(QString::fromUtf8(":/fig/BlankIcon.png")),
                           QIcon::Normal, QIcon::Off);
      }
     if (EmitSignal)
      {
      int Row = iItem->row();
      VisibleRowsChanged(this->item(Row, 1)->text().toInt());
      }
     iItem->setIcon(Icon);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool QGoTableWidget::setCheckStateCheckBox(QTableWidgetItem* iItem,
                                           Qt::CheckState iState)
{
  bool oModification = false;
  if(iState == Qt::Checked)
    {
    //if the row is already checked, no need to do anything:
    if (iItem->checkState() != 2)
       {
       iItem->setCheckState(Qt::Checked);
       iItem->setText("1");
       oModification = true;
       }
    }
  else
    {
    //if the row is already unchecked, no need to do anything:
    if (iItem->checkState() != Qt::Unchecked)
      {
      iItem->setCheckState(Qt::Unchecked);
      iItem->setText("0");
      oModification = true;
      }
    }
  return oModification;
}
