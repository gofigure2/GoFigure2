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

#ifndef __QGoTabManager_h
#define __QGoTabManager_h

#include <QObject>
#include <QAction>
#include <vector>

class QTabWidget;
class QGoMainWindow;
class QGoTabElementBase;

/**
\class QGoTabManager
\brief 
*/
class QGoTabManager : public QObject
{
  Q_OBJECT
  public:

    explicit QGoTabManager( QGoMainWindow* iMW = 0, QTabWidget* iTW = 0 );
    ~QGoTabManager();

    /** \brief Set the MainWindow if it has not been set by calling the constructor.*/
    void SetMainWindow( QGoMainWindow* iMW );

    /** \brief Set the TabWidget if it has not been set by calling the constructor.*/
    void SetTabWidget( QTabWidget* iTW );

  public slots:
    void ChangeCurrentTab( int iIdx );
    void CloseTab( int idx );
    void CloseAllTabs( );
    void UpdateBookmarkMenu( std::vector< QAction* > iBookmarkActions );

  private:
    QGoMainWindow*  m_MainWindow;
    QTabWidget*     m_TabWidget;
    int             m_PreviousTabIndex;

    void ClearTabElement( QGoTabElementBase* iE );
    void SetUpTabElement( QGoTabElementBase* iE );
};
#endif
