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
#include "QGoComboBox.h"

#include "ui_QGoTraceManualEditingWidget.h"

#include "QGoGUILibConfigure.h"

/**
\class QGoTraceManualEditingWidget
\brief this class contains all the comboboxes for collectionID,color,
celltypes and subcelltypes, and displays the trace and collection name.
\ingroup GUI
*/
class QGOGUILIB_EXPORT QGoTraceManualEditingWidget:
  public QWidget,
  private Ui::QGoTraceManualEditingWidget
{
  Q_OBJECT
public:
  explicit QGoTraceManualEditingWidget(QWidget *parent = 0);
  ~QGoTraceManualEditingWidget();

  typedef QGoColorComboBox::ItemColorComboboxData ItemColorComboboxData;
  typedef std::vector< std::pair< std::string, std::string > >
  NamesDescrContainerType;

  //void SetEnableTraceCollectionColorBoxes(bool Enable);

  /**
  \brief get the name of the trace currently displayed in the QLabel
  \return std::string contains the name of the trace
  */
  std::string GetTraceName();

  /**
  \brief set the selected celltype in the combobox corresponding to iCellTypeText
  \param[in] iCellTypeText name of the CellType to be selected
  */
  void SetCurrentCellType(std::string iCellTypeText);

  /**
  \brief set the selected subcelltype in the combobox corresponding to iSubCellTypeText
  \param[in] iSubCellTypeText name of the SubCellType to be selected
  */
  void SetCurrentSubCellType(std::string iSubCellTypeText);

  /**
  \brief set the selected color in the combobox corresponding to iColorText
  \param[in] iColorText name of the Color to be selected
  */
  void SetCurrentColor(std::string iColorText);

  /**
  \brief set the selected collectionID in the combobox to iID
  \param[in] iID  ID of the collection to be selected
  */
  void SetCurrentCollectionID(std::string iID);

  /**
  \brief update the QLabel with iTrace and iCollection, the "add a new.." in the
  CollectionColorComboBox and hide/show the celltype and subcelltype comboboxes
  according to the iTrace
  \param[in] iTrace name of the trace to update
  \param[in] iCollection name of the collection to update
  */
  void UpdateTraceAndCollection(
    std::string iTrace, std::string iCollection);

  /**
  \brief replace the list of collectionID with the ID and corresponding color
  in the iListExistingID and select the ID corresponding to iCollectionIDtoSelect
  if not empty, if empty, select the 1rst one
  \param[in] iListExistingID list of IDs with QColor to be displayed
  \param[in] iCollectionIDtoSelect ID to be selected in the combobox
  */
  void SetListCollectionID(
    std::list< ItemColorComboboxData > iListExistingID,
    std::string iCollectionIDtoSelect = "");

  /**
  \brief replace the list of colors with the name and corresponding color
 in the iListColors and select the color corresponding to iColortoSelect
 if not empty, if empty, select the 1rst one
 \param[in] iListColors list of colors with their names and QColor to be displayed
 \param[in] iColortoSelect name of the color to be selected in the combobox
 */
  void SetListColors(std::list< ItemColorComboboxData > iListColors,
                     std::string iColorToSelect = "");

  /**
  \brief replace the list of celltype with the names in the iCellTypesData and
  select the celltype corresponding to iCellTypetoSelect if not empty, if empty,
  select the 1rst one.
  \param[in] iCellTypesData list of celltypes with their names and description
  to be displayed
  \param[in] iCellTypetoSelect name of the celltype to be selected in the combobox
  */
  void SetListCellTypes(NamesDescrContainerType iCellTypesData,
                        std::string iCellTypeToSelect = "");

  /**
  \brief replace the list of subcelltype with the names in the iSubCellTypesData and
  select the subcelltype corresponding to iSubCellTypetoSelect if not empty, if empty,
  select the 1rst one.
  \param[in] iSubCellTypesData list of subcelltypes with their names and description
  to be displayed
  \param[in] iSubCellTypetoSelect name of the subcelltype to be selected in the combobox
  */
  void SetListSubCellTypes(NamesDescrContainerType iSubCellData,
                           std::string iSubCellTypeToSelect = "");

  /**
  \brief add a new collection in the collectionColorCombobox and select it
  \param[in] iNewCollectionID ID and QColor of the new item
  */
  void AddANewCollectionID(ItemColorComboboxData iNewCollectionID);

signals:
  void AddANewCellType();

  void DeleteCellType();

  void AddANewSubCellType();

  void DeleteSubCellType();

  void AddNewColor();

  void DeleteColor();

  void NewCollectionActivated(ItemColorComboboxData);
  void NewSelectedColorActivated(ItemColorComboboxData);
  void NewSubCellTypeActivated(std::string);

  void NewCellTypeActivated(std::string);

  void NewCollectionToBeCreated();

protected:
  QLabel *                    m_TraceName;
  QLabel *                    m_CollectionName;
  QGoSelectedColorComboBox *  m_SelectedColorComboBox;
  QGoCollectionColorComboBox *m_CollectionColorComboBox;
  QGoComboBox *               m_ChoseCellType;
  QGoComboBox *               m_ChoseSubCellType;

  /**
  \brief add the SelectedColorCombobox to the layout and make the signal/slot connections
  for it
  */
  void SetSelectedColorComboBox();

  /**
  \brief add the CollectionColorCombobox to the layout,set the trace and collection name labels,
  and make the signal/slot connections
  for it
  */
  void SetTraceCollectionColorComboBox();

  /**
  \brief add the Celltype QGoCombobox to the layout and make the signal/slot connections
  for it
  */
  void SetCellTypeComboBox();

  /**
  \brief add the SubCellType QGoCombobox to the layout and make the signal/slot connections
  for it
  */
  void SetSubCellTypeComboBox();

  /**
  \brief call the right methods to initialize the list if there is no iTextItemToSelect
  or select the iTextItemToSelect if not empty.
  \param[in] iComboBox the combobox with the list to be set up
  \param[in] iItemsData the list of items with their names and descriptions to be displayed
  \param[in] iTextItemToSelect the item to be selected
  \tparam T could be a QGoComboBox
  */
  template< typename T >
  void SetListItemAndSelect(T *iComboBox, NamesDescrContainerType iItemsData,
                            std::string iTextItemToSelect = "")
  {
    if ( !iTextItemToSelect.empty() )
      {
      iComboBox->SetItemsFromList(iItemsData);
      if ( iComboBox->findText( iTextItemToSelect.c_str() ) != -1 )
        {
        iComboBox->SetCurrentItem(iTextItemToSelect);
        }
      else
        {
        iComboBox->SetCurrentItemAndActivate(0);
        }
      }
    else
      {
      iComboBox->InitializeTheList(iItemsData);
      }
  }

  /**
  \overload
  */
  template< typename T >
  void SetListItemAndSelect(T *iComboBox, std::list< ItemColorComboboxData > iItemsData,
                            std::string iTextItemToSelect = "")
  {
    if ( !iTextItemToSelect.empty() )
      {
      iComboBox->SetItemsFromListWithColor(iItemsData);
      if ( iComboBox->findText( iTextItemToSelect.c_str() ) != -1 )
        {
        iComboBox->SetCurrentItem(iTextItemToSelect);
        }
      else
        {
        iComboBox->SetCurrentItemAndActivate(0);
        }
      }
    else
      {
      iComboBox->InitializeTheListWithColor(iItemsData);
      }
  }
};
#endif
