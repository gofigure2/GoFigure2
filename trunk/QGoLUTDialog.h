#ifndef __QGoLUTDialog_h
#define __QGoLUTDialog_h

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QSpacerItem>

#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

#include "QVTKWidget.h"

/**
  \class QGoLUTDialog
  \brief Look-up Table Dialog
*/
class QGoLUTDialog : public QDialog
{
  Q_OBJECT
public:
  explicit QGoLUTDialog( QWidget* parent = 0 );
  virtual ~QGoLUTDialog();

  vtkLookupTable* GetLookupTable( );

public slots:
  void ChangeLookupTable( const int& );

protected:
  vtkLookupTable* LUT;
  vtkScalarBarActor* LUTActor;
  vtkRenderer*  Renderer;
  QDialogButtonBox* buttonBox;
  QVTKWidget* qvtkWidget;
  QHBoxLayout* horizontalLayout;
  QHBoxLayout* horizontalLayout_2;
  QVBoxLayout* verticalLayout;
  QSpacerItem* horizontalSpacer;
  QLabel* label;
  QComboBox* LUTComboBox;

  void setupUi( QDialog *LUTDialog );
};
#endif
