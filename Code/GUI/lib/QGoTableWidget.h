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

/**
\class QGoTableWidget
\brief inherits from the Qt class QTableWidget, manages all the interactions between
the user and the data related to the traces
\ingroup GUI
*/
class QGOGUILIB_EXPORT QGoTableWidget:public QTableWidget
{
  Q_OBJECT
public:
  explicit QGoTableWidget(QWidget *parent = 0);
  explicit QGoTableWidget(int rows, int columns, QWidget *parent = 0);

  virtual ~QGoTableWidget();

  typedef GoDBTableWidgetContainer::TWContainerType TWContainerType;

  /** \todo Lydie: check if code reduction with a class member variable Tracename
  and a method to return TraceNameID from TraceName*/

  /**
  \brief display the columns names and the content of iTWRowContainer in the
  table
  \param[in] iTWRowContainer contains the data to be displayed and the
  corresponding info to know how to display them
  \param[in] iIndexColorTraceRowContainer index to know where to find the
  color of the trace in the iTWRowContainer
  \param[in] iIndexColorCollectionRowContainer index to know where to find
  the color of the collection in the iTWRowContainer
  \param[in] iTraceName name of the trace
  \param[in] iCollectionName name of the collection
  \param[in] iColumnNames list of the column names to be displayed
  \param[in] iState if true, the isvisible is checked
  \param[in] iIndexShowColumn index of the show column in the TW Container(
  for contour and mesh)
  */
  void DisplayInitialContent(const TWContainerType & iTWRowContainer,
                             const std::vector< int > & iIndexColorTraceRowContainer,
                             const std::vector< int > & iIndexColorCollectionRowContainer,
                             const std::string & iTraceName,
                             const std::string & iCollectionName,
                             const std::list< std::pair< std::string, std::string > > & iColumnNames,
                             Qt::CheckState iState,
                             int iIndexShowColumn = 0);

  void InsertNewRows(const TWContainerType & iTWRowContainer,
                     const std::vector< int > & iIndexColorTraceRowContainer,
                     const std::vector< int > & iIndexColorCollectionRowContainer,
                     const std::string & iTraceName,
                     const std::string & iCollectionName,
                     Qt::CheckState iVisible = Qt::Checked);
  /**
  \brief Insert a new row and fill the cells with the data
  contained in the RowContainer
  \param[in] iTWRowContainer contains the data to be displayed and the
  corresponding info to know how to display them for one row only
  \param[in] iIndexColorTraceRowContainer index to know where to find the
  color of the trace in the iTWRowContainer
  \param[in] iIndexColorCollectionRowContainer index to know where to find
  the color of the collection in the iTWRowContainer
  \param[in] iTraceName name of the trace
  \param[in] iCollectionName name of the collection
 */
  void InsertOnlyOneNewRow(const TWContainerType & iTWRowContainer,
                           const std::vector< int > & iIndexColorTraceRowContainer,
                           const std::vector< int > & iIndexColorCollectionRowContainer,
                           const std::string & iTraceName,
                           const std::string & iCollectionName,
                           Qt::CheckState iVisible = Qt::Checked);

  /**
  \brief Replace the data in the cells corresponding to the traceID with
  the new data contained in the RowContainer
  \param[in] iTWRowContainer contains the data to be displayed and the
  corresponding info to know how to display them for one row only
  \param[in] iIndexColorTraceRowContainer index to know where to find the
  color of the trace in the iTWRowContainer
  \param[in] iIndexColorCollectionRowContainer index to know where to find
  the color of the collection in the iTWRowContainer
  \param[in] iTraceName name of the trace
  \param[in] iCollectionName name of the collection
  \param[in] iTraceID ID of the trace to be updated
  */
  void UpdateRow(const TWContainerType & iTWRowContainer,
                 const std::vector< int > & iIndexColorTraceRowContainer,
                 const std::vector< int > & iIndexColorCollectionRowContainer,
                 const std::string & iTraceName,
                 const std::string & iCollectionName,
                 int iTraceID);


  void DeleteRowsWithSpecificTimePoints(const QStringList & iListTPs);

  /**
  \brief delete the rows previously checked by the user
  \param[in] iTraceNameID name of the traceID displayed in the table
  \param[in] iTraceIDs list of the traceIDs for which the rows need to be
  deleted
  */
  void DeleteCheckedRows(const std::string & iTraceNameID,
                         const std::list< unsigned int > & iTraceIDs);

  /**
  \brief add values in the table for the corresponding traceID and column names
  \param[in] iColumnsNames names of the columns to display the values
  \param[in] iValues vector containing the values in the same order as the column names
  \param[in] iID ID of the trace where to display the values
  \param[in] iColumnNameForTraceID name of the traceID
  */
  void AddValuesForID(const std::vector< std::string > & iColumnsNames,
                      const std::vector< std::string > & iValues,
                      unsigned int iID,
                      const std::string & iColumnNameForTraceID);

  /**
  \brief calculate the center of the bounding box for the only selected trace
  and return it as a GoDBCoordinateRow
  \param[in] iTraceID ID for the trace the center of bounding box
  needs to be calculated
  \param[in] iTraceName name of the trace
  \return GoDBCoordinateRow corresponds to the center of the bounding box for
  the trace
  */
  GoDBCoordinateRow GetCoordinateCenterBoundingBox(unsigned int iTraceID,
                                                   const std::string & iTraceName);

  /**
  \brief set the state of the checkbox for the check/uncheck column and the TraceID row
  to iState
  \param[in] iTraceID traceID for which the checkbox state needs to be set
  \param[in] iTraceName name of the trace
  \param[in] iState state to which the checkbox needs to be set
  \param[in] EmitSignal if set to true, will emit a signal to tell if the state has
  changed, if set to false will not emit,set to true by default
  */
  void SetCheckStateForTraceID(unsigned int iTraceID,
                               const std::string & iTraceName,
                               Qt::CheckState iState,
                               bool EmitSignal = true);

  /**
  \brief set the state and icon of the checkbox for the IsVisible column and the TraceID row
  to iState
  \param[in] iTraceID traceID for which the IsVisible checkbox state needs to be set
  \param[in] iTraceName name of the trace
  \param[in] iState state to which the checkbox and the icon need to be set
  \param[in] EmitSignal if set to true, will emit a signal to tell if the state has
  changed, if set to false will not emit,set to true by default
  */
  void SetVisibleStateForTraceID(unsigned int iTraceID,
                                 const std::string & iTraceName,
                                 Qt::CheckState iState,
                                 bool EmitSignal = true);

  /**
  \brief
  \return a map with all the traceIDs and the values of the column
  for which a cell of a column is selected
  \param[in] iTraceIDName name of the column for TraceID
  \param[in,out] ioColumnName name of the column of the values
  */
  std::map<unsigned int, std::string> GetTraceIDAndColumnsValues(
    const std::string & iTraceIDName, std::string &ioColumnName);

/**
  \brief update the checkboxes and icon of the visible column for the iListTraceIDs following iState
  \param[in] iListTraceIDs list of the IDs for the traces with visiblity to be set to iState
  \param[in] iState state to which the column IsVisible needs to be modified for iListTraceIDs
  \param[in] iTraceName name of the trace
  */
  void SetVisibleStateForListTraceIDs( const std::list<unsigned int> & iListTraceIDs,
                                       Qt::CheckState iState,
                                       const std::string & iTraceName);

  /**
  \brief hide all rows who have a timepoint different than iTimePoint
  \param[in] iTimePoint current timepoint
  */
  void ShowOnlyRowsForTimePoint(unsigned int iTimePoint);

  void ShowAllRows();

  /**
  \brief create the table widget items for the columns Header and set the
  corresponding tooltips for them
  \param[in] iColumnNamesAndToolTip list of all the names of the columns to be displayed in the
  table with their tooltips
  */
  void DisplayColumnNames(
    const std::list< std::pair<std::string, std::string > >& iColumnNamesAndToolTip);

  QString GetValue(unsigned int iTraceID,
                   const std::string & iTraceName,
                   const std::string & iColumn);

  void DeleteRowsAndColumns();


public slots:

  /**
  \brief uncheck/check the boxes in the check/uncheck column for the rows where at
  least one cell is selected
  \param[in] iTraceName name of the trace
  \param[in] iTraceNameID name of the traceID
  \param[in] iState state to which the checkboxes need to be modified
  */
  void ChangeCheckStateSelectedRows(std::string iTraceName,
                                    std::string iTraceNameID,
                                    Qt::CheckState iState);

  /**
  \brief check/uncheck the visible boxes for the rows where at least one cell is
  selected
  \param[in] iTraceName name of the trace
  \param[in] iTraceNameID name of the traceID
  \param[in] iState state to which the visibility need to be modified
  */
  void ChangeVisibilityStateSelectedRows(std::string iTraceName,
                                         std::string iTraceNameID,
                                         Qt::CheckState iState);

  /**
  \brief convert the text in the selection to a QString with  anti slash n and anti slash t
  and put it in the Clipboard to be pasted in other applications
  */
  void CopySelection();

  /**
  \brief convert the text in the all table and the columns namse
  to a QString with anti slash n and anti slash t and put it in the Clipboard to be
  pasted in other applications
  */
  void CopyTable();

signals:
  void CheckedRowsChanged(int iTraceID);

  void VisibleRowsChanged(int iTraceID);

  void ModifyHighlightListTraces(QStringList,Qt::CheckState);

  void ModifyVisibilityListTraces(QStringList,Qt::CheckState);

protected:
  int PrevCol;
  int PrevOrder;

  /**
  \brief get the value in the table for the given iRowIndex and
  for the given column name
  \param[in] iColumnName name of the column for which the value is needed
  \param[in] iRowIndex index of the row for which the value is needed
  \return corresponding value for the item
  */
  int GetValueForItem(const std::string & iColumnName, int iRowIndex);

  /**
  \brief calculate the mean value for both columns in the given row
  \param[in] iColumnNameOne name of the first column
  \param[in] iColumnNameTwo name of the second column
  \param[in] iRowIndex index of the row for which the mean value is needed
  \return the mean value of both columns in the given row
  */
  std::string GetMeanValue(const std::string & iColumnNameOne,
                           const std::string & iColumnNameTwo,
                           unsigned int iRowIndex);

  /**
  \brief return the row index where the given value is located when specifying
  the column name.
  \param[in] iValue value to be look for
  \param[in] iColumn name of the column in which to look for
  \return the row index where the value was found
  */
  int findValueGivenColumn(int iValue, const QString & iColumn);

  /**
  \brief return the column index who has a column header matching
  ColumnName
  \param[in] iColumnName name of the column the index is needed
  \return index of the column
  */
  int findColumnName(const QString & iColumnName);

  /**
  \brief put the text in the cells which are part of the range in a
  QString and insert antislash n and antislash t to be read by other applications
  \param[in] iRange selected cells
  \param[in,out] istr text of the selected cells
  */
  void PrepareRangeToCopy(const QTableWidgetSelectionRange & iRange,
                          QString & istr);

  /**
  \brief return the RowIndex corresponding to the TraceID
  \param[in] iTraceID ID of the Trace
  \param[in] iTraceName name of the trace
  \return the index of the row where the traceID was found
  */
  int GetRowForTraceID(unsigned int iTraceID,
                       const std::string & iTraceName);

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

  /**
  \brief return a list of the values of a specific column for the rows where the user
  has selected at least one cell.
  \param[in] iColumnName name of the column to look for value
  \return QStringList of the values for the column where the user has selected at
  least once cell in the same row
  */
  QStringList ValuesForSelectedRows(const QString & iColumnName);

  /**
  \brief Put checkboxes in the column "check/uncheck"
  \param[in] iNbOfRows number of rows for which to put a checkbox
  \param[in] iStartedRow index of the first row where to put a checkbox
  */
  void SetSelectedColumn(unsigned int iIndexRow);

  /**
  \brief Put checkboxes and icons in the column "Show"
  \param[in] iNbOfRows number of rows for which to put a checkbox and an icon
  \param[in] iStartedRow index of the first row where to put a checkbox and an icon
  \param[in] iState state to which the checkboxes need to be set
  */
  void SetVisibleColumn( unsigned int iIndexRow,
                         Qt::CheckState iState = Qt::Checked);

  /**
  \brief get the rgba values from the iTWRowContainer and display them in the
  column NameGroupColorID
  \param[in] iTWRowContainer contains the data to be displayed
  \param[in] iIndexColorRowContainer index of the iTWRowContainer to find the
  rgba values
  \param[in] iNameGroupColor name of the trace for which the color needs to
  be displayed
  \param[in] iStartRow index of the first row where to display the color
  */
  void SetColorForTable(const TWContainerType & iTWRowContainer,
                        unsigned int iIndexTWRowContainer,
                        const std::vector< int > & iIndexColorRowContainer,
                        const std::string & iNameGroupColor,
                        unsigned int iIndexRowTW);

  QStringList recordHeaderNamesOrder();

  /**
  \brief check if the value is suitable to be displayed, if yes, return true,
  if not return false so the QTableWidgetChildItem is not created for it
  \param[in] iValue value to be checked
  \param[in] iHeaderCol name of the column
  \return true if the value needs to be displayed, false if not
  */
  bool CheckValueToDisplayData(const std::string & iValue,
                               const std::string & iHeaderCol);

  void InsertNewRow(const TWContainerType & iTWRowContainer,
                    unsigned int iIndexTWRowContainer,
                    const std::vector< int > & iIndexColorTraceRowContainer,
                    const std::vector< int > & iIndexColorCollectionRowContainer,
                    const std::string & iTraceName,
                    const std::string & iCollectionName,
                    Qt::CheckState iVisible = Qt::Checked);

  void DisplayDataForOneRow(const TWContainerType & iTWRowContainer,
                            unsigned int iIndexTWRowContainer,
                            int iIndexTWRow,
                            const std::vector< int > & iIndexColorTraceRowContainer,
                            const std::vector< int > & iIndexColorCollectionRowContainer,
                            const std::string & iTraceName,
                            const std::string & iCollectionName);

protected slots:
  /**
  \brief sort items given one column and one sort order.
  \param[in] iColumn index of the column to be sorted
  \param[in] iOrder order for the column to be sorted
  */
  void sortItems(int iColumn, Qt::SortOrder iOrder);

  /**
  \brief check if the cell clicked is from the check/uncheck column
  or the "Show" column,change the state of the boxes correspondingly
  and emit the corresponding signal
  \param[in] iRow index of the row clicked
  \param[in] iColumn index of the column clicked
  */
  void UpdateColumnsWithCheckBoxes(int iRow, int iColumn);
};

#endif
