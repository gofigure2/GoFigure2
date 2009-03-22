#ifndef __QSplitterchild_h
#define __QSplitterchild_h

#include <QSplitter>

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
  explicit QSplitterchild( QWidget* parent = 0 );
  explicit QSplitterchild ( Qt::Orientation orientation, QWidget * parent = 0 );
  virtual ~QSplitterchild();

public slots:
  void moveSplitter(int pos,int index);

protected:
  int prevpos;
  int previndex;
};

#endif
