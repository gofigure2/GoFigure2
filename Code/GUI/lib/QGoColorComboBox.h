/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1869 $  // Revision of last commit
  Date: $Date: 2010-07-29 11:24:36 -0400 (Thu, 29 Jul 2010) $  // Date of last commit
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

#ifndef __QGoColorComboBox_h
#define __QGoColorComboBox_h

#include <QComboBox>
#include <QtGui>
#include "QGoGUILibConfigure.h"

class QGOGUILIB_EXPORT QGoColorComboBox : public QComboBox
  {
  Q_OBJECT
public:
  explicit QGoColorComboBox(std::string iTextToAddANewOne,QWidget *parent = 0);
  virtual ~QGoColorComboBox();

  typedef std::pair<std::string, QColor> ItemColorComboboxData;

  /** \brief insert a color at a given index*/
  //void insertColor(int index, const QColor& color, const QString& name);

  //int colorCount() const;

 // void setCurrentColor(const QColor& color);
 // QColor currentColor() const;

 // QColor color(int index) const;

  //void setColorDialogEnabled(bool enabled = true, std::string TextToAdd = "More...");
 // bool isColorDialogEnabled() const;

 // void setCreationCollection(bool enabled = true);
 // bool isCreationCollectionEnabled() const;

 // void setStandardColors();

 // int FindItemText(std::string Text);

  //not used
 // void IncrementTrackID();

 // QSize sizeHint() const;

  /** \brief return the name and the QColor of the current selected color*/
//  std::pair<std::string, QColor> GetCurrentColorData();

signals:
  void AddANewOneActivated();
  void ItemSelected(ItemColorComboboxData);
  //void activated(const QColor& color);
  //void highlighted(const QColor& color);
  /** \brief  signal emitted when a new color has been created by the user, in order
  to be saved in the database*/
//  void NewColorToBeSaved(std::vector<std::string>);
  /** \brief  signal emitted when a new collection has been created by the user, in order
  to be saved in the database*/
 // void NewCollectionToBeSaved();

public slots:
  /** \brief insert an item with color at the end, before the "more" and select it if 
  selectetheaddeditem is set to true*/
  void AddItemWithColor(ItemColorComboboxData,bool SelectTheAddedItem = true);
 // void addColor(const QColor& iColor, const QString& iName)
 // { insertColor(colorCount(), iColor, iName); }
  /** \brief insert the existing colors with the data taken from the database,
  containing the color names with the associated vector of rgba*/
  //to keep:
  //void setExistingColors(
  //  std::list<std::pair<std::string, std::vector<int> > > iDataColorsFromDB);
  void setItemsWithColorFromList(std::list< ItemColorComboboxData > iDataFromList);
  void ListToUpdateWithItemDeleted(std::list< ItemColorComboboxData > iDataFromList);
  
  /** \brief delete the corresponding collectionID in the list displayed
  by the colorcombobox*/
 // void DeleteCollectionID(unsigned int);
//  void DontAddTheColor();

protected:
  QString     m_LastActivated;
  ItemColorComboboxData GetTheItemColorComboBoxData(int iIndex);

  virtual     void SetActivatedItem();


protected slots:
  virtual void ActionWhenNewOneRequested() = 0;

private slots:
  void emitActivatedItem(int iIndexActivatedItem);
  //void emitHighlightedColor(int index);
  //void emitActivatedColor(int index);

private:
  std::string m_TextToAddANewOne;
 /// QColor lastActivated;
 // int    numUserColors;
 // bool   NewColorToBeAdded;
  /** \brief list of pair containing all the datas for the existing colors from the database
  with, for each color: a color name and a vector if rgba*/
 // std::list<std::pair<std::string, std::vector<int> > > m_DataFromDB;
//  bool                                                  colorDialogEnabled;
//  bool                                                  creationCollection;
  /** \brief convert the QColor and NameColor to string and emit the signal for the
  new color to be saved in the database*/
  //void PassDataForNewColorToBeSaved(QColor Color, std::string NameColor);
  };

#endif

