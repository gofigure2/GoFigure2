/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 577 $  // Revision of last commit
  Date: $Date: 2009-08-18 10:45:50 -0400 (Tue, 18 Aug 2009) $  // Date of last commit
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

#include "QTextEditChild.h"
#include <QTextEdit>


//QTextEditChild::QTextEditChild( QWidget* iParent ): QTextEdit( iParent )
//{
//}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//QTextEditChild::
//QTextEditChild(const QString & iText, QWidget *iParent ):
 // QTextEdit( iText,iParent )
//{
//}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QTextEditChild::
QTextEditChild(QWidget* iParent, int iNumberMaxCharacters):
QTextEdit( iParent )
{
  this->m_MaxCharacters = iNumberMaxCharacters;
  QObject::connect(this,SIGNAL( textChanged()),
    this, SLOT(RestrainInputCharacters()));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QTextEditChild::~QTextEditChild()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QTextEditChild::RestrainInputCharacters()
{
  QString text;
  int leftChar = 0;
  int sizeText = this->toPlainText().size();
  leftChar = this->m_MaxCharacters - sizeText;
  if (leftChar < 0)
    {
    text = this->toPlainText().left(this->m_MaxCharacters);
    this->setText(text);
    this->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
    }
}