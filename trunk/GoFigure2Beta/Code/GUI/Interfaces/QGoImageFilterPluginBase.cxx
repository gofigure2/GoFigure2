#include "QGoImageFilterPluginBase.h"
#include "vtkImageData.h"

#include <QMessageBox>

/**
 * \class QGoImageFilterPluginBase
 * \brief
*/
QGoImageFilterPluginBase::
QGoImageFilterPluginBase() : m_VTKInput( 0 ), m_VTKOutput( 0 )
{}

QGoImageFilterPluginBase::~QGoImageFilterPluginBase()
{}

void QGoImageFilterPluginBase::SetInput( std::vector< vtkImageData* >& iInput )
{
  QMessageBox::information( 0, "Title", "QGoImageFilterPluginBase::setInput" );
  m_VTKInput = iInput;
}

void QGoImageFilterPluginBase::Update()
{
  if( !m_VTKInput.empty() )
    {
    if( m_VTKInput.front() )
      {
      this->Process();
      }
    }
}

std::vector< vtkImageData* > QGoImageFilterPluginBase::GetOutput()
{
  return m_VTKOutput;
}


