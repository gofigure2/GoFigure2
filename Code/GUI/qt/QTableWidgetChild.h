/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 194 $  // Revision of last commit
  Date: $Date: 2009-05-19 12:18:20 -0400 (Tue, 19 May 2009) $  // Date of last commit
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
  explicit QMEGAVTKADDON2_EXPORT QTableWidgetChild ( int rows, int columns,
    QWidget * parent = 0);
  virtual QMEGAVTKADDON2_EXPORT ~QTableWidgetChild();
  QStringList QMEGAVTKADDON2_EXPORT recordHeaderNamesOrder();


protected:
  int PrevCol;
  int PrevOrder;

  /** \brief return the row index where the given value is located when specifying
  the column name: */
  int findValueGivenColumn(int Value, QString Column);

  /** \brief return the column index who has a column header matching ColumnName */
  int findColumnName(QString ColumnName,QStringList ColumnsHeader);
  //return a List of all the header names of the columns, :

public slots:
  /** \brief sort items given one column and one sort order. */
  QMEGAVTKADDON2_EXPORT void sortItems(int column, Qt::SortOrder order);

  /** \brief select the row corresponding to the given FigureID.
      \todo Change the name of the method (one instance of this class is not necessarily
      containing contours, but could be a mesh, tracks, etc...). So the purpose
      of this method is to be able to select one row given an id. What about:
      void SelectRowWithGivenColumnNameAndValue( QString iColumnName, int iId )?
  */
  QMEGAVTKADDON2_EXPORT void SelectRowFigureID (int FigureID);

  /** \brief return the list of the contours from the widget where the user
      selected the entire rows, to be highlighten in the visualization.
      \todo Change the name of the method (see todo note written above).
      what about std::list<int> GetListOfSelectedRows() ?
      \todo Overload this method to be able to return a std::map<int,bool> where
      the key is the id of the rows (for example FigureID) and the value is to know
      if it is selected. */
  QMEGAVTKADDON2_EXPORT std::map<int,bool> ContoursToHighlight( );
};

#endif