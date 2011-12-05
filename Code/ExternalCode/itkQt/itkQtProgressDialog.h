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

#ifndef __itkQtProgressDialog_h
#define __itkQtProgressDialog_h

#include <QProgressDialog>
#include "itkCommand.h"

#include "itkQtConfigure.h"

namespace itk
{
class IKTQT_EXPORT QtProgressDialog : public ::QProgressDialog
{
public:

  /** Command Class invoked for button redraw */
  typedef itk::MemberCommand< QtProgressDialog > RedrawCommandType;

  /** Constructor */
  explicit QtProgressDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );

  explicit QtProgressDialog( const QString & labelText,
                             const QString & cancelButtonText,
                             int minimum,
                             int maximum,
                             QWidget * parent = 0,
                             Qt::WindowFlags f = 0 );

  /** Get Command */
  RedrawCommandType * GetRedrawCommand(void) const;

  /** Manage a Progress event */
  void ProcessEvent(itk::Object *caller, const itk::EventObject & event);

  void ConstProcessEvent(const itk::Object *caller, const itk::EventObject & event);

  /** Manage a Progress event */
  void Observe(itk::Object *caller);

private:

  Q_DISABLE_COPY( QtProgressDialog );
  RedrawCommandType::Pointer m_RedrawCommand;
};
}  // end of namespace

#endif
