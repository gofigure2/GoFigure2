/*=========================================================================

 Program:   Insight Segmentation & Registration Toolkit
 Module:    $RCSfile: itkQtProgressBar.h,v $
 Language:  C++
 Date:      $Date:  $
 Version:   $Revision:  $

 Copyright (c) 2002 Insight Consortium. All rights reserved.
 See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/

/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009-10

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

#ifndef __itkQtProgressBar_h
#define __itkQtProgressBar_h

#include <QProgressBar>
#include "itkCommand.h"

#include "itkQtConfigure.h"

namespace itk {

class IKTQT_EXPORT QtProgressBar : public ::QProgressBar
  {

public:

  /** Command Class invoked for button redraw */
  typedef itk::MemberCommand<QtProgressBar> RedrawCommandType;

  /** Constructor */
  explicit QtProgressBar(QWidget *parent = 0);

  /** Get Command */
  RedrawCommandType * GetRedrawCommand(void) const;

  /** Manage a Progress event */
  void ProcessEvent(itk::Object * caller, const itk::EventObject& event);
  void ConstProcessEvent(const itk::Object * caller, const itk::EventObject& event);

  /** Manage a Progress event */
  void Observe(itk::Object *caller);

private:

  RedrawCommandType::Pointer m_RedrawCommand;

  };

}  // end of namespace

#endif
