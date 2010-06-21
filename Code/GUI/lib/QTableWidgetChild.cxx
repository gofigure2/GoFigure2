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

#include "QTableWidgetChild.h"
#include <iostream>
#include <sstream>
#include <QTableWidgetSelectionRange>
#include <QHeaderView>
#include <QSettings>
#include <QApplication>
#include <QClipboard>


QTableWidgetChild::QTableWidgetChild( QWidget* iParent ): QTableWidget( iParent )
{
  PrevCol = -1;
  PrevOrder = -1;
  QObject::connect( this,
    SIGNAL( cellClicked(int,int) ),
    this,SLOT( UpdateTableWidgetDisplayAndVectorCheckedRows(int,int) ));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QTableWidgetChild::
QTableWidgetChild( int rows, int columns, QWidget * iParent ) :
  QTableWidget( rows, columns, iParent ), PrevCol( -1 ), PrevOrder( -1 )
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QTableWidgetChild::~QTableWidgetChild()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::sortItems( int iColumn, Qt::SortOrder iOrder )
{
  if ( ( iColumn != PrevCol ) && ( iOrder != PrevOrder ) )
    {
    PrevCol = iColumn;
    PrevOrder = iOrder;
    QTableWidget::sortItems( iColumn, iOrder );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QTableWidgetChild::findValueGivenColumn(int Value, QString Column)
{
  int ColumnIndex = findColumnName(Column);
  if (ColumnIndex == -1)
    {
    std::cout<<"The column figureID has not been found";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    std::cout << std::endl;
    return -1;
    }
  else
    {
    for (int i=0;i<rowCount();i++)
      {
      if (this->item(i,ColumnIndex)->text().toInt() == Value)
        {
        return i;
        }
      }
    }
  return -1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QTableWidgetChild::findColumnName(QString ColumnName)
{
  QStringList ColumnsHeader = recordHeaderNamesOrder();
  if (ColumnsHeader.isEmpty())
    {
    std::cout<<"The QStringlist ColumnsHeader is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return -1;
    }
  return ColumnsHeader.indexOf(ColumnName,0);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QTableWidgetChild::recordHeaderNamesOrder()
{
  QStringList ColumnNamesOrder;
  for (int i=0; i<this->columnCount();i++)
    {
    ColumnNamesOrder.append(this->horizontalHeaderItem(i)->text());
    }

  return ColumnNamesOrder;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::SetSelectRowTraceID (std::string TraceName,
  int TraceID,bool IsSelected)
{
  std::stringstream TraceIDName;
  TraceIDName << TraceName;
  TraceIDName <<"ID";

  int RowIndex = this->findValueGivenColumn(TraceID, TraceIDName.str().c_str());
  if (RowIndex == -1)
    {
    std::cerr <<"The contour "<<TraceID<<"has not been found ";
    std::cerr << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cerr << std::endl;
    return;
    }
  else
    {
    if (IsSelected)
      {
      this->item(RowIndex,0)->setCheckState(Qt::Checked);
      }
    else
      {
      this->item(RowIndex,0)->setCheckState(Qt::Unchecked);
      }
    this->UpdateVectorCheckedRows(RowIndex,0);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 bool QTableWidgetChild::TracesToHighlight(
  std::string TraceName,
  ContourMeshStructureMultiIndexContainer* ioTracesInfo )
{
  // unused argument
  (void) TraceName;

  bool oModified = false;

  //get the selected TraceID:
  std::list<int> SelectedTraces = this->GetListCheckedTraceID();

  std::list<int>::iterator selected_begin = SelectedTraces.begin();

  ContourMeshStructureMultiIndexContainer::iterator traceinfo_it = ioTracesInfo->begin();

  while( traceinfo_it != ioTracesInfo->end() )
    {
    //then, set to IsHighlight the selected ones:
    std::list<int>::iterator iter = selected_begin;

    bool found = false;
    unsigned int t_id = (*traceinfo_it).TraceID;

    while( iter != SelectedTraces.end() )
      {
      if( t_id == static_cast< unsigned int >( *iter ) )
        {
        if( !traceinfo_it->Highlighted )
          {
          oModified = true;
          }
        found = true;
        ContourMeshStructure temp( *traceinfo_it );
        temp.Highlighted = true;
        ioTracesInfo->replace( traceinfo_it, temp );
        break;
        }
      ++iter;
      }

    if( !found )
      {
      if( traceinfo_it->Highlighted )
        {
        oModified = true;
        }
      ContourMeshStructure temp( *traceinfo_it );
      temp.Highlighted = false;
      ioTracesInfo->replace( traceinfo_it, temp );
      }
    ++traceinfo_it;
    }

  return oModified;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QTableWidgetChild::ValuesForSelectedRows(QString ColumnName)
{
  QList<QTableWidgetSelectionRange> Selection;
  Selection = this->selectedRanges();
  int ColumnIndex = findColumnName(ColumnName);

  QList<QString> Values;
  for( int i=0; i< Selection.size(); i++)
    {
    int TopRowSelected = Selection[i].topRow();
    int BottomRowSelected = Selection[i].bottomRow();

    for (int j = TopRowSelected; j<BottomRowSelected+1;j++)
      {
      Values.append(this->item(j,ColumnIndex)->text());
      }
    }
  return Values;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::DisplayColumnNames( QString TableName,
 std::list<std::string> ColumnNames)
{
  size_t numberCol=ColumnNames.size();
  this->setColumnCount(static_cast<int>(numberCol));

  int i = 0;
  for( std::list<std::string>::iterator iter = ColumnNames.begin();
    iter!= ColumnNames.end();
    ++iter, ++i )
    {
    QTableWidgetItem* HeaderCol=new QTableWidgetItem;
    std::string NameHeader;
    NameHeader =*iter;

    HeaderCol->setText(NameHeader.c_str());
    if (NameHeader.empty())
      {
      HeaderCol->setToolTip(tr("Check/Uncheck %1").arg(TableName));
      }
    if(NameHeader.find("T.I.")!= std::string::npos)
      {
      HeaderCol->setToolTip(tr("T.I = Total Intensity For the Channel"));
      }
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    this->setHorizontalHeaderItem(i,HeaderCol);
    this->resizeColumnToContents(i);
    }

  this->horizontalHeader()->setSortIndicatorShown(true);
  /*Need to disabled the Sorting while printing the values from the database in
  the table widget as the sorting is making trouble*/
  this->setSortingEnabled(false);
  this->horizontalHeader()->setMovable(true);

  QObject::connect( this->horizontalHeader(),
    SIGNAL( sortIndicatorChanged(int,Qt::SortOrder) ),
    this,SLOT( sortItems(int,Qt::SortOrder)) );

  // QSettings settings( "MegasonLab", "Gofigure2" );
  QSettings settings;
  QByteArray stateTableWidget = settings.value("StateTableWidget").toByteArray();
  //QTabTableName->horizontalHeader()->restoreState(stateTableWidget);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::DisplayContent(GoDBTableWidgetContainer* iLinkToRowContainer,
  std::string TraceName, std::string CollectionName)
{
  DBTableWidgetContainerType RowContainer = iLinkToRowContainer->GetRowContainer();
  if (RowContainer.empty())
    {
    std::cout<<"The Row Container is totally empty ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    unsigned int NbofRows = 0;
    for (unsigned int i = 0; i < RowContainer.size(); i++)
      {
      bool RowsCountSet = false;
      //check that the column has to be displayed in the table widget and that there are
      //some values to be displayed:
      if (RowContainer[i].first.ColumnNameTableWidget != "None" && !RowContainer[i].second.empty())
        {
        if (NbofRows == 0)
          {
          NbofRows = RowContainer[i].second.size();
          this->setRowCount(NbofRows);
          RowsCountSet = true;
          }
        for (int j = 0; j < this->columnCount();j++)
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if (HeaderCol == RowContainer[i].first.ColumnNameTableWidget)
            {
            std::vector<std::string>::iterator iter = RowContainer[i].second.begin();
            int k=0;
            while(iter != RowContainer[i].second.end())
              {
              QTableWidgetItem* CellTable = new QTableWidgetItem;
              std::string Value = *iter;
              //CellTable->setText(Value.c_str());
              CellTable->setData(0,QString::fromStdString( Value ).toDouble() );
//.toInt());
              CellTable->setTextAlignment(Qt::AlignCenter);
              this->setItem(k,j,CellTable);
              ++iter;
              ++k;
              }//ENDWHILE
            }//ENDIF
          }//ENDFOR
        }//ENDIF
      }//ENDFOR
      SetSelectedColumn(NbofRows,0);
      this->SetColorForTable(iLinkToRowContainer,TraceName,0);
      this->SetColorForTable(iLinkToRowContainer,CollectionName,0);
    }//ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::SetSelectedColumn(unsigned int iNbOfRows,
  unsigned int StartedRow)
{
  int indexCol = findColumnName("");
  for( unsigned int i = StartedRow ; i < iNbOfRows+StartedRow; i++ )
    {
    QTableWidgetItem* Checkbox = new QTableWidgetItem;
    //Checkbox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
    //  Qt::ItemIsSelectable);
    Checkbox->setFlags(Qt::ItemIsEnabled |Qt::ItemIsSelectable);

    Checkbox->setCheckState(Qt::Unchecked);
    this->setItem(i,indexCol,Checkbox);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::SetColorForTable (GoDBTableWidgetContainer* iLinkToRowContainer,
  std::string NameGroupColor,unsigned int StartRow )
{
  DBTableWidgetContainerType RowContainer = iLinkToRowContainer->GetRowContainer();

  std::string RedInfoName = "RedFor";
  RedInfoName += NameGroupColor;
  int indexRedColumn = iLinkToRowContainer->GetIndexInsideRowContainer(RedInfoName);

  std::string GreenInfoName = "GreenFor";
  GreenInfoName += NameGroupColor;
  int indexGreenColumn = iLinkToRowContainer->GetIndexInsideRowContainer(GreenInfoName);

  std::string BlueInfoName = "BlueFor";
  BlueInfoName += NameGroupColor;
  int indexBlueColumn = iLinkToRowContainer->GetIndexInsideRowContainer(BlueInfoName);

  //for the collection color:
  std::string ColumnNameID = NameGroupColor;
  ColumnNameID += "ID";
  int indexGroupIDInTableWidget = findColumnName(ColumnNameID.c_str());
  for (unsigned int i=StartRow; i < RowContainer[1].second.size()+StartRow;i++)
    {
    QColor Color;
    QColor TextColor;
    if (RowContainer[indexGroupIDInTableWidget].second[i-StartRow] == "0")
      {
      Color.setRgb(255,255,255,255);
      int rgb = 255 - (255*3)/3;
      TextColor.setRgb(rgb,rgb,rgb,255);
      }
    else
      {
      int Red   = atoi(RowContainer[indexRedColumn].second[i-StartRow].c_str());
      int Green = atoi(RowContainer[indexGreenColumn].second[i-StartRow].c_str());
      int Blue  = atoi(RowContainer[indexBlueColumn].second[i-StartRow].c_str());
      Color.setRgb(Red,Green,Blue,255);
      int rgb = 255 - (Red+Green+Blue)/3;
      TextColor.setRgb(rgb,rgb,rgb,255);
      }
    this->item(i,indexGroupIDInTableWidget)->setBackgroundColor(Color);
    this->item(i,indexGroupIDInTableWidget)->setTextColor(TextColor);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int IndexForRowContainer(QTableWidgetChild::DBTableWidgetContainerType iRowContainer)
{
  // unused argument
  (void) iRowContainer;

  return 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::InsertNewRow(GoDBTableWidgetContainer* iLinkToRowContainer,
    std::string TraceName, std::string CollectionName)
{
   DBTableWidgetContainerType NewTraceRowContainer = iLinkToRowContainer->GetRowContainer();
   if (NewTraceRowContainer.size() == 0 || NewTraceRowContainer[1].second.size() != 1)
    {
    std::cout<<"The New Trace Row Container is totally empty or there is more than 1 trace in it";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    int NewRow = this->rowCount()+1;
    int NbRow = NewRow;
    this->setRowCount(NbRow);
    for (unsigned int i = 0; i < NewTraceRowContainer.size(); i++)
      {
      if (NewTraceRowContainer[i].first.ColumnNameTableWidget != "None" && !NewTraceRowContainer[i].second.empty())
        {
        for (int j = 0; j < this->columnCount();j++)
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if (HeaderCol == NewTraceRowContainer[i].first.ColumnNameTableWidget)
            {
            QTableWidgetItem* CellTable = new QTableWidgetItem;
            std::string Value = NewTraceRowContainer[i].second[0];
            //CellTable->setText(Value.c_str());
            CellTable->setData(0,QString::fromStdString( Value ).toInt());
            CellTable->setTextAlignment(Qt::AlignCenter);
            this->setItem(NewRow-1,j,CellTable);
            }//ENDIF
          }//ENDFOR
        }//ENDIF
      }//ENDFOR
    SetSelectedColumn(1,NewRow-1);
    this->SetColorForTable(iLinkToRowContainer,TraceName,NewRow-1);
    this->SetColorForTable(iLinkToRowContainer,CollectionName,NewRow-1);
    }//ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::UpdateRow(GoDBTableWidgetContainer* iLinkToRowContainer,
    int TraceID,std::string TraceName, std::string CollectionName)
{
  DBTableWidgetContainerType UpdateTraceRowContainer = iLinkToRowContainer->GetRowContainer();
  if (UpdateTraceRowContainer.size() == 0 || UpdateTraceRowContainer[1].second.size() != 1)
    {
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    std::cout<<"The Update Trace Row Container is totally empty or there is more than 1 trace in it";
    std::cout << std::endl;
    }
  else
    {
    QString TraceNameID = QString("%1ID").arg(TraceName.c_str());
    int iUpdateRow = this->findValueGivenColumn(TraceID,TraceNameID);
    if (iUpdateRow != -1)
      {
      for (unsigned int i = 0; i < UpdateTraceRowContainer.size(); i++)
        {
        if (UpdateTraceRowContainer[i].first.ColumnNameTableWidget != "None" && !UpdateTraceRowContainer[i].second.empty())
          {
          for (int j = 0; j < this->columnCount();j++)
            {
            std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
            if (HeaderCol == UpdateTraceRowContainer[i].first.ColumnNameTableWidget)
              {
              std::string Value = UpdateTraceRowContainer[i].second[0];
              this->item(iUpdateRow,j)->setData(0,QString::fromStdString( Value ).toInt());
              }//ENDIF
            }//ENDFOR
          }//ENDIF
        }//ENDFOR
      this->SetColorForTable(iLinkToRowContainer,TraceName,iUpdateRow);
      this->SetColorForTable(iLinkToRowContainer,CollectionName,iUpdateRow);
      }
    else
      {
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      std::cout<<"the row does't exist";
      std::cout << std::endl;
      }
    }//ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*std::vector<unsigned int> QTableWidgetChild::GetListCheckedRows()
{
  std::vector<unsigned int> oListCheckedRows;
  QStringList ListRowsChecked = this->ValuesForSelectedRows("");

  return oListCheckedRows;
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::UpdateVectorCheckedRows(int Row,int Column)
{
  if (this->item(Row,Column)->checkState()== 0)
    {
    int ID = this->item(Row,1)->text().toInt();

    std::vector<std::pair<int,int> >::iterator iter = this->m_VectorSelectedRows.begin();

    //As the initial iterator becomes incompatible once an element of the vector has been
    //erased, we need a bool to indicate the end of the vector:

    while ( iter != this->m_VectorSelectedRows.end() )
      {
      if (iter->first == ID)
        {
        this->m_VectorSelectedRows.erase(iter);
        break;
        }
      ++iter;
      }
    }
  if (this->item(Row,Column)->checkState()== 2)
    {
    std::pair<int,int> temp;
    /** \todo check that the index stays the same even if the user move the columns*/
    temp.first = this->item(Row,1)->text().toInt();
    temp.second = Row;
    this->m_VectorSelectedRows.push_back(temp);
    }
  CheckedRowsChanged();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<int> QTableWidgetChild::GetListCheckedTraceID()
{
  std::list<int> oListSelectedIDs;
  std::vector<std::pair<int,int> >::iterator iter = this->m_VectorSelectedRows.begin();
  while(iter != this->m_VectorSelectedRows.end())
    {
    oListSelectedIDs.push_back(iter->first);
    ++iter;
    }
  return oListSelectedIDs;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::UpdateIDs (unsigned int iNewCollectionID,
  std::string iCollectionIDName,QColor ColorNewCollection )
{
  int IndexCollectionID = this->findColumnName(iCollectionIDName.c_str());
  std::vector<std::pair<int,int> >::iterator iter = this->m_VectorSelectedRows.begin();
  while(iter != this->m_VectorSelectedRows.end())
    {
    this->item(iter->second,IndexCollectionID)->
      setText(ConvertToString<unsigned int>(iNewCollectionID).c_str());
    this->item(iter->second,IndexCollectionID)->setBackgroundColor(ColorNewCollection);
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::UpdateIDs (unsigned int iNewCollectionID,
  std::string iCollectionIDName, QColor ColorNewCollection,
  std::string TraceIDName,std::list<int> TraceIDToUpdate)
{
  int IndexCollectionID = this->findColumnName(iCollectionIDName.c_str());
  std::list<int>::iterator iter = TraceIDToUpdate.begin();
  while(iter != TraceIDToUpdate.end())
    {
    int RowIndex = this->findValueGivenColumn(*iter, TraceIDName.c_str());
    this->item(RowIndex,IndexCollectionID)->
      setText(ConvertToString<unsigned int>(iNewCollectionID).c_str());
    this->item(RowIndex,IndexCollectionID)->setBackgroundColor(ColorNewCollection);
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::DeleteSelectedRows(std::string iTraceNameID)
{
  unsigned int i = 0;
  while (i<this->m_VectorSelectedRows.size())
    {
    //deselect the row in the m_VectorCheckedRows:
    int ColumnSelectedRow = this->findColumnName("");
    int RowToDelete = this->findValueGivenColumn(
      this->m_VectorSelectedRows[i].first,iTraceNameID.c_str());
    if (RowToDelete != -1)
      {
      this->item(RowToDelete,ColumnSelectedRow)->setCheckState(Qt::Unchecked);
      this->UpdateVectorCheckedRows(RowToDelete,ColumnSelectedRow);
      this->removeRow(RowToDelete);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::UpdateTableWidgetDisplayAndVectorCheckedRows(int Row, int Column)
{
  if (this->horizontalHeaderItem(Column)->text() == "")
    { 
    //force the checkbox to change state, even if the user didn't click 
    //directly in the checkbox but in the cell containing it:
    if (this->item(Row,Column)->checkState()== 0)
      {
      this->item(Row,Column)->setCheckState(Qt::Checked);
      }
    else
      {
      this->item(Row,Column)->setCheckState(Qt::Unchecked);
      }
    this->UpdateVectorCheckedRows(Row,Column);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::CopySelection()
{
  //Get only the first selected Range:
  QList<QTableWidgetSelectionRange> SelectedRanges = this->selectedRanges();
  //QTableWidgetSelectionRange range = SelectedRanges.first();
  QString str;
  for (int k = 0; k <SelectedRanges.size();k++)
    {
    QTableWidgetSelectionRange range = SelectedRanges.at(k);
    this->PrepareRangeToCopy(range,str);
    str += "\n";
    }
  QApplication::clipboard()->setText(str);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::CopyTable()
{
  //first, copy the column names:
  QStringList ColumnsNames = this->recordHeaderNamesOrder();
  QString str;
  str += "IsSelected";
  str += "\t";
  for (int i=1; i<ColumnsNames.size()-1;i++)
    {
    str += ColumnsNames.at(i);
    str += "\t";
    }
  str += ColumnsNames.at(ColumnsNames.size()-1);
  str += "\n";
  //second, get the range for the total selection:
  QTableWidgetSelectionRange RangeForAllTableSelected(
    0,0,this->rowCount()-1,this->columnCount()-1);
  //third, copy the range:
  this->PrepareRangeToCopy(RangeForAllTableSelected,str);
  QApplication::clipboard()->setText(str);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::PrepareRangeToCopy(QTableWidgetSelectionRange Range, 
  QString &str)
{
  for (int i = 0; i < Range.rowCount(); ++i) 
    {
    if (i > 0)
      {
      str += "\n";
      }
    for (int j = 0; j < Range.columnCount()-1; ++j) 
      {
      int k = Range.leftColumn() + j;
      if( k == 0)
        {
        //for the selected column:
        if (item(i,0)->checkState()== 0)
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
        str += this->item(Range.topRow() + i, Range.leftColumn() + j)->text();
        str += "\t";
        }
      }
    str += this->item(Range.topRow() + i, Range.leftColumn() + Range.columnCount()-1)->text();
    } 
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::CheckSelectedRows(std::string iTraceName,
  std::string iTraceNameID)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str());
  if (!ListSelectedTracesID.empty())
    {
    for(int i=0; i<ListSelectedTracesID.size();i++)
      {
      this->SetSelectRowTraceID (iTraceName, 
        atoi(ListSelectedTracesID.at(i).toStdString().c_str()),true);
      }
    }
  else
    {
    std::cout<<"The list of selected Traces ID is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::UncheckSelectedRows(std::string iTraceName,
 std::string iTraceNameID)
{
  QStringList ListSelectedTracesID = this->ValuesForSelectedRows(
    iTraceNameID.c_str());
  if (!ListSelectedTracesID.empty())
    {
    for(int i=0; i<ListSelectedTracesID.size();i++)
      {
      this->SetSelectRowTraceID (iTraceName, 
        atoi(ListSelectedTracesID.at(i).toStdString().c_str()),false);
      }
    }
  else
   {
   std::cout<<"The list of selected Traces ID is empty";
   std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
   std::cout << std::endl;
   }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::AddValuesForID(std::vector<std::string> iColumnsNames,
  std::vector<std::string> iValues, unsigned int iID, 
  std::string iColumnNameForTraceID)
{
  int RowIndex = this->findValueGivenColumn(iID,iColumnNameForTraceID.c_str());
  if (RowIndex != -1)
    {
    for (unsigned int i = 0; i < iColumnsNames.size(); i++)
      {
      int ColumnIndex = this->findColumnName(iColumnsNames.at(i).c_str());
      if (ColumnIndex != -1 )
        {
        QTableWidgetItem* CellTable = new QTableWidgetItem;
        CellTable->setData(0,QString::fromStdString( iValues.at(i) ).toDouble() );
//QString::fromStdString( iValues.at(i)));
        CellTable->setTextAlignment(Qt::AlignCenter);
        this->setItem(RowIndex,ColumnIndex,CellTable);
        }
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow QTableWidgetChild::GetCoordinateCenterBoundingBox()
{
  GoDBCoordinateRow CenterCoord;
  if(this->m_VectorSelectedRows.size() == 1)
    {
    //QString ColumnName = iColumnNameForTraceID.c_str();
    int RowIndex = this->m_VectorSelectedRows.at(0).second;
    //int RowIndex =
      //this->findValueGivenColumn(this->m_VectorSelectedRows.at(0).first,ColumnName);
    if (RowIndex != -1)
      {
      CenterCoord.SetField("TCoord",
        this->GetValueForItem("TimePoint",RowIndex));
      CenterCoord.SetField("XCoord",
        this->GetMeanValue("XMax","XMin",RowIndex));
      CenterCoord.SetField("YCoord",
        this->GetMeanValue("YMax","YMin",RowIndex));
      CenterCoord.SetField("ZCoord",
        this->GetMeanValue("ZMax","ZMin",RowIndex));
      }
    }
  return CenterCoord;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int QTableWidgetChild::GetValueForItem(std::string iColumnName, int iRowIndex)
{
  return
    this->item(
        iRowIndex,this->findColumnName(iColumnName.c_str()))->text().toInt();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QTableWidgetChild::GetMeanValue(std::string iColumnNameOne,
  std::string iColumnNameTwo, unsigned int iRowIndex)
{
  int ValueOne = this->GetValueForItem(iColumnNameOne,iRowIndex);
  int ValueTwo = this->GetValueForItem(iColumnNameTwo,iRowIndex);
  int meanValue = (ValueOne + ValueTwo)/2;
  return ConvertToString<int>(meanValue);
}
