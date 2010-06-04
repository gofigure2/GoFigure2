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
#ifndef QGOCOMPARER_H
#define QGOCOMPARER_H
/*
#include "itkImageToVTKImageFilter.h"
#include "itkSmartPointer.h"
#include "itkImage.h"
*/

#include "SnapshotHelper.h"
#include "vtkSmartPointer.h"
#include "ui_QGoComparer.h"
class vtkCamera;
class vtkImageData;
class vtkEventQtSlotConnect;
class QGoImageView;
class QGoCompareOrchestra;


class QGoComparer : public QWidget, protected Ui::QGoComparer {
    Q_OBJECT
public:
  QGoComparer(QString iComparerName, QWidget *iParent = 0);

  /** \brief Destructor. */
  ~QGoComparer();

  /** \brief Set image displayed by the comparer
  */
  virtual void SetImage(vtkImageData* iImage);

  /** \brief returns the type of comparer (2 for 2D, 3 for 3D)
  */
  virtual int GetComparerType( void )=0;

  /** \brief get comparer's name
  */
  QString* GetName( void );

  /** \brief Update the viewer contained in the widget
  */
  virtual void Update( void );

  /** \brief render the viewer contained in the widget if any
  */
  virtual void Render( void );

  // get the camera of the current viewer;
  virtual vtkCamera* GetCamera( void );

  /** \brief print the comparer information :
  *  it consists in the image information if any.
  */
  virtual void PrintOs(ostream &os) = 0;

  /** \brief true if the widget has a viewer
  */
  bool HasViewer( void );

  /** \brief Set the address of the current orchestra
  */
  void SetCurrentOrchestra(QGoCompareOrchestra* iCurrentOrchestra);

public slots:
  /** \brief Save a snapshot of the displaid view, in a iType file
  */
  virtual QString SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName = tr( "Snapshot" ) ) = 0;


protected:
  /** \brief Qt change event function
  */
  void changeEvent(QEvent *e);

  QString                           m_currentComparerName;
  vtkImageData*                     m_currentImage;
  QString                           m_currentImageName;
  QGoImageView*                     m_currentView;
  QGoCompareOrchestra*              m_currentOrchestra;
  vtkEventQtSlotConnect*            m_VTKEventQtConnector;

private slots:
  // the QMetaObject::connectSlotsByName(QGoComparer)
  // is provided in the Setupui generated by moc

private:
  /** delete the viewer contained in the widget
  */
  void deleteViewer( void );

  /** create the viewer contained in the widget
  */
  virtual void createViewer( void ) = 0;
};

#endif // QGOCOMPARER_H