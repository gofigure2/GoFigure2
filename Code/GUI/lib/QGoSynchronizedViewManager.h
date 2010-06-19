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
#ifndef __QGoSynchronizedViewManager_h
#define __QGoSynchronizedViewManager_h

#include <QObject>
#include "QGoSynchronizedView2D.h"
#include "QGoSynchronizedView3D.h"

class QGoSynchronizedView2DCallbacks;
class QGoSynchronizedView3DCallbacks;
class vtkImageData;



/** \class QGoSynchronizedViewManager
* \brief Class dealing with SynchronizedViews for correct synchronization
* and handling provides a simple interface to create / delete /
* synchronize SynchronizedViews
*
* This class should be used with any class using QGoSynchronizedView
* and QGoSynchronize.
*/
class QGoSynchronizedViewManager : public QObject
{
  Q_OBJECT

public:

  explicit QGoSynchronizedViewManager(QObject *parent = 0);
  ~QGoSynchronizedViewManager();

public slots:
  /** \brief create and add a SynchronizedView2D to
   * QGoSynchronizedViewManager's parent Object/Widget
   */
  QGoSynchronizedView2D* newSynchronizedView2D(
    QString       iSynchronizedViewName,
    vtkImageData* iImage );

  /** \brief create and add a SynchronizedView 3D to
   * QGoSynchronizedViewManager's parent Object/Widget
   */
  QGoSynchronizedView3D* newSynchronizedView3D(
    QString       iSynchronizedViewName,
    vtkImageData* iImage );

  /** \brief delete a SynchronizedView from QGoSynchronizedViewManager's parent
   * Object/Widget
   * delete the SynchronizedView
   */
  void deleteSynchronizedView2D(QGoSynchronizedView2D* ioSynchronizedView);
  void deleteSynchronizedView2D(const int& iIndex);
  void deleteSynchronizedView3D(QGoSynchronizedView3D* ioSynchronizedView);
  void deleteSynchronizedView3D(const int& iIndex);

  /** \brief take a SynchronizedView out of the Manager lists, without deleting it.
   */
  void removeSynchronizedView2D(QGoSynchronizedView2D* ioSynchronizedView);
  void removeSynchronizedView3D(QGoSynchronizedView3D* ioSynchronizedView);

  /** \brief synchronize all open SynchronizedViews
   */
  void synchronizeOpenSynchronizedViews();

  /** \brief remove the synchronization leaving the SynchronizedViews independent
   */
  void unSynchronizeOpenSynchronizedViews();

  /** \brief true if the orchestra synchronizes copmarers
   */
  bool isSynchronizing();

  /** \brief update the SynchronizedViews
   */
  void Update();

  /** \brief show all the SynchronizedViews
   */
  void show();

private:

  // vector to store the open SynchronizedViews
  std::vector<QGoSynchronizedView2D*>       m_openSynchronizedViews;
  std::vector<QGoSynchronizedView3D*>       m_openSynchronizedViews3D;

  QGoSynchronizedView2DCallbacks*           m_Synchronizer;
  QGoSynchronizedView3DCallbacks*           m_Synchronizer3D;

  Q_DISABLE_COPY( QGoSynchronizedViewManager );

};

#endif // __QGoSynchronizedViewManager_h
