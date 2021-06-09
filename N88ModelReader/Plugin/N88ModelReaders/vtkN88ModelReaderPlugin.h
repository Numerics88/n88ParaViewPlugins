/*=========================================================================

This is a copy of the code from vtkn88N88ModelReader, but without
bits that aren't relevant to ParaView.

=========================================================================*/

#ifndef __vtkN88ModelReaderPlugin_h
#define __vtkN88ModelReaderPlugin_h

#include "vtkUnstructuredGridAlgorithm.h"

// forward declarations
class vtkUnstructuredGrid;

class VTK_EXPORT vtkN88ModelReaderPlugin :
  public vtkUnstructuredGridAlgorithm
{
public:
  static vtkN88ModelReaderPlugin* New();
  vtkTypeMacro(vtkN88ModelReaderPlugin,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Get the active problem name.
  vtkGetStringMacro(ActiveSolution);

  // Description:
  // Get the active problem name.
  // Note that if an ActiveSolution exists in the file, then the ActiveProblem
  // will be set from that, and not from the ActiveProblem attribute.
  vtkGetStringMacro(ActiveProblem);

  // Description:
  // Get the active part name used by the ActiveProblem.
  vtkGetStringMacro(ActivePart);

protected:
  vtkN88ModelReaderPlugin();
  ~vtkN88ModelReaderPlugin();

  char * FileName;

  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;

  int ReadDataFromNetCDFFile(int ncid, vtkUnstructuredGrid* model);
  int ReadProblem(int ncid, vtkUnstructuredGrid* model);
  int ReadAttributes(int ncid, vtkUnstructuredGrid* model);
  int ReadNodes(int ncid, vtkUnstructuredGrid* model);
  int ReadElements(int ncid, vtkUnstructuredGrid* model);
  int ReadSolutions(int ncid, vtkUnstructuredGrid* model);

  // not publically modifiable
  vtkSetStringMacro(ActiveSolution);
  vtkSetStringMacro(ActiveProblem);
  vtkSetStringMacro(ActivePart);

  char* ActiveSolution;
  char* ActiveProblem;
  char* ActivePart;

private:
  // Prevent compiler from making public versions of these.
  vtkN88ModelReaderPlugin (const vtkN88ModelReaderPlugin&);
  void operator= (const vtkN88ModelReaderPlugin&);
};

#endif  //
