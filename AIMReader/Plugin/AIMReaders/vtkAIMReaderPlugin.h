/*=========================================================================

$Id$

This is a wrapper for the VTK class vtkboneAIMReader, allowing it
to be used as a plugin in ParaView.

This wrapper modifies vtkboneAIMReader as follows:
  - Converts the data type to float, regardless of the original data type
    in the AIM file.  This was done because Paraview handles char types
    poorly (is inconsistent with signed/unsigned).
  - Adds an option to pad the AIM data with one layer of zero-valued voxels
    on all faces.  This was added to this reader because it is currently
    impossible to implement a padding filter in ParaView.  See
    http://markmail.org/message/sfnw7sgojytzw6y2

=========================================================================*/

#ifndef __vtkAIMReaderPlugin_h
#define __vtkAIMReaderPlugin_h

#include "vtkImageAlgorithm.h"

class VTK_EXPORT vtkAIMReaderPlugin :
  public vtkImageAlgorithm
{
public:
  static vtkAIMReaderPlugin* New();
  vtkTypeMacro (vtkAIMReaderPlugin,vtkImageAlgorithm);
  void PrintSelf (ostream& os, vtkIndent indent) override;

  vtkSetStringMacro (FileName);
  vtkGetStringMacro (FileName);

  // Description:
  // Whether to pad the AIM data with one additional voxel on each face.  Default is on.
  vtkSetMacro(Pad,int);
  vtkGetMacro(Pad,int);
  vtkBooleanMacro(Pad,int);

  // Description:
  // Whether to place data on Points or Cells.
  vtkSetMacro(DataOnCells,int);
  vtkGetMacro(DataOnCells,int);
  vtkBooleanMacro(DataOnCells,int);

protected:
  vtkAIMReaderPlugin();
  ~vtkAIMReaderPlugin();

  int RequestInformation (vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector) override;
  int RequestData (vtkInformation*,
                   vtkInformationVector**,
                   vtkInformationVector*) override;

  char * FileName;
  int Pad;
  int DataOnCells;  // Flag to put data on cells instead of points.

private:
  // Prevent compiler from making public versions of these.
  vtkAIMReaderPlugin (const vtkAIMReaderPlugin&);
  void operator= (const vtkAIMReaderPlugin&);
};

#endif  //
