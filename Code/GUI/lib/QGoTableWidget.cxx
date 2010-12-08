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
#include <QMessageBox>

QGoTableWidget::QGoTableWidget(QWidget *iParent):QTableWidget(iParent)
{
  PrevCol = -1;
  PrevOrder = -1;
  QObject::connect( this,
                    SIGNAL( cellClicked(int, int) ),
                    this, SLOT( UpdateColumnsWithCheckBoxes(int, int) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTableWidget::QGoTableWidget(int rows, int columns, QWidget *iParent):
  QTableWidget(rows, columns, iParent), PrevCol(-1), PrevOrder(-1)
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTableWidget::~QGoTableWidget()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::sortItems(int iColumn, Qt::SortOrder iOrder)
{
  if ( ( iColumn != PrevCol ) && ( iOrder != PrevOrder ) )
    {
    PrevCol = iColumn;
    PrevOrder = iOrder;
    QTableWidget::sortItems(iColumn, iOrder);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::findValueGivenColumn(int iValue, QString iColumn)
{
  int ColumnIndex = findColumnName(iColumn);

  if ( ColumnIndex == -1 )
    {
    std::cout << "The column figureID has not been found";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    std::cout << std::endl;
    return -1;
    }
  else
    {
    for ( int i = 0; i < rowCount(); i++ )
      {
      if ( this->item(i, ColumnIndex)->text().toInt() == iValue )
        {
        return i;
        }
      }
    }
  return -1;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::findColumnName(QString iColumnName)
{
  QStringList ColumnsHeader = recordHeaderNamesOrder();

  if ( ColumnsHeader.isEmpty() )
    {
    std::cout << "The QStringlist ColumnsHeader is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return -1;
    }
  return ColumnsHeader.indexOf(iColumnName, 0);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QGoTableWidget::recordHeaderNamesOrder()
{
  QStringList ColumnNamesOrder;

  for ( int i = 0; i < this->columnCount(); i++ )
    {
    ColumnNamesOrder.append( this->horizontalHeaderItem(i)->text() );
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
  int          RowIndex = this->GetRowForTraceID(iTraceID, iTraceName);

  this->setVisibleStateCheckBox(this->item(RowIndex, ColumnIndex),
                                iState, EmitSignal);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void  QGoTableWidget::SetVisibleStateForListTraceIDs(
  std::list<unsigned int> iListTraceIDs,Qt::CheckState iState,
  std::string iTraceName)
{
  std::list<unsigned int>::iterator iter = iListTraceIDs.begin();
  while (iter != iListTraceIDs.end())
  {
  this->SetVisibleStateForTraceID(*iter,iTraceName,iState,false);
  iter++;
  }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetCheckStateForTraceID(unsigned int iTraceID,
                                             std::string iTraceName,
                                             Qt::CheckState iState,
                                             bool EmitSignal)
{
  unsigned int ColumnIndex = this->findColumnName("");
  int          RowIndex = this->GetRowForTraceID(iTraceID, iTraceName);

  this->setCheckedUncheckedStateCheckBox(
    this->item(RowIndex, ColumnIndex), iState, EmitSignal);
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
  int oRowIndex = this->findValueGivenColumn( iTraceID, TraceIDName.str().c_str() );
  if ( oRowIndex == -1 )
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
QStringList QGoTableWidget::ValuesForSelectedRows(QString iColumnName)
{
  QList< QTableWidgetSelectionRange > Selection = this->selectedRanges();
  int                                 ColumnIndex = findColumnName(iColumnName);

  QList< QString > Values;
  for ( int i = 0; i < Selection.size(); i++ )
    {
    int TopRowSelected = Selection[i].topRow();
    int BottomRowSelected = Selection[i].bottomRow();

    for ( int j = TopRowSelected; j < BottomRowSelected + 1; j++ )
      {
      Values.append( this->item(j, ColumnIndex)->text() );
      }
    }
  return Values;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::DisplayColumnNames(QString iTableName,
                                        std::list< std::string > iColumnNames)
{
  size_t numberCol = iColumnNames.size();

  this->setColumnCount( static_cast< int >( numberCol ) );
  int i = 0;
  for ( std::list< std::string >::iterator iter = iColumnNames.begin();
        iter != iColumnNames.end();
        ++iter, ++i )
    {
    QTableWidgetItem *HeaderCol = new QTableWidgetItem;
    std::string       NameHeader;
    NameHeader = *iter;

    HeaderCol->setText( NameHeader.c_str() );
    if ( NameHeader.empty() )
      {
      HeaderCol->setToolTip( tr("Check/Uncheck %1").arg(iTableName) );
      }
    if ( NameHeader.find("T.I.") != std::string::npos )
      {
      HeaderCol->setToolTip( tr("T.I = Total Intensity For the Channel") );
      }
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    this->setHorizontalHeaderItem(i, HeaderCol);
    this->resizeColumnToContents(i);
    }

  this->horizontalHeader()->setSortIndicatorShown(true);
  //Need to disabled the Sorting while printing the values from the database in
  // the table widget as the sorting is making trouble
  this->setSortingEnabled(false);
  this->horizontalHeader()->setMovable(true);

  QObject::connect( this->horizontalHeader(),
                    SIGNAL( sortIndicatorChanged(int, Qt::SortOrder) ),
                    this, SLOT( sortItems(int, Qt::SortOrder) ) );

  // QSettings settings( "MegasonLab", "Gofigure2" );
  QSettings  settings;
  QByteArray stateTableWidget = settings.value("StateTableWidget").toByteArray();
  //QTabTableName->horizontalHeader()->restoreState(stateTableWidget);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::DisplayContent(TWContainerType iTWRowContainer,
                                    std::vector< int > iIndexColorTraceRowContainer,
                                    std::vector< int > iIndexColorCollectionRowContainer,
                                    std::string iTraceName, std::string iCollectionName,
                                    std::list< std::string > iColumnNames,
                                    Qt::CheckState iState,
                                    int iIndexShowColumn)
{
  this->setSortingEnabled(false);
  this->DisplayColumnNames(iTraceName.c_str(), iColumnNames);
  if ( iTWRowContainer.empty() )
    {
    std::cout << "The Row Container is totally empty ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    size_t NbofRows = 0;
    for ( size_t i = 0; i < iTWRowContainer.size(); i++ )
      {
      bool RowsCountSet = false;
      //check that the column has to be displayed in the table widget and that
      // there are
      //some values to be displayed:
      if ( iTWRowContainer[i].first.ColumnNameTableWidget != "None" && !iTWRowContainer[i].second.empty() )
        {
        if ( NbofRows == 0 )
          {
          NbofRows = iTWRowContainer[i].second.size();
          this->setRowCount( static_cast< int >( NbofRows ) );
          RowsCountSet = true;
          }
        for ( int j = 0; j < this->columnCount(); j++ )
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if ( HeaderCol == iTWRowContainer[i].first.ColumnNameTableWidget )
            {
            std::vector< std::string >::iterator iter = iTWRowContainer[i].second.begin();
            int                                  k = 0;
            while ( iter != iTWRowContainer[i].second.end() )
              {
              std::string Value = *iter;
              if ( this->CheckValueToDisplayData(Value, HeaderCol) )
                {
                QTableWidgetItem *CellTable = new QTableWidgetItem;
                if(iTWRowContainer[i].first.TypeName == "string")
                  {
                  CellTable->setData( 0, QString::fromStdString(Value) );
                  }
                else
                  {
                  CellTable->setData( 0, QString::fromStdString(Value).toDouble() );
                  }
                CellTable->setTextAlignment(Qt::AlignCenter);
                this->setItem(k, j, CellTable);
                }
              ++iter;
              ++k;
              } //ENDWHILE
            }   //ENDIF
          }     //ENDFOR
        }       //ENDIF
      }         //ENDFOR
    SetSelectedColumn(static_cast< unsigned int >( NbofRows ), 0);
    if (iIndexShowColumn == 0 || iTWRowContainer[iIndexShowColumn].second.empty()) //track and lineage
      {
      this->SetVisibleColumn(static_cast< unsigned int >( NbofRows ),0,iState);
      }
    else //mesh and contour
      {
      this->SetVisibleColumn(static_cast< unsigned int >( NbofRows ), 0,iTWRowContainer[iIndexShowColumn].second);
      }

    this->SetColorForTable(iTWRowContainer, iIndexColorTraceRowContainer, iTraceName, 0);
    this->SetColorForTable(iTWRowContainer, iIndexColorCollectionRowContainer, iCollectionName, 0);
    } //ENDELSE
  this->setSortingEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetSelectedColumn(unsigned int iNbOfRows,
                                       unsigned int iStartedRow)
{
  int indexCol = findColumnName("");

  for ( unsigned int i = iStartedRow; i < iNbOfRows + iStartedRow; i++ )
    {
    QTableWidgetItem *Checkbox = new QTableWidgetItem;
    //Checkbox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
    //  Qt::ItemIsSelectable );
    Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    // Checkbox->setFlags( Qt::ItemIsUserCheckable);
    QColor WhiteColor(Qt::white);
    Checkbox->setTextColor(WhiteColor);
    this->setCheckedUncheckedStateCheckBox(Checkbox, Qt::Unchecked, false);
    this->setItem(i, indexCol, Checkbox);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetVisibleColumn(unsigned int iNbOfRows,
                                      unsigned int iStartedRow,
                                      Qt::CheckState iState)
{
  int indexCol = findColumnName("Show");

  for ( unsigned int i = iStartedRow; i < iNbOfRows + iStartedRow; i++ )
    {
    QTableWidgetItem *Checkbox = new QTableWidgetItem;
    //Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable |
    // Qt::ItemIsUserCheckable);
    Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QColor WhiteColor(Qt::white);
    Checkbox->setTextColor(WhiteColor);
    this->setItem(i, indexCol, Checkbox);
    //this->setVisibleStateCheckBox(Checkbox, Qt::Checked, false);
    this->setVisibleStateCheckBox(Checkbox, iState, false);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetVisibleColumn(unsigned int iNbOfRows,
                                      unsigned int StartedRow,
                                      std::vector<std::string> iListState)
{
  int indexCol = findColumnName("Show");
  if(iNbOfRows != iListState.size())
    {
    std::cout<<"can not fill the isVisible column ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }

  for ( unsigned int i = StartedRow; i < iNbOfRows + StartedRow; i++ )
    {
    QTableWidgetItem *Checkbox = new QTableWidgetItem;
    //Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable |
    // Qt::ItemIsUserCheckable);
    Checkbox->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QColor WhiteColor(Qt::white);
    Checkbox->setTextColor(WhiteColor);
    this->setItem(i, indexCol, Checkbox);
    if(iListState.at(i) == "true")
      {
      this->setVisibleStateCheckBox(Checkbox, Qt::Checked, false);
      }
    else
      {
        this->setVisibleStateCheckBox(Checkbox, Qt::Unchecked, false);
      }
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::SetColorForTable(TWContainerType iTWRowContainer,
                                      std::vector< int > iIndexColorRowContainer,
                                      std::string iNameGroupColor, unsigned int iStartRow)
{
  //for the collection color:
  std::string ColumnNameID = iNameGroupColor;

  ColumnNameID += "ID";
  int indexGroupIDInTableWidget = findColumnName( ColumnNameID.c_str() );
  for ( unsigned int i = iStartRow; i < iTWRowContainer[1].second.size() + iStartRow; i++ )
    {
    QColor Color;
    QColor TextColor;
    int    Alpha = atoi( iTWRowContainer[iIndexColorRowContainer[3]].second[i - iStartRow].c_str() );
    if ( iTWRowContainer[indexGroupIDInTableWidget].second[i - iStartRow] == "0" || Alpha == 0 )
      {
      Color.setRgb(255, 255, 255, 255);
      int rgb = 255 - ( 255 * 3 ) / 3;
      TextColor.setRgb(rgb, rgb, rgb, 255);
      }
    else
      {
      int Red   = atoi( iTWRowContainer[iIndexColorRowContainer[0]].second[i - iStartRow].c_str() );
      int Green = atoi( iTWRowContainer[iIndexColorRowContainer[1]].second[i - iStartRow].c_str() );
      int Blue  = atoi( iTWRowContainer[iIndexColorRowContainer[2]].second[i - iStartRow].c_str() );
      Color.setRgb(Red, Green, Blue, Alpha);
      int rgb = 255 - ( Red + Green + Blue ) / 3;
      TextColor.setRgb(rgb, rgb, rgb, 255);
      }
    this->item(i, indexGroupIDInTableWidget)->setBackgroundColor(Color);
    this->item(i, indexGroupIDInTableWidget)->setTextColor(TextColor);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::InsertNewRow(TWContainerType iTWRowContainer,
                                  std::vector< int > iIndexColorTraceRowContainer,
                                  std::vector< int > iIndexColorCollectionRowContainer,
                                  std::string TraceName, std::string CollectionName)
{
  this->setSortingEnabled(false);
  if ( iTWRowContainer.size() == 0 || iTWRowContainer[1].second.size() != 1 )
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
    for ( unsigned int i = 0; i < iTWRowContainer.size(); i++ )
      {
      if ( iTWRowContainer[i].first.ColumnNameTableWidget != "None" && !iTWRowContainer[i].second.empty() )
        {
        for ( int j = 0; j < this->columnCount(); j++ )
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if ( HeaderCol == iTWRowContainer[i].first.ColumnNameTableWidget )
            {
            //QTableWidgetItem* CellTable = new QTableWidgetItem;
            std::string Value = iTWRowContainer[i].second[0];
            if ( this->CheckValueToDisplayData(Value, HeaderCol) ) //(Value !=
                                                                   // "")
              {
              QTableWidgetItem *CellTable = new QTableWidgetItem;
              //CellTable->setData( 0, QString::fromStdString(Value).toDouble() );
              if(iTWRowContainer[i].first.TypeName == "string")
                    {
                    CellTable->setData( 0, QString::fromStdString(Value) );
                    }
                  else
                    {
                    CellTable->setData( 0, QString::fromStdString(Value).toDouble() );
                    }
              CellTable->setTextAlignment(Qt::AlignCenter);
              this->setItem(NewRow - 1, j, CellTable);
              }
            } //ENDIF
          }   //ENDFOR
        }     //ENDIF
      }       //ENDFOR
    SetSelectedColumn(1, NewRow - 1);
    SetVisibleColumn(1, NewRow - 1);
    this->SetColorForTable(iTWRowContainer, iIndexColorTraceRowContainer, TraceName, NewRow - 1);
    this->SetColorForTable(iTWRowContainer, iIndexColorCollectionRowContainer, CollectionName, NewRow - 1);
    } //ENDELSE
  this->setSortingEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool QGoTableWidget::CheckValueToDisplayData(std::string iValue, std::string iHeaderCol)
{
  if ( iValue == "" )
    {
    return false;
    }
  if ( iValue == "-1" && iHeaderCol.compare("T.I.Channel") > 0 )
    {
    return false;
    }
  return true;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::UpdateRow(TWContainerType iTWRowContainer,
                               std::vector< int > iIndexColorTraceRowContainer,
                               std::vector< int > iIndexColorCollectionRowContainer,
                               std::string iTraceName, std::string iCollectionName,
                               int iTraceID)
{
  this->setSortingEnabled(false);
  if ( iTWRowContainer.size() == 0 || iTWRowContainer[1].second.size() != 1 )
    {
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    std::cout << "The Update Trace Row Container is totally empty or there is more than 1 trace in it";
    std::cout << std::endl;
    }
  else
    {
    QString TraceNameID = QString("%1ID").arg( iTraceName.c_str() );
    int     IndexUpdateRow = this->findValueGivenColumn(iTraceID, TraceNameID);
    if ( IndexUpdateRow != -1 )
      {
      QTableWidgetItem *t_item = NULL;

      for ( unsigned int i = 0; i < iTWRowContainer.size(); i++ )
        {
        if ( iTWRowContainer[i].first.ColumnNameTableWidget != "None"
             && !iTWRowContainer[i].second.empty() )
          {
          for ( int j = 0; j < this->columnCount(); j++ )
            {
            std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
            if ( HeaderCol == iTWRowContainer[i].first.ColumnNameTableWidget )
              {
              std::string Value = iTWRowContainer[i].second[0];

              t_item = this->item(IndexUpdateRow, j);

              if ( t_item )
                {
                //t_item->setData( 0, QString::fromStdString(Value).toDouble() );
                if(iTWRowContainer[i].first.TypeName == "string")
                  {
                  t_item->setData( 0, QString::fromStdString(Value) );
                  }
                else
                  {
                  t_item->setData( 0, QString::fromStdString(Value).toDouble() );
                  }
                }
              else
                {
                if ( this->CheckValueToDisplayData(Value, HeaderCol) )
                  {
                  QTableWidgetItem *CellTable = new QTableWidgetItem;
                  //CellTable->setData( 0, QString::fromStdString(Value).toDouble() );
                  if(iTWRowContainer[i].first.TypeName == "string")
                    {
                    CellTable->setData( 0, QString::fromStdString(Value) );
                    }
                  else
                    {
                    CellTable->setData( 0, QString::fromStdString(Value).toDouble() );
                    }
                  CellTable->setTextAlignment(Qt::AlignCenter);
                  this->setItem(IndexUpdateRow, j, CellTable);
                  }
                }
              } //ENDIF
            }   //ENDFOR
          }     //ENDIF
        }       //ENDFOR
      this->SetColorForTable(
        iTWRowContainer, iIndexColorTraceRowContainer, iTraceName, IndexUpdateRow);
      this->SetColorForTable(
        iTWRowContainer, iIndexColorCollectionRowContainer, iCollectionName, IndexUpdateRow);
      }
    else
      {
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      std::cout << "the row does't exist";
      std::cout << std::endl;
      }
    } //ENDELSE
  this->setSortingEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::DeleteCheckedRows(std::string iTraceNameID,
                                       std::list< unsigned int > iTraceIDs)
{
  this->setSortingEnabled(false);
  std::list< unsigned int >::iterator iter = iTraceIDs.begin();
  while ( iter != iTraceIDs.end() )
    {
    int RowToDelete = this->findValueGivenColumn( *iter, iTraceNameID.c_str() );
    removeRow(RowToDelete);
    iter++;
    }
  this->setSortingEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::UpdateColumnsWithCheckBoxes(int iRow, int iColumn)
{
  if ( this->horizontalHeaderItem(iColumn)->text() == "" )
    {
    //force the checkbox to change state, even if the user didn't click
    //directly in the checkbox but in the cell containing it:
    if ( this->item(iRow, iColumn)->checkState() == 0 )
      {
      this->setCheckedUncheckedStateCheckBox(this->item(iRow, iColumn), Qt::Checked, true);
      }
    else
      {
      this->setCheckedUncheckedStateCheckBox(this->item(iRow, iColumn), Qt::Unchecked, true);
      }
    }
  if ( this->horizontalHeaderItem(iColumn)->text() == "Show" )
    {
    if ( this->item(iRow, iColumn)->checkState() == 0 )
      {
      this->setVisibleStateCheckBox(this->item(iRow, iColumn), Qt::Checked, true);
      }
    else
      {
      this->setVisibleStateCheckBox(this->item(iRow, iColumn), Qt::Unchecked, true);
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::CopySelection()
{
  //Get only the first selected Range:
  QList< QTableWidgetSelectionRange > SelectedRanges = this->selectedRanges();
  //QTableWidgetSelectionRange range = SelectedRanges.first();
  QString str;
  for ( int k = 0; k < SelectedRanges.size(); k++ )
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
  for ( int i = 1; i < ColumnsNames.size() - 1; i++ )
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
void QGoTableWidget::PrepareRangeToCopy(QTableWidgetSelectionRange iRange,
                                        QString & istr)
{
  QTableWidgetItem *t_item = NULL;

  for ( int i = 0; i < iRange.rowCount(); ++i )
    {
    if ( i > 0 )
      {
      istr += "\n";
      }
    for ( int j = 0; j < iRange.columnCount() - 1; ++j )
      {
      int k = iRange.leftColumn() + j;
      if ( k == 0 )
        {
        //for the selected column:
        if ( item(i, 0)->checkState() == 0 )
          {
          istr += "No \t";
          }
        else
          {
          istr += "Yes \t";
          }
        }
      else
        {
        t_item = this->item(iRange.topRow() + i, iRange.leftColumn() + j);
        if ( t_item )
          {
          istr += t_item->text();
          }
        else
          {
          istr += "*";
          }
        istr += "\t";
        }
      }
    t_item =
      this->item(iRange.topRow() + i, iRange.leftColumn() + iRange.columnCount() - 1);

    if ( t_item )
      {
      istr += t_item->text();
      }
    else
      {
      istr += "*";
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::ChangeCheckStateSelectedRows(std::string iTraceName,
   std::string iTraceNameID,Qt::CheckState iState)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str() );

  if ( !ListSelectedTracesID.empty() )
    {
    for ( int i = 0; i < ListSelectedTracesID.size(); i++ )
      {
      this->SetCheckStateForTraceID(ListSelectedTracesID.at(i).toUInt(),
                                    iTraceName, iState,false);
      }
    emit ModifyHighlightListTraces(ListSelectedTracesID,iState);
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
void QGoTableWidget::ChangeVisibilityStateSelectedRows(std::string iTraceName,
                                                       std::string iTraceNameID,
                                                       Qt::CheckState iState)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str() );

  if ( !ListSelectedTracesID.empty() )
    {
    for ( int i = 0; i < ListSelectedTracesID.size(); i++ )
      {
      this->SetVisibleStateForTraceID(ListSelectedTracesID.at(i).toUInt(), iTraceName,
                                      iState,false);
      }
    emit ModifyVisibilityListTraces(ListSelectedTracesID,iState);
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
void QGoTableWidget::AddValuesForID(std::vector< std::string > iColumnsNames,
                                    std::vector< std::string > iValues, unsigned int iID,
                                    std::string iColumnNameForTraceID)
{
  int RowIndex = this->findValueGivenColumn( iID, iColumnNameForTraceID.c_str() );

  if ( RowIndex != -1 )
    {
    for ( unsigned int i = 0; i < iColumnsNames.size(); i++ )
      {
      int ColumnIndex = this->findColumnName( iColumnsNames.at(i).c_str() );
      if ( ColumnIndex != -1 )
        {
        QTableWidgetItem *CellTable = new QTableWidgetItem;
        CellTable->setData( 0, QString::fromStdString( iValues.at(i) ).toDouble() );
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
  unsigned int iTraceID, std::string iTraceName)
{
  GoDBCoordinateRow CenterCoord;
  int               RowIndex = this->GetRowForTraceID(iTraceID, iTraceName);

  if ( RowIndex != -1 )
    {
    CenterCoord.SetField( "TCoord",
                          this->GetValueForItem("TimePoint", RowIndex) );
    CenterCoord.SetField( "XCoord",
                          this->GetMeanValue("XMax", "XMin", RowIndex) );
    CenterCoord.SetField( "YCoord",
                          this->GetMeanValue("YMax", "YMin", RowIndex) );
    CenterCoord.SetField( "ZCoord",
                          this->GetMeanValue("ZMax", "ZMin", RowIndex) );
    }
  return CenterCoord;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QGoTableWidget::GetValueForItem(std::string iColumnName, int iRowIndex)
{
  return
    this->item(
      iRowIndex, this->findColumnName( iColumnName.c_str() ) )->text().toInt();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoTableWidget::GetMeanValue(std::string iColumnNameOne,
                                         std::string iColumnNameTwo, unsigned int iRowIndex)
{
  int ValueOne = this->GetValueForItem(iColumnNameOne, iRowIndex);
  int ValueTwo = this->GetValueForItem(iColumnNameTwo, iRowIndex);
  int meanValue = ( ValueOne + ValueTwo ) / 2;

  return ConvertToString< int >(meanValue);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::setCheckedUncheckedStateCheckBox(QTableWidgetItem *iItem,
                                                      Qt::CheckState iState,
                                                      bool EmitSignal)
{
  if ( this->setCheckStateCheckBox(iItem, iState) && EmitSignal )
    {
    int  Row = iItem->row();
    emit CheckedRowsChanged( this->item(Row, 1)->text().toInt() );
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::setVisibleStateCheckBox(QTableWidgetItem *iItem,
                                             Qt::CheckState iState,
                                             bool EmitSignal)
{
  if ( this->setCheckStateCheckBox(iItem, iState) )
    {
    QIcon Icon;
    if ( iState == Qt::Checked )
      {
      Icon.addPixmap(QPixmap( QString::fromUtf8(":/fig/EyeIcon.png") ),
                     QIcon::Normal, QIcon::Off);
      }
    else
      {
      Icon.addPixmap(QPixmap( QString::fromUtf8(":/fig/BlankIcon.png") ),
                     QIcon::Normal, QIcon::Off);
      }
    if ( EmitSignal )
      {
      int Row = iItem->row();
      VisibleRowsChanged( this->item(Row, 1)->text().toInt() );
      }
    iItem->setIcon(Icon);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool QGoTableWidget::setCheckStateCheckBox(QTableWidgetItem *iItem,
                                           Qt::CheckState iState)
{
  bool oModification = false;

  if ( iState == Qt::Checked )
    {
    //if the row is already checked, no need to do anything:
    if ( iItem->checkState() != 2 )
      {
      iItem->setCheckState(Qt::Checked);
      iItem->setText("1");
      oModification = true;
      }
    }
  else
    {
    //if the row is already unchecked, no need to do anything:
    if ( iItem->checkState() != Qt::Unchecked )
      {
      iItem->setCheckState(Qt::Unchecked);
      iItem->setText("0");
      oModification = true;
      }
    }
  return oModification;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::map<unsigned int, std::string> QGoTableWidget::
GetTraceIDAndColumnsValues(std::string iTraceIDName,std::string &ioColumnName)
{
  std::map<unsigned int,std::string> oMapValues =
    std::map<unsigned int,std::string>();
  QList<QTableWidgetSelectionRange> Ranges = this->selectedRanges();
  if ( Ranges.size()>1 || Ranges[0].columnCount()>1 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please choose only one column to color code") );
    msgBox.exec();
    return oMapValues;
    }
  unsigned int ColumnIndex = Ranges[0].leftColumn();
  ioColumnName = this->horizontalHeaderItem(ColumnIndex)->text().toStdString();
  int NbOfRows = this->rowCount();
  unsigned int IndexTraceID = this->findColumnName(iTraceIDName.c_str());
  for( int i = 0; i<NbOfRows; i++ )
      {     
      if (this->item(i,ColumnIndex))
        {
        std::string Text = this->item(i,ColumnIndex)->text().toStdString();  //for test purpose
        oMapValues[this->item(i,IndexTraceID)->text().toUInt()] =
          this->item(i,ColumnIndex)->text().toStdString();     
        }
      else
        {
        oMapValues[this->item(i,IndexTraceID)->text().toUInt()] = "";
        }
      }
  return oMapValues;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::ShowOnlyRowsForTimePoint(unsigned int iTimePoint)
{
  int ColumnIndex = this->findColumnName("TimePoint");
  if ( ColumnIndex == -1)
    {
    std::cout<<"column TimePoint not found in the table widget";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }
  for( int i = 0; i < this->rowCount(); i++)
    {
    if (this->item(i,ColumnIndex)->text().toUInt() != iTimePoint)
      this->hideRow(i);
    else
      this->showRow(i);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTableWidget::ShowAllRows()
{
   for( int i = 0; i < this->rowCount(); i++)
    {
    this->showRow(i);
    }
}