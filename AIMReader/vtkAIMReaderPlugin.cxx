#include "vtkAIMReaderPlugin.h"
#include "vtkboneAIMReader.h"
#include "vtkObjectFactory.h"
#include "vtkAlgorithm.h"
#include "vtkImageData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkImageCast.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkImageConstantPad.h"

// If this is defined, then input data will be converted to float.
// #define AIMREADER_CONVERT_TO_FLOAT

vtkStandardNewMacro (vtkAIMReaderPlugin);

//----------------------------------------------------------------------------
vtkAIMReaderPlugin::vtkAIMReaderPlugin()
:
  FileName (NULL),
  Pad (0),
  DataOnCells (1)
{
  this->SetNumberOfInputPorts (0);
}

//----------------------------------------------------------------------------
vtkAIMReaderPlugin::~vtkAIMReaderPlugin()
{
  this->SetFileName(NULL);
}

//----------------------------------------------------------------------------
void vtkAIMReaderPlugin::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkAIMReaderPlugin::RequestInformation (
  vtkInformation       * vtkNotUsed( request ),
  vtkInformationVector** vtkNotUsed( inputVector ),
  vtkInformationVector * outputVector)
{
  // Make sure FileName is set
  if (!this->FileName)
  {
    vtkErrorMacro("A Filename must be specified");
    return 0;
  }

  vtkSmartPointer<vtkboneAIMReader> reader = vtkSmartPointer<vtkboneAIMReader>::New();

  reader->SetFileName (FileName);
  reader->SetDataOnCells (DataOnCells);
  reader->UpdateInformation();
  vtkInformation* readerInfo = reader->GetOutputInformation(0);

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  int extent[6];
  readerInfo->Get (vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
  if (Pad && !DataOnCells)
    {
    extent[0] -= 1;
    extent[1] += 1;
    extent[2] -= 1;
    extent[3] += 1;
    extent[4] -= 1;
    extent[5] += 1;
    }
  outInfo->Set (vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);

  double spacing[3];
  readerInfo->Get (vtkDataObject::SPACING(), spacing);
  outInfo->Set (vtkDataObject::SPACING(), spacing, 3);
  double origin[3];
  readerInfo->Get (vtkDataObject::ORIGIN(), origin);
  outInfo->Set (vtkDataObject::ORIGIN(), origin, 3);

#ifdef AIMREADER_CONVERT_TO_FLOAT
  if (DataOnCells)
    {
    vtkDataObject::SetActiveAttributeInfo(outInfo, vtkDataObject::FIELD_ASSOCIATION_CELLS,
      vtkDataSetAttributes::SCALARS, NULL, VTK_FLOAT, 1, -1);
    }
  else
    {
    vtkDataObject::SetPointDataActiveScalarInfo (outInfo, VTK_FLOAT, 1);
    }
#else
  if (DataOnCells)
    {
    vtkInformation *attrInfo = vtkDataObject::GetActiveFieldInformation(readerInfo,
      vtkDataObject::FIELD_ASSOCIATION_CELLS, vtkDataSetAttributes::SCALARS);
    if (!attrInfo)
      {
      vtkErrorMacro(<< "AIM Reader did not produce active field information.");
      return 0;
      }
    if (!attrInfo->Has(vtkDataObject::FIELD_ARRAY_TYPE()))
      {
      vtkErrorMacro(<< "Data type unidentified by AIM Reader.");
      return 0;
      }
    int dataType = attrInfo->Get(vtkDataObject::FIELD_ARRAY_TYPE());
    vtkDataObject::SetActiveAttributeInfo(outInfo, vtkDataObject::FIELD_ASSOCIATION_CELLS,
      vtkDataSetAttributes::SCALARS, NULL, dataType, 1, -1);
    }
  else
    {
    vtkInformation *attrInfo = vtkDataObject::GetActiveFieldInformation(readerInfo,
      vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
    if (!attrInfo)
      {
      vtkErrorMacro(<< "AIM Reader did not produce active field information.");
      return 0;
      }
    if (!attrInfo->Has(vtkDataObject::FIELD_ARRAY_TYPE()))
      {
      vtkErrorMacro(<< "Data type unidentified by AIM Reader.");
      return 0;
      }
    int dataType = attrInfo->Get(vtkDataObject::FIELD_ARRAY_TYPE());
    vtkDataObject::SetPointDataActiveScalarInfo (outInfo, dataType, 1);
    }
#endif
  return 1;
}

//----------------------------------------------------------------------------
int vtkAIMReaderPlugin::RequestData (vtkInformation*,
                                     vtkInformationVector**,
                                     vtkInformationVector* outputVector)
{

  vtkSmartPointer<vtkboneAIMReader> reader = vtkSmartPointer<vtkboneAIMReader>::New();
  reader->SetFileName (FileName);
  reader->SetDataOnCells (DataOnCells);
  reader->Update();

  vtkImageData* out = vtkImageData::GetData (outputVector);
  if (!out)
  {
    return 0;
  }

#ifdef AIMREADER_CONVERT_TO_FLOAT
  vtkSmartPointer<vtkImageCast> caster = vtkSmartPointer<vtkImageCast>::New();
  caster->SetInputConnection (reader->GetOutputPort());
  caster->SetOutputScalarType (VTK_FLOAT);
  caster->Update();
#endif
  if (Pad && !DataOnCells)
    {
    vtkSmartPointer<vtkImageConstantPad> padder =
                                   vtkSmartPointer<vtkImageConstantPad>::New();
#ifdef AIMREADER_CONVERT_TO_FLOAT
    padder->SetInputConnection (caster->GetOutputPort());
    int extent[6];
    caster->GetOutput()->GetExtent(extent);
#else
    padder->SetInputConnection (reader->GetOutputPort());
    int extent[6];
    reader->GetOutput()->GetExtent(extent);
#endif
    padder->SetOutputWholeExtent (extent[0]-1, extent[1]+1,
                                  extent[2]-1, extent[3]+1,
                                  extent[4]-1, extent[5]+1);
    padder->SetConstant(0);
    padder->Update();
    out->ShallowCopy(padder->GetOutput());
    }
  else
    {
#ifdef AIMREADER_CONVERT_TO_FLOAT
    out->ShallowCopy(caster->GetOutput());
#else
    out->ShallowCopy(reader->GetOutput());
#endif
    }

  return 1;
}
