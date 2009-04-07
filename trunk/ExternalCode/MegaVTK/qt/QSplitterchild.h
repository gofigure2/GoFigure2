/*=========================================================================
 Authors: The GoFigure Dev. Team.
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

#ifndef __QSplitterchild_h
#define __QSplitterchild_h

#include <QSplitter>
#include "MegaVTK2Configure.h"

/**
  \class QSplitterchild
  \brief in the QSplitter class, the method moveSplitter is protected, so it is
not possible to use connect between the QSplitter to synchronize them.
that's the reason for the creation of QSplitterchild.
*/
class QSplitterchild : public QSplitter
{
  Q_OBJECT

public:
  explicit QMEGAVTKADDON2_EXPORT QSplitterchild( QWidget* parent = 0 );
  explicit QMEGAVTKADDON2_EXPORT QSplitterchild ( Qt::Orientation orientation,
    QWidget* parent = 0 );
  QMEGAVTKADDON2_EXPORT virtual ~QSplitterchild();

public slots:
  QMEGAVTKADDON2_EXPORT void moveSplitter(int pos,int index);

protected:
  int prevpos;
  int previndex;
};

#endif
