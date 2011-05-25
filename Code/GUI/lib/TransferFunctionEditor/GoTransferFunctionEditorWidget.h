/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef __GoTransferFunctionEditorWidget_h
#define __GoTransferFunctionEditorWidget_h

#include "QGoGUILibConfigure.h"

#include <QtGui>

class HoverPoints;
class GoTransferFunctionWidget;

class GoTransferFunctionEditorWidget : public QWidget
{
  Q_OBJECT
public:
  GoTransferFunctionEditorWidget(QWidget *parent);

  void setGradientStops(const QGradientStops &stops);

public slots:
  void pointsUpdated();

signals:
  void gradientStopsChanged(const QGradientStops &stops);

private:
  GoTransferFunctionWidget *m_red_shade;
  GoTransferFunctionWidget *m_green_shade;
  GoTransferFunctionWidget *m_blue_shade;
  GoTransferFunctionWidget *m_alpha_shade;
};

#endif
