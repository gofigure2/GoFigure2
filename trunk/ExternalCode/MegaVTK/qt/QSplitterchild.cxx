#include "QSplitterchild.h"


QSplitterchild::QSplitterchild( QWidget* parent ): QSplitter( parent )
{
  prevpos=0;
  previndex=0;
}

QSplitterchild::QSplitterchild( Qt::Orientation orientation, QWidget * parent):QSplitter(orientation,parent)
{}

QSplitterchild::~QSplitterchild()
{}



void QSplitterchild::moveSplitter(int pos,int index)
{
  if ((prevpos != pos) || (previndex != index))
  {
    prevpos=pos;
    previndex=index;
    QSplitter::moveSplitter(pos, index);

  }
}
