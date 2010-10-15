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
#ifndef __QCellPreprocess_h
#define __QCellPreprocess_h

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkImageExport.h"

#include "itkCellPreprocess.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"

#include "ui_CellPreprocess.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <map>
#include <string>

class QCellPreprocess:public QWidget,
  private Ui::CellPreprocessWidget
{
  Q_OBJECT
public:
  typedef itk::Image< unsigned char, 3 >                        InputImageType;
  typedef InputImageType::Pointer                               InputImagePointer;
  typedef itk::CellPreprocess< InputImageType, InputImageType > PreprocessFilterType;
  typedef PreprocessFilterType::Pointer                         PreprocessFilterPointer;
  typedef itk::VTKImageExport< InputImageType >                 ImageExportType;
  typedef ImageExportType::Pointer                              ImageExportPointer;
  typedef itk::VTKImageImport< InputImageType >                 ImageImportType;
  typedef ImageImportType::Pointer                              ImageImportPointer;

  explicit QCellPreprocess(QWidget *iParent = 0);
  virtual ~QCellPreprocess(){}

  void SetMembraneDataType(bool x);

  void SetInput(std::vector< vtkImageData * > & iImg);

  std::vector< vtkImageData * > GetOutput();

signals:
  void Done(std::vector< vtkImageData * > );

protected slots :
  void on_RadiusSpinBox_valueChanged();

  void on_RadiusSlider_sliderReleased();

  void on_GlobalResetButton_clicked();

  void on_GlobalApplyButton_clicked();

  void on_singleChRadioButton_clicked();

  void on_allChRadioButton_clicked();

protected:
  void GetParameters();

  void Preprocess(unsigned int i);

  double m_CellRadius;
  bool m_MembraneData;

  std::vector< vtkImageData * > m_VTKInput;
  std::vector< vtkImageData * > m_VTKOutput;
private:
  QCellPreprocess(const QCellPreprocess &);
  QCellPreprocess operator=(const QCellPreprocess &);
};

#endif
