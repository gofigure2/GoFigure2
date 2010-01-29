/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 525 $  // Revision of last commit
  Date: $Date: 2009-08-05 16:08:25 -0400 (Wed, 05 Aug 2009) $  // Date of last commit
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

#ifndef __QTableWidgetChild_h
#define __QTableWidgetChild_h

#include <QTableWidget>
#include <QStringList>
#include <QList>
#include "MegaVTK2Configure.h"
#include "GoDBTraceInfoForVisu.h"
#include "GoDBTraceInfoForTableWidget.h"
#include "GoDBTableWidgetContainer.h"
#include "ContourMeshStructure.h"

/**
\class QTableWidgetchild
\brief in the QTableWidget class, the method sorItems is a public function, not a public slots,
so it is not possible to connect it directly with a signal.
that's the reason for the creation of QTableWidgetchild.
*/
class QTableWidgetChild : public QTableWidget
{
  Q_OBJECT

public:
  explicit QTableWidgetChild( QWidget* parent = 0 );
  explicit QTableWidgetChild( int rows, int columns, QWidget * parent = 0 );

  virtual ~QTableWidgetChild();
  QStringList recordHeaderNamesOrder();

  typedef GoDBTableWidgetContainer::DBTableWidgetContainerType
    DBTableWidgetContainerType;

  void DisplayColumnNames( QString TableName,std::list<std::string> ColumnNames);
  void DisplayContent(GoDBTableWidgetContainer* iLinkToRowContainer,
    std::string TraceName, std::string CollectionName);

   /** \brief Insert a new row and fill the cells with the data
  contained in the RowContainer with the link: iLinkToRowContainer*/
  void InsertNewRow(GoDBTableWidgetContainer* iLinkToRowContainer,
    std::string TraceName, std::string CollectionName);

  /** \brief Replace the data in the cells corresponding to the traceID with
  the new data contained in the RowContainer with the link: iLinkToRowContainer*/
  void UpdateRow(GoDBTableWidgetContainer* iLinkToRowContainer,
    int TraceID,std::string TraceName, std::string CollectionName);

  void DeleteSelectedRows(std::string iTraceNameID);

  std::list<int> GetListCheckedTraceID();
  /** \brief Change the CollectionID in the trace table of the selected
  rows with the newCollectionID and set the background with the colorNewCollection*/
  void UpdateIDs (unsigned int iNewCollectionID,std::string iCollectionIDName, 
    QColor ColorNewCollection);
  /** \brief Change the CollectionID in the trace table of the TraceIDToUpdate
  with the newCollectionID and set the background with the colorNewCollection*/
  void UpdateIDs(unsigned int iNewCollectionID,std::string iCollectionIDName, 
  QColor ColorNewCollection,std::string TraceIDName,std::list<int> TraceIDToUpdate);

signals:
  void CheckedRowsChanged();

protected:
  int PrevCol;
  int PrevOrder;
  
  /** \brief Vector containing the ID of the selected row and the index in the table widget*/
  std::vector<std::pair<int,int> > m_VectorSelectedRows;

  /** \brief return the row index where the given value is located when specifying
  the column name: */
  int findValueGivenColumn(int Value, QString Column);

  /** \brief return the column index who has a column header matching
  ColumnName */
  int findColumnName(QString ColumnName);

  /** \brief Update the m_VectorSelectedRows */
  void UpdateVectorCheckedRows(int Row,int Column);
  /** \brief put the text in the cells which are part of the range in a 
  QString and insert \n and \t to be read by other applications*/
  void PrepareRangeToCopy(QTableWidgetSelectionRange Range, QString &str);

public slots:
  /** \brief sort items given one column and one sort order. */
  void sortItems(int column, Qt::SortOrder order);

  /** \brief select or unselect the row corresponding to the given TraceID.
  */
  void SetSelectRowTraceID (std::string TraceName, int TraceID,
    bool IsSelected);

  void UpdateTableWidgetDisplayAndVectorCheckedRows(int Row, int Column);

  /** \brief modify the ioTracesInfo in order to set the IsHighLighted parameter to false
  for the traces not selected by the user and set it to true for the selected ones,
  selected ones means at least one cell in the row has been selected by the user in
  the tableWidget. The TraceName has to be chosen between Contour and Mesh*/
  void TracesToHighlight(std::string TraceName,
    std::vector<ContourMeshStructure>* ioTracesInfo);

  /** \brief return a list of the values of a specific column for the rows where the user
       has selected at least one cell.*/
  QStringList ValuesForSelectedRows(QString ColumnName );

  /** \brief Put checkboxes in the column "Selected" */
  void SetSelectedColumn(unsigned int iNbOfRows,unsigned int StartedRow);

  void SetColorForTable (GoDBTableWidgetContainer* iLinkToRowContainer,
  std::string NameGroupColor,unsigned int StartRow);

  //std::vector<unsigned int> GetListCheckedRows();
  /** \brief convert the text in the selection to a QString with \n and \t 
  and put it in the Clipboard to be pasted in other applications*/
  void CopySelection();
  /** \brief convert the text in the all table and the columns namse 
  to a QString with \n and \t and put it in the Clipboard to be 
  pasted in other applications*/
  void CopyTable();

};

#endif
