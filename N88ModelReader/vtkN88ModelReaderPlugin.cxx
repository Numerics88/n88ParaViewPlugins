#include "vtkN88ModelReaderPlugin.h"
#include "vtkObjectFactory.h"
#include "vtkAlgorithm.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkSmartPointer.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtk_netcdf.h"

vtkStandardNewMacro (vtkN88ModelReaderPlugin);

// This can only be used in a function that can return VTK_ERROR.
// NOTE: This macro should be used very sparingly in the read.
// Anytime an error might conceivably be caused by a malformed file, a more
// informative error message should be returned.
#define NC_SAFE_CALL(x) \
{ \
  int sc_status = (x); \
  if (sc_status != NC_NOERR) \
    { \
    vtkErrorMacro(<< "NetCDF error " <<  nc_strerror(sc_status) << "."); \
    return VTK_ERROR; \
    } \
}

//----------------------------------------------------------------------------
vtkN88ModelReaderPlugin::vtkN88ModelReaderPlugin()
:
  FileName (NULL)
{
  this->ActiveSolution = NULL;
  this->ActiveProblem = NULL;
  this->ActivePart = NULL;
  this->SetNumberOfInputPorts (0);
}

//----------------------------------------------------------------------------
vtkN88ModelReaderPlugin::~vtkN88ModelReaderPlugin()
{
  this->SetFileName(NULL);
  this->SetActiveSolution(NULL);
  this->SetActiveProblem(NULL);
  this->SetActivePart(NULL);
}

//----------------------------------------------------------------------------
void vtkN88ModelReaderPlugin::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::RequestData (vtkInformation*,
                                   vtkInformationVector**,
                                   vtkInformationVector* outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  if (!output)
  {
    vtkErrorMacro("No output object.");
    return VTK_ERROR;
  }

  // Make sure we have a file to read.
  if (!this->FileName)
  {
    vtkErrorMacro("FileName not set.");
    return VTK_ERROR;
  }

  int status;
  int ncid;

  // Open input file.
  status = nc_open (this->FileName, NC_NOWRITE, &ncid);
  if (status != NC_NOERR)
    {
    vtkErrorMacro(<< "Unable to open file " << this->FileName
                  << " ; NetCDF error " <<  nc_strerror(status) << ".");
    return VTK_ERROR;
    }

  this->SetActiveSolution(NULL);
  this->SetActiveProblem(NULL);
  this->SetActivePart(NULL);

  status = this->ReadDataFromNetCDFFile (ncid, output);

  nc_close (ncid);

  return status;
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::ReadDataFromNetCDFFile
(
  int ncid,
  vtkUnstructuredGrid *model
)
{
  if (this->ReadAttributes(ncid, model) == VTK_ERROR) return VTK_ERROR;
  if (this->ReadProblem(ncid, model) == VTK_ERROR) return VTK_ERROR;
  if (this->ReadNodes(ncid, model) == VTK_ERROR) return VTK_ERROR;
  if (this->ReadElements(ncid, model) == VTK_ERROR) return VTK_ERROR;
  if (this->ReadSolutions(ncid, model) == VTK_ERROR) return VTK_ERROR;
  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::ReadAttributes
(
  int ncid,
  vtkUnstructuredGrid *model
)
{
  size_t att_len = 0;
  if (nc_inq_attlen(ncid, NC_GLOBAL, "Conventions", &att_len) != NC_NOERR)
    {
    vtkErrorMacro(<< "No Conventions attribute in NetCDF file.");
    return VTK_ERROR;
    }
  std::string buffer;
  buffer.resize(att_len);
  if (nc_get_att_text(ncid, NC_GLOBAL, "Conventions", &buffer[0]) != NC_NOERR)
    {
    vtkErrorMacro(<< "Error reading Conventions attribute in NetCDF file.");
    return VTK_ERROR;
    }
  if (buffer != "Numerics88/Finite_Element_Model-1.0")
    {
    vtkErrorMacro(<< "File not identified as Numerics88/Finite_Element_Model-1.0.");
    return VTK_ERROR;
    }

  if (nc_inq_attlen  (ncid, NC_GLOBAL, "ActiveSolution", &att_len) == NC_NOERR)
    {
    buffer.resize(att_len);
    if (nc_get_att_text (ncid, NC_GLOBAL, "ActiveSolution", &buffer[0]) != NC_NOERR)
      {
      vtkErrorMacro(<< "Error reading ActiveSolution attribute in NetCDF file.");
      return VTK_ERROR;
      }    
    this->SetActiveSolution(buffer.c_str());
    // Get ActiveProblem from Solution
    int solutions_ncid;
    if (nc_inq_ncid(ncid, "Solutions", &solutions_ncid) != NC_NOERR)
      {
      vtkErrorMacro(<< "Solutions group not found.");
      return VTK_ERROR;
      }
    int activeSolution_ncid;
    if (nc_inq_ncid(solutions_ncid, this->ActiveSolution, &activeSolution_ncid) != NC_NOERR)
      {
      vtkErrorMacro(<< "ActiveSolution group not found " << this->ActiveSolution << ".");
      return VTK_ERROR;
      }
    if (nc_inq_attlen  (activeSolution_ncid, NC_GLOBAL, "Problem", &att_len) != NC_NOERR)
      {
      vtkErrorMacro(<< "Problem attribute not found for specified ActiveSolution.");
      return VTK_ERROR;
      }    
    buffer.resize(att_len);
    if (nc_get_att_text (activeSolution_ncid, NC_GLOBAL, "Problem", &buffer[0]) != NC_NOERR)
      {
      vtkErrorMacro(<< "Error reading Problem in ActiveSolution group " << this->ActiveSolution << ".");
      return VTK_ERROR;
      }    
    this->SetActiveProblem(buffer.c_str());
    }
  else
    {
    // No ActiveSolution; try ActiveProblem
    if (nc_inq_attlen  (ncid, NC_GLOBAL, "ActiveProblem", &att_len) != NC_NOERR)
      {
      vtkErrorMacro(<< "Neither ActiveSolution nor ActiveProblem attribute not found in NetCDF file.");
      return VTK_ERROR;
      }    
    buffer.resize(att_len);
    if (nc_get_att_text (ncid, NC_GLOBAL, "ActiveProblem", &buffer[0]) != NC_NOERR)
      {
      vtkErrorMacro(<< "Error reading ActiveProblem attribute in NetCDF file.");
      return VTK_ERROR;
      }    
    this->SetActiveProblem(buffer.c_str());
    }

  int dimid;
  if (nc_inq_dimid (ncid, "Dimensionality", &dimid))
    {
    vtkErrorMacro(<< "Dimensionality dimension not found in NetCDF file.");
    return VTK_ERROR;
    }    
  size_t dimensions = 0;
  NC_SAFE_CALL( nc_inq_dimlen(ncid, dimid, &dimensions) );
  if (dimensions != 3)
    {
    vtkErrorMacro(<< "Only dimensionality of 3 currently supported.");
    return VTK_ERROR;
    }    
  
  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::ReadProblem
(
  int ncid,
  vtkUnstructuredGrid *model
)
{
  int problems_ncid;
  if (nc_inq_ncid(ncid, "Problems", &problems_ncid) != NC_NOERR)
    {
    vtkErrorMacro(<< "Problems group not found.");
    return VTK_ERROR;
    }

  int activeProblem_ncid;
  if (nc_inq_ncid(problems_ncid, this->ActiveProblem, &activeProblem_ncid) != NC_NOERR)
    {
    vtkErrorMacro(<< "ActiveProblem group not found " << this->ActiveProblem << ".");
    return VTK_ERROR;
    }

  size_t att_len = 0;
  std::string buffer;
  if (nc_inq_attlen  (activeProblem_ncid, NC_GLOBAL, "Part", &att_len) != NC_NOERR)
    {
    vtkErrorMacro(<< "Part attribute not found in NetCDF file.");
    return VTK_ERROR;
    }    
  buffer.resize(att_len);
  if (nc_get_att_text (activeProblem_ncid, NC_GLOBAL, "Part", &buffer[0]) != NC_NOERR)
    {
    vtkErrorMacro(<< "Unable to read Part attribute in Part group " << this->ActiveProblem << ".");
    return VTK_ERROR;
    }
  this->SetActivePart(buffer.c_str());

  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::ReadNodes
(
  int ncid,
  vtkUnstructuredGrid *model
)
{
  int parts_ncid;
  if (nc_inq_ncid(ncid, "Parts", &parts_ncid) != NC_NOERR)
    {
    vtkErrorMacro(<< "Parts group not found.");
    return VTK_ERROR;
    }
  int activePart_ncid;
  if (nc_inq_ncid (parts_ncid, this->GetActivePart(), &activePart_ncid) != NC_NOERR)
    {
    vtkErrorMacro(<< "ActivePart group not found " << this->GetActivePart() << ".");
    return VTK_ERROR;
    }

  int varid;
  if (nc_inq_varid (activePart_ncid, "NodeCoordinates", &varid) != NC_NOERR)
    {
    vtkErrorMacro(<< "Unable to find variable NodeCoordinates for Part " << this->GetActivePart() << ".");
    return VTK_ERROR;
    }
  int ndims = 0;
  NC_SAFE_CALL (nc_inq_varndims(activePart_ncid, varid, &ndims));
  if (ndims != 2)
    {
    vtkErrorMacro(<< "NodeCoordinates must be 2-dimensional.");
    return VTK_ERROR;
    }
  int dimids[2];
  NC_SAFE_CALL (nc_inq_vardimid(activePart_ncid, varid, dimids));
  size_t len = 0;
  NC_SAFE_CALL( nc_inq_dimlen(activePart_ncid, dimids[1], &len) );
  if (len != 3)
    {
    vtkErrorMacro(<< "Second dimension of NodeCoordinates must have length 3.");
    return VTK_ERROR;
    }
  NC_SAFE_CALL( nc_inq_dimlen(activePart_ncid, dimids[0], &len) );
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetDataTypeToFloat();
  points->SetNumberOfPoints(len);
  // The following call crashes on Linux with netCDF 4.2.  No idea why.
  // The nc_get_vara variation seems to be OK though.
//   NC_SAFE_CALL( nc_get_var_float(activePart_ncid, varid,
//                         reinterpret_cast<float*>(points->GetVoidPointer(0))));
  size_t start[2] = {0,0};
  size_t count[2];
  count[0] = len;
  count[1] = 3;
  NC_SAFE_CALL (nc_get_vara_float (activePart_ncid, varid, start, count,
                         reinterpret_cast<float*>(points->GetVoidPointer(0))));
  model->SetPoints(points);

  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::ReadElements
(
  int ncid,
  vtkUnstructuredGrid *model
)
{
  int parts_ncid;
  if (nc_inq_ncid(ncid, "Parts", &parts_ncid) != NC_NOERR)
    {
    vtkErrorMacro (<< "Parts group not found.");
    return VTK_ERROR;
    }
  int activePart_ncid;
  if (nc_inq_ncid (parts_ncid, this->GetActivePart(), &activePart_ncid) != NC_NOERR)
    {
    vtkErrorMacro (<< "ActivePart group " << this->GetActivePart() << " not found.");
    return VTK_ERROR;
    }
  int elements_ncid;
  if (nc_inq_ncid (activePart_ncid, "Elements", &elements_ncid) != NC_NOERR)
    {
    vtkErrorMacro (<< "Elements group not found for Part " << this->GetActivePart() << ".");
    return VTK_ERROR;
    }
  int hexahedrons_ncid;
  if (nc_inq_ncid (elements_ncid, "Hexahedrons", &hexahedrons_ncid) != NC_NOERR)
    {
    vtkErrorMacro (<< "Hexahedrons group not found in Elements group for Part " << this->GetActivePart() << ".");
    return VTK_ERROR;
    }

  int elementNumber_varid;
  if (nc_inq_varid (hexahedrons_ncid, "ElementNumber", &elementNumber_varid) != NC_NOERR)
    {
    vtkErrorMacro (<< "Unable to find variable ElementNumber for Hexahedrons group for Part " << this->GetActivePart() << ".");
    return VTK_ERROR;
    }
  int ndims = 0;
  NC_SAFE_CALL (nc_inq_varndims(hexahedrons_ncid, elementNumber_varid, &ndims));
  if (ndims != 1)
    {
    vtkErrorMacro (<< "ElementNumber variable in Hexahedrons group must be 1-dimensional.");
    return VTK_ERROR;
    }
  int dimid[2];
  NC_SAFE_CALL (nc_inq_vardimid(hexahedrons_ncid, elementNumber_varid, dimid));
  size_t elementNumber_len = 0;
  NC_SAFE_CALL (nc_inq_dimlen(hexahedrons_ncid, dimid[0], &elementNumber_len));
  std::vector<vtkIdType> ids_input;
  ids_input.resize(elementNumber_len);
  // The following call crashes on Linux with netCDF 4.2.  No idea why.
  // The nc_get_vara variation seems to be OK though.
//   NC_SAFE_CALL (nc_get_var_longlong(hexahedrons_ncid, elementNumber_varid, &ids_input[0]));
  size_t start[2] = {0,0};
  size_t count[2];
  count[0] = elementNumber_len;
  NC_SAFE_CALL (nc_get_vara_longlong(hexahedrons_ncid, elementNumber_varid, start, count, &ids_input[0]));
  for (vtkIdType i=0; i<elementNumber_len; ++i)
    {
    if (ids_input[i] != i+1)
      {
      vtkErrorMacro (<< "ElementNumbers must start at 1 and be consecutive.");
      return VTK_ERROR;
      }
    }
  ids_input.clear();  // no longer need the data.

  int nodeNumbers_varid;
  if (nc_inq_varid (hexahedrons_ncid, "NodeNumbers", &nodeNumbers_varid) != NC_NOERR)
    {
    vtkErrorMacro (<< "Unable to find variable NodeNumbers for Hexahedrons group for Part " << this->GetActivePart() << ".");
    return VTK_ERROR;
    }
  NC_SAFE_CALL (nc_inq_varndims(hexahedrons_ncid, nodeNumbers_varid, &ndims));
  if (ndims != 2)
    {
    vtkErrorMacro (<< "NodeNumbers variable in Hexahedrons group must be 2-dimensional.");
    return VTK_ERROR;
    }
  NC_SAFE_CALL (nc_inq_vardimid(hexahedrons_ncid, nodeNumbers_varid, dimid));
  size_t nodesPerElement = 0;
  NC_SAFE_CALL (nc_inq_dimlen(hexahedrons_ncid, dimid[1], &nodesPerElement));
  if (nodesPerElement != 8)
    {
    vtkErrorMacro (<< "NodeNumbers variable must have second dimension size of 8 in Hexahedrons.");
    return VTK_ERROR;
    }  
  size_t nodeNumbers_len = 0;
  NC_SAFE_CALL (nc_inq_dimlen(hexahedrons_ncid, dimid[0], &nodeNumbers_len));
  if (nodeNumbers_len != elementNumber_len)
    {
    vtkErrorMacro(<< "ElementNumber and NodeNumbers in Elements group must have same length.");
    return VTK_ERROR;
    }
  ids_input.resize(nodeNumbers_len*nodesPerElement);
  // The following call crashes on Linux with netCDF 4.2.  No idea why.
  // The nc_get_vara variation seems to be OK though.
//   NC_SAFE_CALL (nc_get_var_longlong(hexahedrons_ncid, nodeNumbers_varid, &ids_input[0]));
  count[0] = nodeNumbers_len;
  count[1] = nodesPerElement;
  NC_SAFE_CALL (nc_get_vara_longlong(hexahedrons_ncid, nodeNumbers_varid, start, count, &ids_input[0]));
  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
  // The following allocation is exact
  cells->Allocate(cells->EstimateSize(nodeNumbers_len, nodesPerElement));
  vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
  pointIds->SetNumberOfIds(nodesPerElement);    
  for (vtkIdType newCellId=0; newCellId < nodeNumbers_len; ++newCellId)
    {
    // Convert from 1-indexed to 0-indexed.
    for (int j=0; j<nodesPerElement ; ++j)
      {
      pointIds->SetId(j, ids_input[nodesPerElement*newCellId + j] - 1);
      }
    cells->InsertNextCell(pointIds);
    }
  ids_input.clear();  // no longer need the data.
  model->SetCells(VTK_VOXEL, cells);

  int materialID_varid;
  if (nc_inq_varid (hexahedrons_ncid, "MaterialID", &materialID_varid) != NC_NOERR)
    {
    vtkWarningMacro (<< "Unable to find variable MaterialID for Hexahedrons group for Part " << this->GetActivePart() << ".");
    return VTK_OK;
    }
  NC_SAFE_CALL (nc_inq_varndims(hexahedrons_ncid, materialID_varid, &ndims));
  if (ndims != 1)
    {
    vtkErrorMacro (<< "MaterialID variable in Hexahedrons group must be 1-dimensional.");
    return VTK_ERROR;
    }
  NC_SAFE_CALL (nc_inq_vardimid(hexahedrons_ncid, materialID_varid, dimid));
  size_t materialID_len = 0;
  NC_SAFE_CALL (nc_inq_dimlen(hexahedrons_ncid, dimid[0], &materialID_len));
  if (materialID_len != elementNumber_len)
    {
    vtkErrorMacro(<< "ElementNumber and MaterialID in Elements group must have same length.");
    return VTK_ERROR;
    }
  vtkSmartPointer<vtkIntArray> scalars = vtkSmartPointer<vtkIntArray>::New();
  scalars->SetName("MaterialID");
  scalars->SetNumberOfTuples(materialID_len);
  // The following call crashes on Linux with netCDF 4.2.  No idea why.
  // The nc_get_vara variation seems to be OK though.
//   NC_SAFE_CALL (nc_get_var_int(hexahedrons_ncid, materialID_varid, scalars->GetPointer(0)));
  count[0] = materialID_len;
  NC_SAFE_CALL (nc_get_vara_int(hexahedrons_ncid, materialID_varid, start, count, scalars->GetPointer(0)));
  model->GetCellData()->SetScalars(scalars);

  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkN88ModelReaderPlugin::ReadSolutions
(
  int ncid,
  vtkUnstructuredGrid *model
)
{
  // Do nothing if there is no active solution in the data file.
  if (this->ActiveSolution == NULL)
    { return VTK_OK; }

  int solutions_ncid;
  NC_SAFE_CALL (nc_inq_ncid(ncid, "Solutions", &solutions_ncid));
  int activeSolution_ncid;
  NC_SAFE_CALL (nc_inq_ncid(solutions_ncid, this->ActiveSolution, &activeSolution_ncid));

  // Node Values
  int nodevalues_ncid;
  if (nc_inq_ncid(activeSolution_ncid, "NodeValues", &nodevalues_ncid) == NC_NOERR)
    {
    int nvars;
    NC_SAFE_CALL (nc_inq_varids(nodevalues_ncid, &nvars, NULL));
    std::vector<int> varids;
    varids.resize(nvars);
    NC_SAFE_CALL (nc_inq_varids(nodevalues_ncid, NULL, &varids.front()));
    char name[NC_MAX_NAME+1];
    for (int v=0; v<nvars; ++v)
      {
      NC_SAFE_CALL (nc_inq_varname(nodevalues_ncid, varids[v], name));
      vtkSmartPointer<vtkFloatArray> data = vtkSmartPointer<vtkFloatArray>::New();
      data->SetName(name);
      int ndims = 0;
      int dimids[2];
      size_t dims[2];
      NC_SAFE_CALL (nc_inq_varndims(nodevalues_ncid, varids[v], &ndims));
      if (ndims > 2)
        {
        vtkErrorMacro (<< "Variables in NodeValues group must be 1 or 2 dimensional.");
        return VTK_ERROR;
        }
      NC_SAFE_CALL (nc_inq_vardimid(nodevalues_ncid, varids[v], dimids));
      NC_SAFE_CALL (nc_inq_dimlen(nodevalues_ncid, dimids[0], &dims[0]));
      dims[1] = 1;
      if (ndims == 2)
        {
        NC_SAFE_CALL (nc_inq_dimlen(nodevalues_ncid, dimids[1], &dims[1]));
        }
      data->SetNumberOfComponents(dims[1]);
      if (dims[0] != model->GetNumberOfPoints())
        {
        vtkErrorMacro (<< "Dimensions of variable " << name << " in NodeValues does not match number of nodes in model.");
        return VTK_ERROR;
        }
      data->SetNumberOfTuples(dims[0]);
      // The following call crashes on Linux with netCDF 4.2.  No idea why.
      // The nc_get_vara variation seems to be OK though.
//       NC_SAFE_CALL (nc_get_var_float(nodevalues_ncid, varids[v], data->GetPointer(0)));
      size_t start[2] = {0,0};
      NC_SAFE_CALL (nc_get_vara_float(nodevalues_ncid, varids[v], start, dims, data->GetPointer(0)));
      model->GetPointData()->AddArray(data);
      }
    }

  // Element Values
  int elementvalues_ncid;
  if (nc_inq_ncid(activeSolution_ncid, "ElementValues", &elementvalues_ncid) == NC_NOERR)
    {
    int nvars;
    NC_SAFE_CALL (nc_inq_varids(elementvalues_ncid, &nvars, NULL));
    std::vector<int> varids;
    varids.resize(nvars);
    NC_SAFE_CALL (nc_inq_varids(elementvalues_ncid, NULL, &varids.front()));
    char name[NC_MAX_NAME+1];
    for (int v=0; v<nvars; ++v)
      {
      NC_SAFE_CALL (nc_inq_varname(elementvalues_ncid, varids[v], name));
      vtkSmartPointer<vtkFloatArray> data = vtkSmartPointer<vtkFloatArray>::New();
      data->SetName(name);
      int ndims = 0;
      int dimids[2];
      size_t dims[2];
      NC_SAFE_CALL (nc_inq_varndims(elementvalues_ncid, varids[v], &ndims));
      if (ndims > 2)
        {
        vtkErrorMacro (<< "Variables in ElementValues group must be 1 or 2 dimensional.");
        return VTK_ERROR;
        }
      NC_SAFE_CALL (nc_inq_vardimid(elementvalues_ncid, varids[v], dimids));
      NC_SAFE_CALL (nc_inq_dimlen(elementvalues_ncid, dimids[0], &dims[0]));
      dims[1] = 1;
      if (ndims == 2)
        {
        NC_SAFE_CALL (nc_inq_dimlen(elementvalues_ncid, dimids[1], &dims[1]));
        }
      data->SetNumberOfComponents(dims[1]);
      if (dims[0] != model->GetNumberOfCells())
        {
        vtkErrorMacro (<< "Dimensions of variable " << name << " in ElementValues does not match number of elements in model.");
        return VTK_ERROR;
        }
      data->SetNumberOfTuples(dims[0]);
      // The following call crashes on Linux with netCDF 4.2.  No idea why.
      // The nc_get_vara variation seems to be OK though.
//       NC_SAFE_CALL (nc_get_var_float(elementvalues_ncid, varids[v], data->GetPointer(0)));
      size_t start[2] = {0,0};
      NC_SAFE_CALL (nc_get_vara_float(elementvalues_ncid, varids[v], start, dims, data->GetPointer(0)));
      model->GetCellData()->AddArray(data);
      }
    }

  return VTK_OK;
}
