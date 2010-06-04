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
#ifndef QGOCOMPARER2D_H
#define QGOCOMPARER2D_H

#include "itkImageToVTKImageFilter.h"
#include "itkSmartPointer.h"
#include "itkImage.h"


#include "QGoComparer.h"

#include "SnapshotHelper.h"
#include "vtkSmartPointer.h"
//#include "ui_QGoComparer.h"
class vtkCamera;
class vtkImageData;
class vtkEventQtSlotConnect;
class QGoImageView2D;
class QGoCompareOrchestra;


class QGoComparer2D : public QGoComparer {
    Q_OBJECT
public:
  QGoComparer2D(QString iComparerName, QWidget *iParent = 0);

  ~QGoComparer2D();

  /** \brief Print self informations
  */
  void PrintOs(ostream &os);

  /** \brief returns the type of comparer (2 for 2D, 3 for 3D)
  */
  int GetComparerType( void );

  /** \brief Set image displayed by the comparer
  */
  virtual void SetImage(vtkImageData* iImage);

  /** \brief Set ITK image displayed by the comparer
  */
  template <typename ITKInputImageType>
  void SetITKImage(typename ITKInputImageType::Pointer iImage)
  {
    // define the itk smartpointer to image type from the given itkimagetype
    typedef typename ITKInputImageType::Pointer ITKimagePointerType;
    // we create the appropriate connector
    typedef typename itk::ImageToVTKImageFilter<ITKInputImageType>
      itkvtkConnectorType;
    typedef typename itkvtkConnectorType::Pointer itkvtkConnectorPointerType;


    itkvtkConnectorPointerType connector = itkvtkConnectorType::New();
    connector->SetInput(iImage);
    connector->Update();

    //connector->DisconnectPipeline();
    //ITKimagePointerType ITKImage = connector->GetOutput();
    //ITKImage->DisconnectPipeline();
    //vtkSmartPointer<vtkImageData> tempvtkImage = vtkSmartPointer<vtkImageData>::New();
    //
    //connector->GetOutput();// = vtkSmartPointer<vtkImageData>::New();
    //tempvtkImage->ShallowCopy(connector->GetOutput());
    // tempvtkImage.Take(connector->GetOutput());
    // we store the transformed image
    //vtkSmartPointer<vtkImageData> tempvtkImage = connector->GetOutput();

    // we set the current image
    SetImage(connector->GetOutput());
    Update();

  }

  /** \brief Set the address of the current orchestra
  */
  void SetCurrentOrchestra(QGoCompareOrchestra* iCurrentOrchestra);

  /** \brief Returns the imageview managed by this comparer
  */
  QGoImageView2D* GetImageView( void );

public slots:
  /** \brief Save a screenshot of the viewer's content
  */
  QString SnapshotViewXY( const GoFigure::FileType& iType,
  const QString& iBaseName = tr( "Snapshot" ) );

private:
  /** \brief create the viewer contained in the widget
  */
  void createViewer( void );
};

#endif // QGOCOMPARER2D_H
