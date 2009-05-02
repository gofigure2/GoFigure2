#include "vtkLSMReader.h"
#include "vtkImageAppendComponents.h"
#include "vtkImageViewer2.h"
#include "vtkIndent.h"
#include "vtkImageData.h"
#include "vtkViewImage.h"
#include "vtkViewImage2D.h"
#include "vtkViewImage2DWithContourWidget.h"

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

  {
  std::cout << "vtkImageViewer2.:" << std::endl;
  vtkImageViewer2 * viewer = vtkImageViewer2::New();
  viewer->SetInput( myImage3 );
  viewer->Render();
  char buffer;
  cin >> buffer;
  viewer->Delete();
  }

//   {
//   std::cout << "vtkViewImage.:" << std::endl;
//   vtkViewImage * viewer = vtkViewImage::New();
//   viewer->SetInput( myImage3 );
//   viewer->Render();
//   char buffer;
//   cin >> buffer;
//   viewer->Delete();
//   }

  {
  std::cout << "vtkViewImage2D.:" << std::endl;
  vtkViewImage2D * viewer = vtkViewImage2D::New();
  viewer->SetInput( myImage3 );
  viewer->Render();
  char buffer;
  cin >> buffer;
  viewer->Delete();
  }

  {
  std::cout << "vtkViewImage2DWithContourWidget:" << std::endl;
  vtkViewImage2DWithContourWidget* viewer = vtkViewImage2DWithContourWidget::New();
  viewer->SetInput( myImage3 );
  viewer->Render();
  char buffer;
  cin >> buffer;
  viewer->Delete();
  }
}
