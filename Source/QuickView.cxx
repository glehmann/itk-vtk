/* =========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "QuickView.h"

#include "itkImage.h"

#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderer.h"

#include <itkImageToVTKImageFilter.h>

void QuickView::AddImage(itk::Image<unsigned char, 2>::Pointer  image)
{
  this->Images.push_back(image);
}

void QuickView::Visualize()
{
  // Setup the render window
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  unsigned int rendererSize = 300;
  renderWindow->SetSize(rendererSize * this->Images.size(), rendererSize);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  // Render all of the images
  double step = 1./(static_cast<double>(this->Images.size()));
  std::vector<double*> viewports;

  typedef itk::ImageToVTKImageFilter<itk::Image<unsigned char, 2> > ConnectorType;
  std::vector<ConnectorType::Pointer> connectors; // Force the connectors to persist (not lose scope) after each iteration of the loop

  for(unsigned int i = 0; i < this->Images.size(); i++)
    {
    ConnectorType::Pointer connector = ConnectorType::New();
    connectors.push_back(connector);
    connector->SetInput(this->Images[i]);
    connector->Update();

    // (xmin, ymin, xmax, ymax)
    double viewport[4] = {static_cast<double>(i)*step, 0.0, static_cast<double>(i+1)*step, 1.0};
    viewports.push_back(viewport);
    vtkSmartPointer<vtkImageActor> actor =
      vtkSmartPointer<vtkImageActor>::New();
    actor->SetInput(connector->GetOutput());

    // Setup both renderers
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
    renderer->SetViewport(viewports[i]);
    //renderer->SetBackground(.6, .5, .4);

    renderer->AddActor(actor);
    renderer->ResetCamera();
    }

  renderWindow->Render();

  vtkSmartPointer<vtkInteractorStyleImage> style =
    vtkSmartPointer<vtkInteractorStyleImage>::New();
  interactor->SetInteractorStyle(style);
  interactor->Start();
}
