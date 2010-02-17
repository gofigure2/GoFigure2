/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#ifndef __QGoVisualizationDockWidget_h
#define __QGoVisualizationDockWidget_h

#include <QDockWidget>
#include <QComboBox>
#include "ui_VisualizationDockWidget.h"

#include "qtcolorcombobox.h"

class QHBoxLayout;
class QVBoxLayout;

class QGoVisualizationDockWidget :
  public QDockWidget,
  private Ui::VisualizationDockWidget
{
  Q_OBJECT

  public:
    explicit QGoVisualizationDockWidget( QWidget* parent = 0,
                                const unsigned int& iDim = 4 );
    ~QGoVisualizationDockWidget();

    void SetNumberOfChannels( const unsigned int& iN );
    void SetChannel( const unsigned int& i, const QString& iText = QString() );

    void SetXMinimumAndMaximum( const int& iMin, const int& iMax );
    void SetYMinimumAndMaximum( const int& iMin, const int& iMax );
    void SetZMinimumAndMaximum( const int& iMin, const int& iMax );
    void SetTMinimumAndMaximum( const int& iMin, const int& iMax );
    int GetCurrentChannel() const;
    int GetCurrentCollectionID ();
    void SetEnableTraceCollectionColorBoxes(bool Enable);
    std::string GetCurrentColor();
    
    QtColorComboBox* ColorTraceComboBox;
    QtColorComboBox* ColorIDCollectionComboBox;
    QComboBox*       m_ChoseCellType;
    QComboBox*       m_ChoseSubCellType;
    QLabel* TraceName;
    QLabel* CollectionName;

  public slots:
    void SetXSlice( int iSlice );
    void SetYSlice( int iSlice );
    void SetZSlice( int iSlice );
    void SetTSlice( int iSlice );
    void SetCollectionID(
      std::list<std::pair<std::string,QColor> > iListExistingID);
    void SetListCellTypes(QStringList iListCellTypes);
    void SetListSubCellTypes(QStringList iListSubCellTypes);
    void CheckUserAction(QString iCellTypeText);
    void CheckUserActionSubCell(QString iSubCellTypeText);

  signals:
    void ShowAllChannelsChanged( bool iChanged );
    void ShowOneChannelChanged( int Channel );

    void XSliceChanged( int Slice );
    void YSliceChanged( int Slice );
    void ZSliceChanged( int Slice );
    void TSliceChanged( int Slice );
    void AddANewCellType();
    void DeleteCellType();
    void AddANewSubCellType();
    void DeleteSubCellType();

  protected:
    unsigned int m_Dimension;
    std::list<std::pair<std::string,std::vector<int> > > m_DataColors;
    void SetColorTraceComboBox();
    void SetColorIDCollectionComboBox();
    void SetCellTypeComboBox();
    void SetSubCellTypeComboBox();
    QHBoxLayout* HLayoutForColor;
    QVBoxLayout* VLayoutForCollection;
};
#endif
