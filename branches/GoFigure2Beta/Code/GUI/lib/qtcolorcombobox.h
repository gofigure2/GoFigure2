/****************************************************************************
**
** This file is part of a Qt Solutions component.
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H
#include <QtGui/QComboBox>
#include <QtGui/QColor>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTCOLORCOMBOBOX_EXPORT) && !defined(QT_QTCOLORCOMBOBOX_IMPORT)
#    define QT_QTCOLORCOMBOBOX_EXPORT
#  elif defined(QT_QTCOLORCOMBOBOX_IMPORT)
#    if defined(QT_QTCOLORCOMBOBOX_EXPORT)
#      undef QT_QTCOLORCOMBOBOX_EXPORT
#    endif
#    define QT_QTCOLORCOMBOBOX_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOLORCOMBOBOX_EXPORT)
#    undef QT_QTCOLORCOMBOBOX_EXPORT
#    define QT_QTCOLORCOMBOBOX_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOLORCOMBOBOX_EXPORT
#endif

class QT_QTCOLORCOMBOBOX_EXPORT QtColorComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit QtColorComboBox(QWidget *parent = 0, const char *name = 0);

    /** \brief insert a color at a given index*/
    void insertColor(int index, const QColor &color, const QString &name);

    /** \brief insert a color at the end, before the "more" if the ColorDialog
    is enabled*/
    inline void addColor(const QColor &color, const QString &name)
      { insertColor( colorCount(), color, name ); }

    int colorCount() const;

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;

    QColor color(int index) const;

    void setColorDialogEnabled(bool enabled = true);
    bool isColorDialogEnabled() const;

    void setCreationCollection(bool enabled = true);
    bool isCreationCollectionEnabled() const;

    void setStandardColors();

    int FindItemText(std::string Text);

    QSize sizeHint() const;

    /** \brief fill the list m_DataFromDB with the iDataColors*/
    void SetDataForColors(
      std::list<std::pair<std::string,std::vector<int> > > iDataColors);

    /** \brief return a vector with all the data describing the new color*/
    std::vector<std::string> GetDataForNewColorToBeSaved();

    /** \brief return the name and the QColor of the current selected color*/
    std::pair<std::string,QColor> GetCurrentColorData();

signals:
    void activated(const QColor &color);
    void highlighted(const QColor &color);
    /** \brief  signal emitted when a new color has been created by the user, in order
    to be saved in the database*/
    void NewColorToBeSaved();
    /** \brief  signal emitted when a new collection has been created by the user, in order
    to be saved in the database*/
    void NewCollectionToBeSaved();

private slots:
    void emitActivatedColor(int index);
    void emitHighlightedColor(int index);

private:
    QColor lastActivated;
    int numUserColors;
    /** \brief list of pair containing all the datas for the existing colors from the database
    with, for each color: a color name and a vector if rgba*/
    std::list<std::pair<std::string,std::vector<int> > > m_DataFromDB;
    bool colorDialogEnabled;
    /** \brief insert the colors with the data taken from the list m_DataFromDB */
    bool creationCollection;
    void setExistingColors();
    /** \brief fill the vector m_NewColorData with a QColor and a NameColor*/
    void StoreDataForNewColorToBeSaved(QColor Color,std::string NameColor);
    /** \brief vector containing the 5 datas describing a new color: its name and
    its rgba*/
    std::vector<std::string> m_NewColorData;
};

#endif

