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

#ifndef __QGoTableWidget_h
#define __QGoTableWidget_h

#include <QTableWidget>
#include <QStringList>
#include <QList>
#include "MegaVTK2Configure.h"
#include "GoDBTraceInfoForVisu.h"
#include "GoDBTraceInfoForTableWidget.h"
#include "GoDBTableWidgetContainer.h"
#include "QGoGUILibConfigure.h"
#include "GoDBCoordinateRow.h"

/// \todo definitively remove this include
//#include "ContourMeshStructureHelper.h"

/**
\class QGoTableWidget
\brief in the QTableWidget class, the method sorItems is a public function, not a public slots,
so it is not possible to connect it directly with a signal.
that's the reason for the creation of QGoTableWidget.
*/
class QGOGUILIB_EXPORT QGoTableWidget:public QTableWidget
{
  Q_OBJECT
public:
  explicit QGoTableWidget(QWidget *parent = 0);
  explicit QGoTableWidget(int rows, int columns, QWidget *parent = 0);

  virtual ~QGoTableWidget();

  typedef GoDBTableWidgetContainer::TWContainerType TWContainerType;

  void DisplayColumnNames(QString TableName, std::list< std::string > ColumnNames);

  void DisplayContent(TWContainerType iTWRowContainer,
                      std::vector< int > iIndexColorTraceRowContainer,
                      std::vector< int > iIndexColorCollectionRowContainer,
                      std::string iTraceName, std::string iCollectionName,
                      std::list< std::string > iColumnNames,
                      Qt::CheckState iState,
                      int iIndexShowColumn = 0);

  /** \brief Insert a new row and fill the cells with the data
 contained in the RowContainer with the link: iLinkToRowContainer*/
  void InsertNewRow(TWContainerType iTWRowContainer,
                    std::vector< int > iIndexColorTraceRowContainer,
                    std::vector< int > iIndexColorCollectionRowContainer,
                    std::string iTraceName, std::string iCollectionName);

  /** \brief Replace the data in the cells corresponding to the traceID with
  the new data contained in the RowContainer with the link: iLinkToRowContainer*/
  void UpdateRow(TWContainerType iTWRowContainer,
                 std::vector< int > iIndexColorTraceRowContainer,
                 std::vector< int > iIndexColorCollectionRowContainer,
                 std::string iTraceName, std::string iCollectionName,
                 int iTraceID);

  void DeleteCheckedRows(std::string iTraceNameID, std::list< unsigned int > iTraceIDs);

  /** \brief Change the CollectionID in the trace table of the TraceIDToUpdate
  with the newCollectionID and set the background with the colorNewCollection*/
  void UpdateIDs(unsigned int iNewCollectionID, std::string iCollectionIDName,
                 QColor ColorNewCollection, std::string TraceIDName, std::list< int > TraceIDToUpdate);

  void AddValuesForID(std::vector< std::string > iColumnsNames,
                      std::vector< std::string > iValues, unsigned int iID,
                      std::string iColumnNameForTraceID);

  /**
  \brief calculate the center of the bounding box for the only selected trace
  and return it as a GoDBCoordinateRow
  \param[in] iTraceID ID for the trace the center of bounding box
  needs to be calculated
  \param[in] iTraceName name of the trace
  \return GoDBCoordinateRow corresponds to the center of the bounding box for
  the trace
  */
  GoDBCoordinateRow GetCoordinateCenterBoundingBox(unsigned int iTraceID, std::string iTraceName);

  /** \brief get the value in the table for the given iRowIndex and
  for the given column name*/
  int GetValueForItem(std::string iColumnName, int iRowIndex);

  /** \brief calculate the mean value for both columns in the given row*/
  std::string GetMeanValue(std::string iColumnNameOne,
                           std::string iColumnNameTwo, unsigned int iRowIndex);

  /** \brief set the state of the checkbox for the check/uncheck column and the TraceID row
  to iState*/
  void SetCheckStateForTraceID(unsigned int iTraceID,
                               std::string iTraceName,
                               Qt::CheckState iState,
                               bool EmitSignal = true);

  /** \brief set the state and icon of the checkbox for the IsVisible column and the TraceID row
  to iState*/
  void SetVisibleStateForTraceID(unsigned int iTraceID,
                                 std::string iTraceName,
                                 Qt::CheckState iState,
                                 bool EmitSignal = true);

  /**
  \brief update the checkboxes and icon of the visible column for the iListTraceIDs following iState
  \param[in] iListTraceIDs list of the IDs for the traces with visiblity to be set to iState
  \param[in] iState state to which the column IsVisible needs to be modified for iListTraceIDs
  \param[in] iTraceName name of the trace
  */
  void SetVisibleStateForListTraceIDs( std::list<unsigned int> iListTraceIDs,
    Qt::CheckState iState,std::string iTraceName);

public slots:
  /** \brief check the boxes for the rows where at least one cell is selected */
  void CheckSelectedRows(std::string iTraceName,
                         std::string iTraceNameID);

  /** \brief uncheck the boxes for the rows where at least one cell is selected
    */
  void UncheckSelectedRows(std::string iTraceName,
                           std::string iTraceNameID);

  /** \brief check the visible boxes for the rows where at least one cell is
    selected */
  void ShowSelectedRows(std::string iTraceName, std::string iTraceNameID);

  /** \brief uncheck the visible boxes for the rows where at least one cell is
    selected */
  void HideSelectedRows(std::string iTraceName, std::string iTraceNameID);

  /** \brief convert the text in the selection to a QString with \n and \t
  and put it in the Clipboard to be pasted in other applications*/
  void CopySelection();

  /** \brief convert the text in the all table and the columns namse
  to a QString with \n and \t and put it in the Clipboard to be
  pasted in other applications*/
  void CopyTable();

signals:
  void CheckedRowsChanged(int iTraceID);

  void VisibleRowsChanged(int iTraceID);

protected:
  int PrevCol;
  int PrevOrder;

  /** \brief return the row index where the given value is located when specifying
  the column name: */
  int findValueGivenColumn(int Value, QString Column);

  /** \brief return the column index who has a column header matching
  ColumnName */
  int findColumnName(QString ColumnName);

  /** \brief put the text in the cells which are part of the range in a
  QString and insert \n and \t to be read by other applications*/
  void PrepareRangeToCopy(QTableWidgetSelectionRange Range, QString & str);

  /**
  \brief return the RowIndex corresponding to the TraceID
  \param[in] iTraceID ID of the Trace
  \param[in] iTraceName name of the trace
  \return the index of the row where the traceID was found
  */
  int GetRowForTraceID(unsigned int iTraceID,
                       std::string iTraceName);

  /**
  \brief change the state for the check/uncheck checkbox to iState and emit a signal
  if EmitSignal = true
  \param[in] iItem item in the tablewidget where there is a checkbox in the
  "Checked/Unchecked" column
  \param[in] iState state to which the Checkbox needs to be set
  \param[in] EmitSignal if true, a signal will be emitted with the corresponding TraceID
  */
  void setCheckedUncheckedStateCheckBox(QTableWidgetItem *iItem,
                                        Qt::CheckState iState,
                                        bool EmitSignal);

  /**
  \brief change the state and the icon for the "IsVisible" checkbox depending on iState
  and emit a signal if EmitSignal = true
  \param[in] iItem item in the tablewidget where there is a checkbox in the
  "IsVisible" column
  \param[in] iState state to which the Checkbox and the icon need to be set
  \param[in] EmitSignal if true, a signal will be emitted with the corresponding TraceID
  */
  void setVisibleStateCheckBox(QTableWidgetItem *iItem,
                               Qt::CheckState iState,
                               bool EmitSignal = true);

  /**
  \brief change the state for the checkbox to iState and return true if the
  state has been changed
  \param[in] iItem item with a checkbox
  \param[in] iState state to which the checkbox needs to be set
  */
  bool setCheckStateCheckBox(QTableWidgetItem *iItem, Qt::CheckState iState);

  /** \brief return a list of the values of a specific column for the rows where the user
       has selected at least one cell.*/
  QStringList ValuesForSelectedRows(QString ColumnName);

  /** \brief Put checkboxes in the column "Selected" */
  void SetSelectedColumn(unsigned int iNbOfRows, unsigned int StartedRow);

  /** \brief Put checkboxes in the column "Show" */
  void SetVisibleColumn(unsigned int iNbOfRows, unsigned int StartedRow,
    Qt::CheckState iState = Qt::Checked);

  void SetVisibleColumn(unsigned int iNbOfRows,
                                      unsigned int StartedRows,
                                      std::vector<std::string> iListState);

  void SetColorForTable(TWContainerType iTWRowContainer,
                        std::vector< int > iIndexColorRowContainer,
                        std::string NameGroupColor, unsigned int StartRow);

  QStringList recordHeaderNamesOrder();

  /**
  \brief check if the value is suitable to be displayed, if yes, return true,
  if not return false so the QTableWidgetChildItem is not created for it
  */
  bool CheckValueToDisplayData(std::string iValue, std::string HeaderCol);

protected slots:
  /** \brief sort items given one column and one sort order. */
  void sortItems(int column, Qt::SortOrder order);

  /** \brief check if the cell clicked is from the check/uncheck column
    or the "Show" column and change the state of the boxes correspondingly*/
  void UpdateColumnsWithCheckBoxes(int Row, int Column);
};

#endif
