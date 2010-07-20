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
 modificatio  n, are permitted provided that the following conditions are met:

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
#include "QCellPreprocess.h"

#include "vtkitkAdaptor.h"

#include <iostream>

//--------------------------------------------------------------------------
QCellPreprocess::QCellPreprocess(QWidget* iParent) : QWidget(iParent)
  {
  this->setupUi(this);
  m_CellRadius = 4.0;
  m_MembraneData = true;

  this->buttonGroup->setId(allChRadioButton, 0);
  this->buttonGroup->setId(singleChRadioButton, 1);
  this->ChannelComboBox->setEnabled(false);
  }

void QCellPreprocess::SetMembraneDataType(bool x)
{
  m_MembraneData = x;
}

void QCellPreprocess::SetInput(std::vector<vtkImageData*>& iImg)
{
  m_VTKInput = iImg;
  m_VTKOutput.resize(m_VTKInput.size(), 0);
}

std::vector<vtkImageData*> QCellPreprocess::GetOutput()
{
  return m_VTKOutput;
}

//--------------------------------------------------------------------------
void QCellPreprocess::GetParameters()
{ // Get the current snapshot of parameters
  m_CellRadius = static_cast<double>(this->RadiusSpinBox->value());
}

//--------------------------------------------------------------------------
void QCellPreprocess::on_RadiusSpinBox_valueChanged()
{
  double t = this->RadiusSpinBox->value();
  double max_spin = this->RadiusSpinBox->maximum();
  double min_spin = this->RadiusSpinBox->minimum();

  int max_slider = this->RadiusSlider->maximum();
  int min_slider = this->RadiusSlider->minimum();

  int out = static_cast<int>(min_slider +
                             (max_slider - min_slider) * (t - min_spin) / (max_spin - min_spin));

  this->RadiusSlider->setValue(out);
}

//--------------------------------------------------------------------------
void QCellPreprocess::on_RadiusSlider_sliderReleased()
{
  int t = this->RadiusSlider->value();

  double max_spin = this->RadiusSpinBox->maximum();
  double min_spin = this->RadiusSpinBox->minimum();

  int max_slider = this->RadiusSlider->maximum();
  int min_slider = this->RadiusSlider->minimum();

  double out = static_cast<double>(min_spin +
                                   (max_spin - min_spin) * (t - min_slider) / (max_slider - min_slider));

  this->RadiusSpinBox->setValue(out);
}

//--------------------------------------------------------------------------
void QCellPreprocess::on_allChRadioButton_clicked()
{
  this->ChannelComboBox->setEnabled(false);
}

//--------------------------------------------------------------------------
void QCellPreprocess::on_singleChRadioButton_clicked()
{
  this->ChannelComboBox->setEnabled(true);
}

//--------------------------------------------------------------------------
void QCellPreprocess::on_GlobalResetButton_clicked()
{
  this->m_CellRadius = 4.0;
  this->RadiusSpinBox->setValue(this->m_CellRadius);
}

//--------------------------------------------------------------------------
void QCellPreprocess::on_GlobalApplyButton_clicked()
{
  unsigned int option = static_cast<unsigned int>(this->buttonGroup->checkedId());
  if (!option)
    {
    Preprocess(0);
//    Preprocess( 1 );
//    Preprocess( 2 );
    }
  else
    {
    unsigned int channel = static_cast<unsigned int>(this->ChannelComboBox->currentIndex());
    Preprocess(channel);
    }

  emit Done(m_VTKOutput);
}

void QCellPreprocess::Preprocess(unsigned int i)
{
  if (m_VTKInput[i])
    {
    // convert VTK image to ITK image
    vtkImageExport* vtkExporter = vtkImageExport::New();
    vtkExporter->SetInput(m_VTKInput[i]);

    ImageImportPointer itkImporter = ImageImportType::New();

    ConnectPipelines(vtkExporter, itkImporter);

    InputImagePointer m_ITKImage = itkImporter->GetOutput();

    PreprocessFilterPointer filter = PreprocessFilterType::New();
    filter->SetInput (m_ITKImage);
    filter->SetLargestCellRadius (m_CellRadius);   // in real coordinates
    filter->SetMembraneData(m_MembraneData);
    filter->Update();

    InputImagePointer m_ITKOutputImage = filter->GetOutput();

    ImageExportType::Pointer itkExporter = ImageExportType::New();
    itkExporter->SetInput(m_ITKOutputImage);

    vtkImageImport* vtkImporter = vtkImageImport::New();

    ConnectPipelines(itkExporter, vtkImporter);

    m_VTKOutput[i] = vtkImporter->GetOutput();

    vtkExporter->Delete();
    vtkImporter->Delete();
    }
}
