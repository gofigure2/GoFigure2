#ifndef __QGoLUTDialog_h
#define __QGoLUTDialog_h

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

// #include "ui_LUTDialog.h"

#include "QVTKWidget.h"

class QGoLUTDialog : public QDialog
{
  Q_OBJECT
public:
  QGoLUTDialog( QWidget* parent = 0 );
  virtual ~QGoLUTDialog();

  vtkLookupTable* GetLookupTable( );

public slots:
  void ChangeLookupTable( const int& );

protected:
  vtkLookupTable* LUT;
  vtkScalarBarActor* LUTActor;
  //   Ui::LUTDialog ui;
  QDialogButtonBox *buttonBox;
  QVTKWidget *qvtkWidget;
  QWidget *layoutWidget;
  QHBoxLayout *horizontalLayout;
  QLabel *label;
  QComboBox *LUTComboBox;

  void setupUi( QDialog *LUTDialog );
};
#endif
