#ifndef __MOUSEINTERACTORSTYLE_H__
#define __MOUSEINTERACTORSTYLE_H__
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkIdTypeArray.h>
#include <vtkTriangleFilter.h>
#include <vtkTetra.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPlaneSource.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkObjectFactory.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointPicker.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkPolyData.h>


//#include "igl/colon.h"
//#include "igl/directed_edge_orientations.h"
//#include "igl/directed_edge_parents.h"
//#include "igl/forward_kinematics.h"
//#include "igl/PI.h"
//#include "igl/lbs_matrix.h"
//#include "igl/deform_skeleton.h"
//#include "igl/dqs.h"
//#include "igl/readDMAT.h"
//#include "igl/readOFF.h"
//#include "igl/arap.h"

//#include <Eigen/Geometry>
//#include <Eigen/StdVector>
//#include <Eigen/Core>
//#include <Eigen/Eigen>
//#include <Eigen/Dense>
//#include <Eigen/Sparse>
//#include <Eigen/Cholesky>
//#include <Eigen/LU>
//#include <Eigen/SVD>
//#include <Eigen/SparseCore>
enum DataType
{
    UNSTRUCTED_GRID,
    POLYDATA
};

enum InteractorStyle
{
    Interactor_Style_TrackballActor, Interactor_Style_TrackballCamera, Interactor_Style_RubberBandPick
};

void switchInteractorStyle(InteractorStyle currentStyle = Interactor_Style_RubberBandPick);

// Catch mouse events
class InteractorStyleTrackballCamera: public vtkInteractorStyleTrackballCamera
{
public:
    static InteractorStyleTrackballCamera* New();

    InteractorStyleTrackballCamera();
    virtual void OnKeyPress();
    virtual void OnLeftButtonDown();
    void SetDataType(const DataType dataType);
    void SetData(vtkSmartPointer<vtkUnstructuredGrid> data);
    void SetData(vtkSmartPointer<vtkPolyData> data);

    vtkSmartPointer<vtkUnstructuredGrid> vtkUnstructuredGridData;
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkDataSetMapper> selectedMapper;
    vtkSmartPointer<vtkActor> selectedActor;
    vtkSmartPointer<vtkTextActor> selectedTextActor;
    bool selectedModeOn;
    DataType dataType;
};

//class InteractorStyleTrackballActor: public vtkInteractorStyleTrackballActor
//{
//public:
//    static InteractorStyleTrackballActor* New();

//    InteractorStyleTrackballActor();
//    virtual void OnKeyPress();
//    void OnMouseMove();
//    void OnMiddleButtonUp();
//    void OnMiddleButtonDown();
//    void OnLeftButtonUp();
//    void OnLeftButtonDown();

//    vtkUnstructuredGrid* UnstructuredGrid;
//    vtkPolyData* GlyphData;

//    vtkSmartPointer<vtkPolyDataMapper> MoveMapper;
//    vtkSmartPointer<vtkActor> MoveActor;
//    vtkSmartPointer<vtkPolyData> MovePolyData;
//    vtkSmartPointer<vtkVertexGlyphFilter> MoveGlyphFilter;
//    vtkSmartPointer<vtkPointPicker> PointPicker;

//    bool Move;
//    vtkIdType SelectedPoint;
//    double pressDownPosition[3];
//    double pressUpPosition[3];
//};

//class InteractorStyleRubberBandPick: public vtkInteractorStyleRubberBandPick
//{
//public:
//    static InteractorStyleRubberBandPick* New();
//    InteractorStyleRubberBandPick();
//    virtual void OnKeyPress();
//    virtual void OnLeftButtonUp();
//    void SetPoints(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid);
//private:
//    vtkSmartPointer<vtkUnstructuredGrid> UnstructuredGrid;
//    vtkSmartPointer<vtkIdTypeArray> multi_ids;
//    bool MultiSelectMode;
//};
#endif // __MOUSEINTERACTORSTYLE_H__
