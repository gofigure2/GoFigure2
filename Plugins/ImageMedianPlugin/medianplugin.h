/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#ifndef __medianplugin_h
#define __medianplugin_h

#include "QGoImageFilterPluginBase.h"
#include "QCellPreprocess.h"

class medianplugin : public QGoImageFilterPluginBase
  {
  Q_OBJECT
  Q_INTERFACES(QGoImageFilterPluginBase)

public :
    explicit medianplugin(QObject* iParent = 0x0);
  virtual ~medianplugin();

  virtual std::vector<QAction*> Actions();
  virtual QToolBar* ToolBar();
  virtual QMenu* Menu();
  virtual QDockWidget* DockWidget();
  virtual QWidget* AdditionalWidget();

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}

  virtual void OnTabActivated(const int&) {}
  virtual void OnTabMoved(const int&, const int&) {}
  virtual void OnTabClosed(const int&) {}

  virtual std::list<GoFigure::TabDimensionType> TabElementCompatibility() const;

protected:
  virtual void Process();
  virtual void SetAllRequiredInformation();

  QCellPreprocess* preprocess;

private:
  Q_DISABLE_COPY(medianplugin);
  };

#endif
