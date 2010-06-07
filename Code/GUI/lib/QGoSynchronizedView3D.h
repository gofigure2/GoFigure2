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
#ifndef QGoSynchronizedView3D_H
#define QGoSynchronizedView3D_H

#include "QGoSynchronizedView.h"
#include "vtkSmartPointer.h"
class vtkCamera;
class vtkImageData;
class vtkEventQtSlotConnect;
class QGoImageView3D;
class QGoCompareOrchestra;


class QGoSynchronizedView3D : public QGoSynchronizedView
{
  Q_OBJECT
public:
  QGoSynchronizedView3D(QString iViewName, QWidget *iParent = 0);

  ~QGoSynchronizedView3D();

  /** print the comparer information :
  *  it consists in the image information if any.
  */
  void PrintOs(ostream &os);

  int GetComparerType();

  /** Set image displayed by the comparer
  */
  void SetImage(vtkImageData* iImage);

  /** \brief returns the type of comparer (2 for 2D, 3 for 3D)
  */
  virtual int GetSynchronizedViewType( void );

  /** render the iId'th imageview:
  *  3D visualization usually contains 4 imageviewers :
  *  three 2D projection and a 3D view : iId=[0-3]
  */
  void Render(const int& iId);

  /** get the camera of the current viewer;
  *  iId=[0-3]
  */
  vtkCamera* GetCamera(const int& iId);

  /** Get the fullscreen view : iId = [0-4]
  *  0 : Quadview (all 4 views)
  *  1 : XY
  *  2 : XZ
  *  3 : YZ
  *  4 : 3D view
  */
  int GetFullScreenView();

  QGoImageView3D* GetImageView();

public slots:
  QString SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName = tr( "Snapshot" ) );

  QString SnapshotView2( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  QString SnapshotView3( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  QString SnapshotViewXYZ( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  /** Set the fullscreen view : iId = [0-4]
  *  0 : Quadview (all 4 views)
  *  1 : XY
  *  2 : XZ
  *  3 : YZ
  *  4 : 3D view
  */
  void SetFullScreenView(const int& iId);

  /** Set the fullscreen view : XY
  */
  void SetFullXYScreenView();

  /** Set the fullscreen view : XZ
  */
  void SetFullXZScreenView();

  /** Set the fullscreen view : YZ
  */
  void SetFullYZScreenView();

  /** Set the fullscreen view : XYZ
  */
  void SetFullXYZScreenView();

  /** Set the fullscreen view : 3D view
  */
  void SetQuadView();

private:
  /** Create the viewer in the widget
  */
  void createViewer();
};

#endif // QGoSynchronizedView3D_H
