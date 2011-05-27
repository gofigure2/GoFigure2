#include "GoTransferFunctionWidget.h"

#include "hoverpoints.h"

//vtk
#include "vtkLookupTable.h"

//-------------------------------------------------------------------------

GoTransferFunctionWidget::GoTransferFunctionWidget(ShadeType type, QWidget *parent)
    : QWidget(parent), m_shade_type(type), m_alpha_gradient(QLinearGradient(0, 0, 0, 0))
{
  // Checkers background
  if (m_shade_type == ARGBShade) {
      QPixmap pm(20, 20);
      QPainter pmp(&pm);
      pmp.fillRect(0, 0, 10, 10, Qt::lightGray);
      pmp.fillRect(10, 10, 10, 10, Qt::lightGray);
      pmp.fillRect(0, 10, 10, 10, Qt::darkGray);
      pmp.fillRect(10, 0, 10, 10, Qt::darkGray);
      pmp.end();
      QPalette pal = palette();
      pal.setBrush(backgroundRole(), QBrush(pm));
      setAutoFillBackground(true);
      setPalette(pal);

  } else {
      setAttribute(Qt::WA_NoBackground);

  }

  m_hoverPoints = new HoverPoints(this, HoverPoints::CircleShape);
  m_hoverPoints->setConnectionType(HoverPoints::LineConnection);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  connect(m_hoverPoints, SIGNAL(pointsChanged(QPolygonF)), this, SIGNAL(colorsChanged()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QPolygonF GoTransferFunctionWidget::points() const
{
  return m_hoverPoints->points();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
uint GoTransferFunctionWidget::colorAt(int x)
{
  generateShade();

  QPolygonF pts = m_hoverPoints->points();
  for (int i=1; i < pts.size(); ++i) {
      if (pts.at(i-1).x() <= x && pts.at(i).x() >= x) {
          QLineF l(pts.at(i-1), pts.at(i));
          l.setLength(l.length() * ((x - l.x1()) / l.dx()));
          return m_shade.pixel(qRound(qMin(l.x2(), (qreal(m_shade.width() - 1)))),
                               qRound(qMin(l.y2(), qreal(m_shade.height() - 1))));
      }
  }
  return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionWidget::setGradientStops(const QGradientStops &stops)
{
  if (m_shade_type == ARGBShade) {
      m_alpha_gradient = QLinearGradient(0, 0, width(), 0);

      for (int i=0; i<stops.size(); ++i) {
          QColor c = stops.at(i).second;
          m_alpha_gradient.setColorAt(stops.at(i).first, QColor(c.red(), c.green(), c.blue()));
      }

      m_shade = QImage();
      generateShade();
      update();
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionWidget::paintEvent(QPaintEvent *)
{
  generateShade();

  QPainter p(this);
  p.drawImage(0, 0, m_shade);

  p.setPen(QColor(146, 146, 146));
  p.drawRect(0, 0, width() - 1, height() - 1);

  // draw histogram
  //if(m_Histogram.size() > 0)
    {
//    QPen pen;  // creates a default pen
//    pen.setWidth(3);
//    pen.setBrush(Qt::black);
//    pen.setCapStyle(Qt::RoundCap);
//    pen.setJoinStyle(Qt::RoundJoin);

//    p.setPen(pen);

      //get all points
      //int* extent = m_Histogram->GetOutput()->GetExtent();
      //m_Histogram->get


      // should be gloabl variable
      QVector<QPointF> listOfPoints;



      //p.drawLines();

    p.drawLine(0, 0, m_shade.width(), m_shade.height());
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoTransferFunctionWidget::generateShade()
{
  if (m_shade.isNull() || m_shade.size() != size()) {

      if (m_shade_type == ARGBShade) {
          m_shade = QImage(size(), QImage::Format_ARGB32_Premultiplied);
          m_shade.fill(0);

          QPainter p(&m_shade);
          p.fillRect(rect(), m_alpha_gradient);

          p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
          QLinearGradient fade(0, 0, 0, height());
          fade.setColorAt(0, QColor(0, 0, 0, 255));
          fade.setColorAt(1, QColor(0, 0, 0, 0));
          p.fillRect(rect(), fade);

      } else {
          m_shade = QImage(size(), QImage::Format_RGB32);
          QLinearGradient shade(0, 0, 0, height());
          shade.setColorAt(1, Qt::black);

          if (m_shade_type == RedShade)
              shade.setColorAt(0, Qt::red);
          else if (m_shade_type == GreenShade)
              shade.setColorAt(0, Qt::green);
          else
              shade.setColorAt(0, Qt::blue);

          QPainter p(&m_shade);
          p.fillRect(rect(), shade);
      }
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
AddLockPoints(const double& iFirstPoint, const double& iLastPoint)
{
QPolygonF points;
points << QPointF(0, height()*(1 - iFirstPoint))
       << QPointF(width(), height()*(1-iLastPoint));
m_hoverPoints->setPoints(points);
m_hoverPoints->setPointLock(0, HoverPoints::LockToLeft);
m_hoverPoints->setPointLock(1, HoverPoints::LockToRight);
m_hoverPoints->setSortType(HoverPoints::XSort);

emit colorsChanged();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
UpdateLookupTable(vtkLookupTable* iLUT)
{
  iLUT->SetNumberOfTableValues(m_shade.width());
  iLUT->Build();

  for(int i=0; i<m_shade.width();++i)
    {
    QColor color(m_shade.pixel(i, 0));
    iLUT->SetTableValue(i, color.redF(), color.greenF(), color.blueF());
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoTransferFunctionWidget::
SetHistogram(QVector<qreal> iHistogram)
{
  m_Histogram = iHistogram;
}
//-------------------------------------------------------------------------
