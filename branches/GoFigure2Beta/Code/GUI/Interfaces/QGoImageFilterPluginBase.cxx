#include "QGoImageFilterPluginBase.h"
#include "vtkImageData.h"

#include <QMessageBox>

/**
 * \class QGoImageFilterPluginBase
 * \brief
*/
QGoImageFilterPluginBase::QGoImageFilterPluginBase()
{
  this->m_VTKInput = 0;
  this->m_VTKOutput = 0;
}

QGoImageFilterPluginBase::~QGoImageFilterPluginBase()
{}

void QGoImageFilterPluginBase::SetInput( vtkImageData* iInput )
{
  QMessageBox::information( 0, "Title", "QGoImageFilterPluginBase::setInput" );
  m_VTKInput = iInput;
}

void QGoImageFilterPluginBase::Update()
{
  if( m_VTKInput )
    {
    this->Process();
    }
}

vtkImageData* QGoImageFilterPluginBase::GetOutput()
{
  return m_VTKOutput;
}
