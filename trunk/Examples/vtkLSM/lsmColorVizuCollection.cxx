#include "vtkLSMReader.h"
#include "vtkImageAppendComponents.h"
#include "vtkIndent.h"
#include "vtkImageData.h"
#include "vtkViewImage2DWithContourWidget.h"
#include "vtkViewImage2DWithContourWidgetCollection.h"
#include "vtkRenderWindowInteractor.h"

int main(int argc,char* argv[])
{
  if (argc != 2)
    {
    std::cout << "Usage: ";
    std::cout << "(exe) FileName";
    std::cout << std::endl;
    return 0;
    }

  vtkImageData* myImage_ch1 = vtkImageData::New();
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();
  int NumberOfChannels = reader->GetNumberOfChannels();
  std::cout << reader << std::endl;
  myImage_ch1->ShallowCopy( reader->GetOutput() );
  reader->Delete();

  vtkImageData* myImage_ch2 = vtkImageData::New();
  if( NumberOfChannels == 1 )
    {
    myImage_ch2->ShallowCopy( myImage_ch1 );
    }
  else
    {
    vtkLSMReader* reader2=vtkLSMReader::New();
    reader2->SetFileName(argv[1]);
    reader2->SetUpdateChannel( 1 );
    reader2->Update();
    myImage_ch2->ShallowCopy( reader2->GetOutput() );
    reader2->Delete();
    }

  vtkImageData* myImage2 = vtkImageData::New();
  vtkImageAppendComponents* appendFilter1 = vtkImageAppendComponents::New();
  appendFilter1->AddInput( myImage_ch1 );
  appendFilter1->AddInput( myImage_ch2 );
  appendFilter1->Update();
  myImage2->ShallowCopy( appendFilter1->GetOutput() );
  appendFilter1->Delete();
  myImage_ch2->Delete();


  vtkImageData* myImage_ch3 = vtkImageData::New();
  if( NumberOfChannels == 2 )
    {
  myImage_ch3->ShallowCopy( myImage_ch1 );
    }
  else
    {
  vtkLSMReader* reader3=vtkLSMReader::New();
    reader3->SetFileName(argv[1]);
    reader3->SetUpdateChannel( 2 );
    reader3->Update();
    myImage_ch3->ShallowCopy( reader3->GetOutput() );
    reader3->Delete();
    }
  myImage_ch1->Delete();

  vtkImageData* myImage3 = vtkImageData::New();
  vtkImageAppendComponents* appendFilter2 = vtkImageAppendComponents::New();
  appendFilter2->AddInput( myImage2    );
  appendFilter2->AddInput( myImage_ch3 );
  appendFilter2->Update();
  myImage3->ShallowCopy( appendFilter2->GetOutput() );
  appendFilter2->Delete();
  myImage2->Delete();
  myImage_ch3->Delete();

  vtkViewImage2DWithContourWidgetCollection* pool =
    vtkViewImage2DWithContourWidgetCollection::New();

  vtkRenderWindowInteractor* iren1 = vtkRenderWindowInteractor::New();
  vtkViewImage2DWithContourWidget* viewer1 = vtkViewImage2DWithContourWidget::New();
  viewer1->SetupInteractor( iren1 );
  viewer1->SetInput( myImage3 );
  viewer1->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_AXIAL);
  viewer1->Reset();

  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  vtkViewImage2DWithContourWidget* viewer2 = vtkViewImage2DWithContourWidget::New();
  viewer2->SetupInteractor( iren2 );
  viewer2->SetInput( myImage3 );
  viewer2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
  viewer2->Reset();

  vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
  vtkViewImage2DWithContourWidget* viewer3 = vtkViewImage2DWithContourWidget::New();
  viewer3->SetupInteractor( iren3 );
  viewer3->SetInput( myImage3 );
  viewer3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);
  viewer3->Reset();

  pool->AddItem( viewer1 );
  pool->AddItem( viewer2 );
  pool->AddItem( viewer3 );

  viewer1->Delete();
  viewer2->Delete();
  viewer3->Delete();
  iren1->Delete();
  iren2->Delete();
  iren3->Delete();

  pool->Initialize();
  pool->SyncRender();

  char buffer;
  cin >> buffer;

  pool->Delete();

  return 0;
}
