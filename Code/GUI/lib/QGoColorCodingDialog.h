/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

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

#ifndef __QGoColorCodingDialog_h
#define __QGoColorCodingDialog_h

#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QRadioButton>
#include "QGoGUILibConfigure.h"
#include "QGoLUTDialog.h"

/**
\class QGoColorCodingDialog
\brief This dialog asks the user to chose the way he wants its
traces to be colorcoded
\ingroup GUI
*/
class QGOGUILIB_EXPORT QGoColorCodingDialog:public QDialog
{
  Q_OBJECT

public:

  /**
  \brief Constructor
  \param[in] iTraceName name of the trace to be colorcoded
  \param[in] iParent parent of the widget
  \param[in] iRandomIncluded true if the random option needs to be included
 */
  explicit QGoColorCodingDialog(std::string iTraceName, bool iRandomIncluded,
    QWidget *iParent = 0);

  enum ColorWay{Nothing,Default,Random,LUT};

   /** \brief Destructor */
  virtual ~QGoColorCodingDialog();

  /**
  \brief get the way the user wants its traces to be colorcoded
  and the LUT if he chooses the LUT
  \param[in] iTraceName name of the trace to be colorcoded
  \param[in,out] ioLUT lookup table choosen by the user
  \param[in] iiParent parent of the widget
  \param[in] iRandomIncluded true if the random option needs to be included
  */
  static ColorWay GetColorWay( std::string iTraceName,
   vtkLookupTable **ioLUT, bool iRandomIncluded, QWidget *iiParent = 0 );

protected:
  QRadioButton*   m_DefaultButton;
  QRadioButton*   m_LUTButton;
  QRadioButton*   m_RandomButton;
  vtkLookupTable* m_LUT;
  /**
  \brief set the components of the widget and the connection
  \param[in] iTraceName name of the trace to be colorcoded
  \param[in] iRandomIncluded true if the random option needs to be included
  */
  void SetUpUi(std::string iTraceName, bool iRandomIncluded);

protected slots:
  /**
  \brief open tha QGoLUTDialog to let the user choose a LUT
  */
  void OpenLUTDialog();
private:
  Q_DISABLE_COPY( QGoColorCodingDialog );
};
#endif
