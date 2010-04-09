#include "QGoImageFilterPluginBase.h"
#include "vtkImageData.h"

#include <QMessageBox>

/**
 * \class QGoImageFilterPluginBase
 * \brief
*/
QGoImageFilterPluginBase::
QGoImageFilterPluginBase( QObject* iParent ) : QGoPlugin( iParent ),
  m_VTKInput( 0 ), m_VTKOutput( 0 )
{}

QGoImageFilterPluginBase::~QGoImageFilterPluginBase()
{}

void QGoImageFilterPluginBase::SetInput( vtkImageData* iInput )
{
  m_VTKInput.resize( 1, 0 );
  m_VTKInput[0] = iInput;
}

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


