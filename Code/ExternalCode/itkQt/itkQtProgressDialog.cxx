/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#include "itkProcessObject.h"
#include "itkQtProgressDialog.h"

namespace itk
{
/** Constructor */
QtProgressDialog
::QtProgressDialog( QWidget * iParent, Qt::WindowFlags f ) : QProgressDialog(iParent, f )
{
  m_RedrawCommand = RedrawCommandType::New();
  m_RedrawCommand->SetCallbackFunction(this, &QtProgressDialog::ProcessEvent);
  m_RedrawCommand->SetCallbackFunction(this, &QtProgressDialog::ConstProcessEvent);

  this->setMinimum(0);
  this->setMaximum(100);
}

QtProgressDialog
::QtProgressDialog( const QString & labelText,
                    const QString & cancelButtonText,
                    int minimum,
                    int maximum,
                    QWidget * iParent,
                    Qt::WindowFlags f ) :
  QProgressDialog( labelText, cancelButtonText, minimum, maximum, iParent, f )
{
  m_RedrawCommand = RedrawCommandType::New();
  m_RedrawCommand->SetCallbackFunction(this, &QtProgressDialog::ProcessEvent);
  m_RedrawCommand->SetCallbackFunction(this, &QtProgressDialog::ConstProcessEvent);
}

/** Get Command */
QtProgressDialog::RedrawCommandType *
QtProgressDialog::GetRedrawCommand() const
{
  return m_RedrawCommand.GetPointer();
}

/** Manage a Progress event */
void
QtProgressDialog::ProcessEvent( itk::Object *caller,
                                const itk::EventObject & iEvent )
{
  if ( typeid( itk::ProgressEvent )   ==  typeid( iEvent ) )
    {
    itk::ProcessObject::Pointer process = dynamic_cast< itk::ProcessObject * >( caller );

    const int tempvalue = static_cast< int >(
          process->GetProgress() * static_cast< float >( this->maximum() ) / 10 );

    this->setValue(tempvalue);
    }
}

void
QtProgressDialog::ConstProcessEvent( const itk::Object *caller,
                                     const itk::EventObject & iEvent)
{
  if ( typeid( itk::ProgressEvent )   ==  typeid( iEvent ) )
    {
    itk::ProcessObject::ConstPointer process =
      dynamic_cast< const itk::ProcessObject * >( caller );

    const int temp_value = static_cast< int >(
        process->GetProgress() * static_cast< float >( this->maximum() ) );
    this->setValue(temp_value);
    }
}

/** Manage a Progress event */
void
QtProgressDialog::Observe(itk::Object *caller)
{
  caller->AddObserver( itk::ProgressEvent(), m_RedrawCommand.GetPointer() );
}

} // end namespace fltk
