#include "vtkLSMReader.h"
#include "vtkImageAppendComponents.h"
#include "vtkImageViewer2.h"
#include "vtkIndent.h"
#include "vtkImageData.h"
#include "vtkViewImage.h"
#include "vtkViewColorImage.h"

int main(int argc,char* argv[])
{
  if (argc != 2)
    {
    std::cout << "Usage: ";
    std::cout << "(exe) FileName";
    std::cout << std::endl;
    return 0;
    }  

  //read the LSM file:
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  int NumberOfTimePoints = reader->GetNumberOfTimePoints();
  int NumberOfChannels = reader->GetNumberOfChannels();

  for( int timePoint = 0;timePoint < NumberOfTimePoints; timePoint++)
    {
      {
        {
        vtkImageData* myImage_ch1;
        vtkImageData* myImage_ch2;
        vtkImageData* myImage_ch3;
      
        myImage_ch1 = reader->GetOutput();
 
        {
        vtkLSMReader* treader=vtkLSMReader::New();
        treader->SetFileName(argv[1]);
        treader->SetUpdateTimePoint( timePoint );
        treader->SetUpdateChannel( 1 );
        treader->Update();
        int * dimensions = treader->GetDimensions();
        int  flatindex = dimensions[0] * dimensions[1] * dimensions[2];
        myImage_ch2 = treader->GetOutput();
        if( NumberOfChannels == 1 ) 
          {
          char *ptr = (char*)( myImage_ch2->GetScalarPointer());
          for( int k=0; k < flatindex; k++ )
            {
            *ptr++ = 0; 
            }            
          }
        }
      
        {
        vtkLSMReader* treader=vtkLSMReader::New();
        treader->SetFileName(argv[1]);
        treader->SetUpdateTimePoint( timePoint );
        treader->SetUpdateChannel( 2 );
        treader->Update();
        int * dimensions = treader->GetDimensions();
        int  flatindex = dimensions[0] * dimensions[1] * dimensions[2];
        myImage_ch3 = treader->GetOutput();
        if( NumberOfChannels == 2 ) // dummy third channel 
          {
          char *ptr = (char*)( myImage_ch3->GetScalarPointer());
          for( int k=0; k < flatindex; k++ )
            {
            *ptr++ = 0; 
            }            
          }
        }
   
        vtkImageAppendComponents* appendFilter1 = vtkImageAppendComponents::New();
        appendFilter1->AddInput( myImage_ch1 );
        appendFilter1->AddInput( myImage_ch2 );
        appendFilter1->Update();

        vtkImageAppendComponents* appendFilter2 = vtkImageAppendComponents::New();
        appendFilter2->AddInput( appendFilter1->GetOutput() );
        appendFilter2->AddInput( myImage_ch3 );
        appendFilter2->Update(); 

        {
        std::cout << "vtkImageViewer2.:" << std::endl;
        vtkImageViewer2 * viewer = vtkImageViewer2::New();
        viewer->SetInput( appendFilter2->GetOutput() );
        viewer->Render();
        char buffer;
        cin >> buffer;
        }
        {
        std::cout << "vtkViewImage.:" << std::endl;
        vtkViewImage * viewer = vtkViewImage::New();
        viewer->SetInput( appendFilter2->GetOutput() );
        viewer->Render();
        char buffer;
        cin >> buffer;
        }
        {
        std::cout << "vtkViewColorImage.:" << std::endl;
        vtkViewColorImage * viewer = vtkViewColorImage::New();
        viewer->SetInput( appendFilter2->GetOutput() );
        viewer->Render();
        char buffer;
        cin >> buffer;
        }
        }
      } 
    } 
  return 1;
}
