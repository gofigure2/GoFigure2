/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQtAdaptor.h,v $
  Language:  C++
  Date:      $Date: 2002-09-13 14:32:40 $
  Version:   $Revision: 1.4 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/*=========================================================================
   Modifications were made by the GoFigure Dev. Team.
   while at Megason Lab, Systems biology, Harvard Medical school, 2009

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

#ifndef __itkQtAdaptor_h
#define __itkQtAdaptor_h

#include <QObject>
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkCommand.h"

#include "itkQtConfigure.h"

namespace itk {

/** Helper class that interface with Qt Signals and Slots */
class IKTQT_EXPORT QtTranslator : public QObject
  {
  Q_OBJECT

public:
  QtTranslator() {}
  virtual ~QtTranslator() {}

signals:
  void Signal();

public slots:
  virtual void Slot() {}
  virtual void Slot(int) {}
  virtual void Slot(double) {}

  };

/** Helper class that interface Methods with Qt Slots */
template <typename T>
class IKTQT_EXPORT QtSlotAdaptor : public QtTranslator
  {
  typedef  void (T::*TMemberFunctionVoidPointer)();
  typedef  void (T::*TMemberFunctionIntPointer)(int);
  typedef  void (T::*TMemberFunctionDoublePointer)(double);

public:
  QtSlotAdaptor() : m_MemberFunctionVoid(0),
    m_MemberFunctionInt(0),
    m_MemberFunctionDouble(0) {}

  virtual ~QtSlotAdaptor() {}

  /** Specify the callback function. */
  void SetCallbackFunction(T* object,
                           TMemberFunctionVoidPointer memberFunction)
  {
    m_This = object;
    m_MemberFunctionVoid = memberFunction;
  }

  /** Specify the callback function. */
  void SetCallbackFunction(T* object,
                           TMemberFunctionIntPointer memberFunction)
  {
    m_This = object;
    m_MemberFunctionInt = memberFunction;
  }

  /** Specify the callback function. */
  void SetCallbackFunction(T* object,
                           TMemberFunctionDoublePointer memberFunction)
  {
    m_This = object;
    m_MemberFunctionDouble = memberFunction;
  }

  /** Slot to be connected to Qt Signals. */
  void Slot()
  {
    if (m_MemberFunctionVoid)
      {
      ((*m_This).*(m_MemberFunctionVoid))();
      }
  }

  /** Slot to be connected to Qt Signals. */
  void Slot(int value)
  {
    if (m_MemberFunctionInt)
      {
      ((*m_This).*(m_MemberFunctionInt))(value);
      }
  }

  /** Slot to be connected to Qt Signals. */
  void Slot(double value)
  {
    if (m_MemberFunctionDouble)
      {
      ((*m_This).*(m_MemberFunctionDouble))(value);
      }
  }

protected:
  T*                           m_This;
  TMemberFunctionVoidPointer   m_MemberFunctionVoid;
  TMemberFunctionIntPointer    m_MemberFunctionInt;
  TMemberFunctionDoublePointer m_MemberFunctionDouble;

  };

/** Helper class that interface Observers with Qt Signals */
class IKTQT_EXPORT QtSignalAdaptor : public QtTranslator
  {
  typedef SimpleMemberCommand<QtSignalAdaptor> CommandType;

public:
  QtSignalAdaptor()
    {
    m_Command = CommandType::New();
    m_Command->SetCallbackFunction(this, &QtSignalAdaptor::EmitSignal);
    }

  virtual ~QtSignalAdaptor() {}

  CommandType * GetCommand()
  {
    return m_Command;
  }

  void EmitSignal()
  {
    emit Signal();
  }

private:
  CommandType::Pointer m_Command;
  };

} // end namespace

#endif
