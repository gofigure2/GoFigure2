/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 577 $  // Revision of last commit
  Date: $Date: 2009-08-18 10:45:50 -0400 (Tue, 18 Aug 2009) $  // Date of last commit
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

#include "QTableWidgetChild.h"
#include <iostream>
#include <sstream>
#include <QTableWidgetSelectionRange>
#include <QHeaderView>
#include <QSettings>
#include "QTableWidgetNumericalItem.h"


QTableWidgetChild::QTableWidgetChild( QWidget* iParent ): QTableWidget( iParent )
{
  PrevCol = -1;
  PrevOrder = -1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QTableWidgetChild::QTableWidgetChild ( int rows, int columns,
                  QWidget * iParent ):QTableWidget(rows,columns,iParent)
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
  QStringList ColumnsHeader = recordHeaderNamesOrder();
  if (ColumnsHeader.isEmpty())
    {
    std::cout<<"The QStringlist ColumnsHeader is empty";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return -1;
    }

  int ColumnIndex = findColumnName(Column,ColumnsHeader);
  if (ColumnIndex == -1)
    {
    std::cout<<"The column figureID has not been found";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
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
int QTableWidgetChild::findColumnName(QString ColumnName, QStringList ColumnsHeader)
{
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
  int TraceID, bool IsSelected)
{
  std::stringstream TraceIDName;
  TraceIDName << TraceName;
  TraceIDName <<"ID";

  int RowIndex = this->findValueGivenColumn(TraceID, TraceIDName.str().c_str());
  if (RowIndex == -1)
    {
    std::cout<<"The contour "<<TraceID<<"has not been found ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    QTableWidgetSelectionRange RangeToSelect(RowIndex,0,RowIndex,columnCount()-1);
    this->setRangeSelected(RangeToSelect,IsSelected);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 void QTableWidgetChild::TracesToHighlight(
  std::string TraceName,std::vector<GoDBTraceInfoForVisu> & ioTracesInfo)
{
  //first set all the hightlighted traces to false:
  for( unsigned int i=0; i < ioTracesInfo.size(); i++ )
    {
    ioTracesInfo[i].IsHighLighted = false;
    }
  
  //get the selected TraceID:
  std::stringstream TraceID;
  TraceID <<TraceName;
  TraceID << "ID" ;
  TraceID.str();
  QStringList ListTraceIDToHighLight = ValuesForSelectedRows(TraceID.str().c_str());
  //then, set to IsHighlight the selected ones:
  for (int i = 0; i<ListTraceIDToHighLight.size();i++)
    {
    int j = ListTraceIDToHighLight.at(i).toInt()-1;
    ioTracesInfo[j].IsHighLighted = true;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QTableWidgetChild::ValuesForSelectedRows(QString ColumnName)
{
  QList<QTableWidgetSelectionRange> Selection;
  Selection = this->selectedRanges();

  QStringList ColumnsHeader = this->recordHeaderNamesOrder();
  int ColumnIndex = findColumnName(ColumnName,ColumnsHeader);

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
  int numberCol=ColumnNames.size();
  this->setColumnCount(numberCol);
  
  int i = 0;
  for ( std::list<std::string>::iterator iter = ColumnNames.begin(); 
    iter!= ColumnNames.end();iter++)
    {
    QTableWidgetItem* HeaderCol=new QTableWidgetItem;
    std::string NameHeader;
    NameHeader =*iter;

    HeaderCol->setText(NameHeader.c_str());
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    this->setHorizontalHeaderItem(i,HeaderCol);
    this->resizeColumnToContents(i);
    i++;
    }
  
  this->horizontalHeader()->setSortIndicatorShown(true);
  /*Need to disabled the Sorting while printing the values from the database in
  the table widget as the sorting is making trouble*/
  this->setSortingEnabled(false);
  this->horizontalHeader()->setMovable(true);

  QObject::connect( this->horizontalHeader(),
    SIGNAL( sortIndicatorChanged(int,Qt::SortOrder) ),
    this,SLOT( sortItems(int,Qt::SortOrder)) );

  QSettings settings( "MegasonLab", "Gofigure2" );
  QByteArray stateTableWidget = settings.value("StateTableWidget").toByteArray();
  //QTabTableName->horizontalHeader()->restoreState(stateTableWidget);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::DisplayContent(DBTableWidgetContainerType iRowContainer)
{ 
  if (iRowContainer.empty())
    {
    std::cout<<"The Row Container is totally empty ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    unsigned int NbofRows = 0;
    for (unsigned int i = 0; i < iRowContainer.size(); i++)
      {    
      bool RowsCountSet = false;
      //check that the column has to be displayed in the table widget and that there are
      //some values to be displayed:
      if (iRowContainer[i].first.ColumnNameTableWidget != "None" && !iRowContainer[i].second.empty())
        {
        if (NbofRows == 0)
          {
          NbofRows = iRowContainer[i].second.size();
          this->setRowCount(NbofRows);
          RowsCountSet = true;
          }
        for (unsigned int j = 0; j <this->columnCount();j++)
          {
          std::string HeaderCol = this->horizontalHeaderItem(j)->text().toStdString();
          if (HeaderCol == iRowContainer[i].first.ColumnNameTableWidget)
            {
            std::vector<std::string>::iterator iter = iRowContainer[i].second.begin();
            int k=0;
            while(iter != iRowContainer[i].second.end())
              {
              QTableWidgetNumericalItem* CellTable = new QTableWidgetNumericalItem;
              std::string Value = *iter;
              CellTable->setText(Value.c_str());
              this->setItem(k,j,CellTable);
              iter++;
              k++;
              }//ENDWHILE
            }//ENDIF
          }//ENDFOR
        }//ENDIF
      }//ENDFOR
      SetSelectedColumn(NbofRows);
    }//ENDELSE
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::SetSelectedColumn(unsigned int iNbOfRows)
{
  //int indexCol = findColumnName( "Selected", recordHeaderNamesOrder());
  int indexCol = findColumnName( "", recordHeaderNamesOrder());
  for (unsigned int i =0 ; i < iNbOfRows ; i++)
    {
    QTableWidgetNumericalItem* Checkbox = new QTableWidgetNumericalItem;
    Checkbox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
      Qt::ItemIsSelectable);
    Checkbox->setCheckState(Qt::Unchecked);
    this->setItem(i,indexCol,Checkbox);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTableWidgetChild::SetColorForAllTable (DBTableWidgetContainerType iRowContainer)
{
  for (int i=0; i<iRowContainer.size();i++)
    {

    }
  //QColor Color (Red,Green,Blue,Alpha); 
  //int Row = 0;
  //int Column = 0;
  //this->item(Row,Column)->setBackgroundColor(Color);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int IndexForRowContainer(QTableWidgetChild::DBTableWidgetContainerType iRowContainer)
{
  return 0;
}