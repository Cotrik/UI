#include "MouseInteractorStyle.h"
#include "mainwindow.h"
#include <QString>

vtkStandardNewMacro(InteractorStyleTrackballCamera)
//vtkStandardNewMacro(InteractorStyleTrackballActor)
//vtkStandardNewMacro(InteractorStyleRubberBandPick)

//std::vector<vtkIdType> selectVertices;

//int set_handle_points(vtkUnstructuredGrid* UnstructuredGrid);
//bool update_vertices(vtkUnstructuredGrid* UnstructuredGrid);
//void GetPointNeighborPointIds(vtkIdType pointId, vtkUnstructuredGrid* dataSet, std::vector<vtkIdType>& neighbors);
//bool IsPointInPoints(vtkIdType pointId, const std::vector<vtkIdType>& Ids);
//bool IsCutPoint(vtkIdType pointId, const std::vector<vtkIdType>& Ids, vtkUnstructuredGrid* dataSet);

// Catch mouse events
InteractorStyleTrackballCamera::InteractorStyleTrackballCamera()
{
    selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    selectedActor = vtkSmartPointer<vtkActor>::New();
    selectedTextActor = vtkSmartPointer<vtkTextActor>::New();
    selectedModeOn = false;
}
void InteractorStyleTrackballCamera::OnKeyPress()
{
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    // Output the key that was pressed
    std::cout << "Pressed " << key << std::endl;

    // Handle an arrow key
    if (key == "Up")
    {
        std::cout << "The up arrow was pressed." << std::endl;
    }

    // Handle a "normal" key
    if (key == "a")
    {
        std::cout << "The a key was pressed." << std::endl;
    }
    if (key == "s")
    {
        std::cout << "The s key was pressed." << std::endl;
        selectedModeOn = !selectedModeOn;
        if (selectedModeOn){
            this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_CROSSHAIR);
            if (!g_mainWindow->IsSheetsEmpty())
                g_mainWindow->DeactivateAllSheets();
        }
        else
            this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_ARROW);
    }
    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}
void InteractorStyleTrackballCamera::OnLeftButtonDown()
{
    if (!selectedModeOn)
        return vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    // Get the location of the click (in window coordinates)
    int* pos = this->GetInteractor()->GetEventPosition();

    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);

    // Pick from this location.
    picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

    double* worldPosition = picker->GetPickPosition();
    std::cout << "Cell id: " << picker->GetCellId() << std::endl;

    QString strId = QString::number(picker->GetCellId());
    selectedTextActor->SetInput(strId.toStdString().c_str());
    selectedTextActor->SetPosition(pos[0], pos[1]);
    selectedTextActor->GetTextProperty()->SetFontSize ( 24 );
    selectedTextActor->GetTextProperty()->SetColor (0.0, 1.0, 0.0 );

    if (picker->GetCellId() != -1)
    {
        //std::cout << "Pick position is: " << worldPosition[0] << " " << worldPosition[1] << " " << worldPosition[2] << endl;

        vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
        ids->SetNumberOfComponents(1);
        ids->InsertNextValue(picker->GetCellId());

        vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
        selectionNode->SetFieldType(vtkSelectionNode::CELL);
        selectionNode->SetContentType(vtkSelectionNode::INDICES);
        selectionNode->SetSelectionList(ids);

        vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
        selection->AddNode(selectionNode);

        vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
#if VTK_MAJOR_VERSION<= 5
        extractSelection->SetInput(0, this->polyData);
        extractSelection->SetInput(1, selection);
#else
        if (this->dataType == UNSTRUCTED_GRID)
            extractSelection->SetInputData(0, this->vtkUnstructuredGridData);
        else if (this->dataType == POLYDATA)
            extractSelection->SetInputData(0, this->polyData);
        extractSelection->SetInputData(1, selection);
#endif
        extractSelection->Update();

        vtkSmartPointer<vtkUnstructuredGrid> selected = vtkSmartPointer<vtkUnstructuredGrid>::New();
        selected->ShallowCopy(extractSelection->GetOutput());

        std::cout << "There are " << selected->GetNumberOfPoints() << " points "
                  << selected->GetNumberOfCells() << " cells in the selection." << std::endl;
        std::vector<int> pIds;
        if (this->dataType == POLYDATA){
            std::cout << "Point Ids: ";
            vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();
            this->polyData.GetPointer()->GetCellPoints(picker.GetPointer()->GetCellId(), list);
            for (vtkIdType i = 0; i < list->GetNumberOfIds(); i++)
            {
                std::cout << list->GetId(i) << " ";
                pIds.push_back(list->GetId(i));
            }
            std::sort(pIds.begin(), pIds.end());
            std::cout << endl;
        }
        //////////////////////////////////////////////////
        //////////////////////////////////////////////////
        /// neighboring sheets
        if (this->dataType == POLYDATA && !pIds.empty())
        {
//            std::vector<int> sheetIndices;
//            for (int sheet_index = 0; sheet_index < all_com_sheets.size(); sheet_index++)
//            {
//                const Component_Sheet& cs = all_com_sheets.at(sheet_index);
//                std::vector<int> allnodes;
//                for (int i = 0; i < cs.components.size(); i++)
//                {
//                    const int components_index = cs.components.at(i);
//                    for (int j = 0; j < FrameI.FHs[components_index].hs_net.size(); j++)
//                    {
//                        const int cell_index = FrameI.FHs[components_index].hs_net.at(j);
//                        const Hex& cell = hex_mesh.HHs.at(cell_index);
//                        for (int k = 0; k < 8; k++)
//                        allnodes.push_back(cell.V_Ids[k]);
//                    }
//                }
//                std::sort(allnodes.begin(), allnodes.end());
//                std::vector<int>::iterator iter = std::unique(allnodes.begin(), allnodes.end());
//                allnodes.resize(std::distance(allnodes.begin(), iter));
//                std::vector<int> v_intersection;
//                std::set_intersection(pIds.begin(), pIds.end(), allnodes.begin(), allnodes.end(), std::back_inserter(v_intersection));
//                if (pIds == v_intersection)
//                {
//                    std::cout << "sheet_" << sheet_index << std::endl;
//                    sheetIndices.push_back(sheet_index);
//                }
//                std::cout << endl;
//            }
//            g_mainWindow->DisplaySheets(sheetIndices);
//            this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_ARROW);
//            selectedModeOn = !selectedModeOn;
        }
//#if VTK_MAJOR_VERSION<= 5
//        selectedMapper->SetInputConnection(selected->GetProducerPort());
//#else
        selectedMapper->SetInputData(selected);
//#endif

        selectedActor->SetMapper(selectedMapper);
        selectedActor->GetProperty()->EdgeVisibilityOn();
        selectedActor->GetProperty()->SetEdgeColor(1, 0, 1);
        if (this->dataType == UNSTRUCTED_GRID)
            selectedActor->GetProperty()->SetLineWidth(5);
        else if (this->dataType == POLYDATA)
            selectedActor->GetProperty()->SetLineWidth(8);

        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(selectedActor);
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(selectedTextActor);

        this->Interactor->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
        selectedModeOn = false;
        this->Interactor->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_ARROW);
    }
    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void InteractorStyleTrackballCamera::SetDataType(const DataType dataType)
{
    this->dataType = dataType;
}

void InteractorStyleTrackballCamera::SetData(vtkSmartPointer<vtkUnstructuredGrid> data)
{
    this->vtkUnstructuredGridData = data;
    SetDataType(UNSTRUCTED_GRID);
    cout << "vtkUnstructuredGridData" << std::endl;
}

void InteractorStyleTrackballCamera::SetData(vtkSmartPointer<vtkPolyData> data)
{
    this->polyData = data;
    SetDataType(POLYDATA);
    cout << "POLYDATA" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////////

//InteractorStyleTrackballActor::InteractorStyleTrackballActor()
//{
//    this->Move = false;
//    this->PointPicker = vtkSmartPointer<vtkPointPicker>::New();

//    // Setup ghost glyph
//    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//    points->InsertNextPoint(0, 0, 0);
//    this->MovePolyData = vtkSmartPointer<vtkPolyData>::New();
//    this->MovePolyData->SetPoints(points);
//    this->MoveGlyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
//#if VTK_MAJOR_VERSION <= 5
//    this->MoveGlyphFilter->SetInputConnection(
//            this->MovePolyData->GetProducerPort());
//#else
//    this->MoveGlyphFilter->SetInputData(this->MovePolyData);
//#endif
//    this->MoveGlyphFilter->Update();

//    this->MoveMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//    this->MoveMapper->SetInputConnection(this->MoveGlyphFilter->GetOutputPort());

//    this->MoveActor = vtkSmartPointer<vtkActor>::New();
//    this->MoveActor->SetMapper(this->MoveMapper);
//    this->MoveActor->VisibilityOff();
//    this->MoveActor->GetProperty()->SetPointSize(10);
//    this->MoveActor->GetProperty()->SetColor(1, 0, 0);
//}
//void InteractorStyleTrackballActor::OnKeyPress()
//{
//    // Get the keypress
//    vtkRenderWindowInteractor *rwi = this->Interactor;
//    std::string key = rwi->GetKeySym();

//    // Output the key that was pressed
//    std::cout << "Pressed " << key << std::endl;

//    // Handle an arrow key
//    if (key == "Up")
//    {
//        std::cout << "The up arrow was pressed." << std::endl;
//    }

//    if (key == "s")
//    {
//        std::cout << "The s key was pressed." << std::endl;
//        switchInteractorStyle(Interactor_Style_TrackballActor);
//    }

//    // Forward events
//    vtkInteractorStyleTrackballActor::OnKeyPress();
//}
//void InteractorStyleTrackballActor::OnMouseMove()
//{
//    if (!this->Move)
//    {
//        return;
//    }

//    vtkInteractorStyleTrackballActor::OnMouseMove();
//}

//void InteractorStyleTrackballActor::OnMiddleButtonUp()
//{
//    this->EndPan();

//    this->Move = false;
//    this->MoveActor->VisibilityOff();

//    this->UnstructuredGrid->GetPoints()->SetPoint(this->SelectedPoint, this->MoveActor->GetPosition());
//    this->UnstructuredGrid->Modified();
//    this->GetCurrentRenderer()->Render();
//    this->GetCurrentRenderer()->GetRenderWindow()->Render();

//}
//void InteractorStyleTrackballActor::OnMiddleButtonDown()
//{
//    // Get the selected point
//    int x = this->Interactor->GetEventPosition()[0];
//    int y = this->Interactor->GetEventPosition()[1];
//    this->FindPokedRenderer(x, y);

//    this->PointPicker->Pick(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1], 0,  // always zero.
//            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

//    if (this->PointPicker->GetPointId() >= 0)
//    {
//        this->StartPan();
//        this->MoveActor->VisibilityOn();
//        this->Move = true;
//        this->SelectedPoint = this->PointPicker->GetPointId();

//        std::cout << "Dragging point " << this->SelectedPoint << std::endl;

//        double p[3];
//        this->UnstructuredGrid->GetPoint(this->SelectedPoint, p);
//        std::cout << "p: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
//        this->MoveActor->SetPosition(p);

//        this->GetCurrentRenderer()->AddActor(this->MoveActor);
//        this->InteractionProp = this->MoveActor;
//    }
//}
//void InteractorStyleTrackballActor::OnLeftButtonUp()
//{
//    this->EndPan();

//    this->Move = false;
//    this->MoveActor->VisibilityOff();

//    //this->UnstructuredGrid->GetPoint(this->SelectedPoint, this->pressUpPosition);
//    double* p = this->MoveActor->GetPosition();
//    this->pressUpPosition[0] = p[0];
//    this->pressUpPosition[1] = p[1];
//    this->pressUpPosition[2] = p[2];
//    double dir[3];
//    dir[0] = this->pressUpPosition[0] - this->pressDownPosition[0];
//    dir[1] = this->pressUpPosition[1] - this->pressDownPosition[1];
//    dir[2] = this->pressUpPosition[2] - this->pressDownPosition[2];
//    if (this->Interactor->GetControlKey() == 0)
//    {
//        this->UnstructuredGrid->GetPoints()->SetPoint(this->SelectedPoint, this->MoveActor->GetPosition());
//        this->UnstructuredGrid->Modified();
//    }
//    else
//    {

//        std::sort(selectVertices.begin(), selectVertices.end());
//        std::vector<vtkIdType>::iterator iter = std::unique(selectVertices.begin(), selectVertices.end());
//        selectVertices.resize(std::distance(selectVertices.begin(), iter));
//        //////////////////
//        //test cut point
//        for (int j = 0; j < selectVertices.size(); j++)
//        {
//            if (IsCutPoint(selectVertices.at(j), selectVertices, this->UnstructuredGrid))
//                std::cout << "cut : " << selectVertices.at(j) << std::endl;;
//        }
//        ///////////////////
//        set_handle_points(UnstructuredGrid);
//        vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(this->UnstructuredGrid->GetPointData()->GetArray("OriginalIds"));
//        for (int j = 0; j < selectVertices.size(); j++)
//        {
//            vtkIdType id = selectVertices.at(j);
//            bool isSelectedVertex = false;
//            vtkIdType i;
//            int n = ids->GetNumberOfTuples();
//            for (i = 0; i < n; i++)
//            {
//                //std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
//                vtkIdType meshid = ids->GetValue(i);
//                if (meshid == id)
//                {
//                    isSelectedVertex = true;
//                    break;
//                }
//            }
//            if (isSelectedVertex)
//            {
//                double op[3];
//                this->UnstructuredGrid->GetPoint(i, op);
//                op[0] += dir[0];
//                op[1] += dir[1];
//                op[2] += dir[2];
//                this->UnstructuredGrid->GetPoints()->SetPoint(i, op);
//            }
//        }
//        update_vertices(UnstructuredGrid);
//        this->UnstructuredGrid->Modified();
//    }
//    this->GetCurrentRenderer()->Render();
//    this->GetCurrentRenderer()->GetRenderWindow()->Render();

//}
//void InteractorStyleTrackballActor::OnLeftButtonDown()
//{
//    // Get the selected point
//    int x = this->Interactor->GetEventPosition()[0];
//    int y = this->Interactor->GetEventPosition()[1];
//    this->FindPokedRenderer(x, y);

//    this->PointPicker->Pick(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1], 0,  // always zero.
//            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

//    if (this->PointPicker->GetPointId() >= 0)
//    {
//        this->StartPan();
//        this->MoveActor->VisibilityOn();
//        this->Move = true;
//        this->SelectedPoint = this->PointPicker->GetPointId();

//        std::cout << "Dragging point " << this->SelectedPoint << std::endl;

//        double p[3];
//        this->UnstructuredGrid->GetPoint(this->SelectedPoint, p);
//        this->UnstructuredGrid->GetPoint(this->SelectedPoint, this->pressDownPosition);
//        std::cout << "p: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
//        this->MoveActor->SetPosition(p);

//        this->GetCurrentRenderer()->AddActor(this->MoveActor);
//        this->InteractionProp = this->MoveActor;
//    }
//}

/////////////////////////////////////////////////////////////////////////////////////////
////
/////////////////////////////////////////////////////////////////////////////////////////
//InteractorStyleRubberBandPick::InteractorStyleRubberBandPick()
//{
//    this->MultiSelectMode = false;
//    this->multi_ids = vtkSmartPointer<vtkIdTypeArray>::New();
//}

//void InteractorStyleRubberBandPick::OnKeyPress()
//{
//    // Get the keypress
//    vtkRenderWindowInteractor *rwi = this->Interactor;
//    std::string key = rwi->GetKeySym();

//    // Output the key that was pressed
//    std::cout << "Pressed " << key << std::endl;

//    // Handle an arrow key
//    if (key == "Up")
//    {
//        std::cout << "The up arrow was pressed." << std::endl;
//    }

//    // Handle a "normal" key
//    if (key == "s")
//    {
//        std::cout << "The a key was pressed." << std::endl;
//        switchInteractorStyle(Interactor_Style_RubberBandPick);
//    }
//    // Handle a "normal" key
//    if (key == "Control_L")
//    {
//        std::cout << "The Ctrl key was pressed." << std::endl;
//    }

//    // Forward events
//    vtkInteractorStyleTrackballCamera::OnKeyPress();
//}

//void InteractorStyleRubberBandPick::OnLeftButtonUp()
//{
//    // Forward events
//    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

//    vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();

//    vtkSmartPointer<vtkExtractGeometry> extractGeometry = vtkSmartPointer<vtkExtractGeometry>::New();
//    extractGeometry->SetImplicitFunction(frustum);
//#if VTK_MAJOR_VERSION <= 5
//    extractGeometry->SetInput(this->UnstructuredGrid);
//#else
//    extractGeometry->SetInputData(this->UnstructuredGrid);
//#endif
//    extractGeometry->Update();

//    vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
//    glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
//    glyphFilter->Update();

//    vtkPolyData* selected = glyphFilter->GetOutput();
//    std::cout << "Selected " << selected->GetNumberOfPoints() << " points." << std::endl;
//    std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;
//    vtkSmartPointer<vtkDataSetMapper> SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
//#if VTK_MAJOR_VERSION <= 5
//    this->SelectedMapper->SetInput(selected);
//#else
//    this->SelectedMapper->SetInputData(selected);
//#endif
//    this->SelectedMapper->ScalarVisibilityOff();

//    std::cout << this->GetInteractor()->GetControlKey();
//    if (this->GetInteractor()->GetControlKey() == 0)
//    {
//        vtkPropCollection * ps = this->CurrentRenderer->GetViewProps();
//        //this->CurrentRenderer->RemoveAllViewProps();
//        ps->InitTraversal();
//        vtkProp* p = ps->GetNextProp();
//        p = ps->GetNextProp();
//        while (p != NULL)
//        {
//            ps->RemoveItem(p);
//            p = ps->GetNextProp();
//        }
//        selectVertices.clear();
//    }
//    //else
//    {
//        vtkIdTypeArray* ids = vtkIdTypeArray::SafeDownCast(selected->GetPointData()->GetArray("OriginalIds"));
//        for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
//        {
//            std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
//            this->multi_ids->InsertNextTuple(ids->GetTuple(i));
//            selectVertices.push_back(ids->GetValue(i));
//        }
//    }
//    vtkSmartPointer<vtkActor> SelectedActor = vtkSmartPointer<vtkActor>::New();
//    SelectedActor->SetMapper(SelectedMapper);
//    SelectedActor->GetProperty()->SetColor(1.0, 0.0, 0.0); //(R,G,B)
//    SelectedActor->GetProperty()->SetPointSize(5);

//    this->CurrentRenderer->AddActor(SelectedActor);
//    this->GetInteractor()->GetRenderWindow()->Render();
//    this->HighlightProp(NULL);
//}

//void InteractorStyleRubberBandPick::SetPoints(vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid)
//{
//    this->UnstructuredGrid = unstructuredGrid;
//}

/////////////////////////////////////////
//vtkSmartPointer<vtkIdList> GetConnectedVertices(vtkSmartPointer<vtkPolyData> mesh, int id);
//void GetPointNeighborPointIds(vtkIdType pointId, vtkUnstructuredGrid* dataSet, std::vector<vtkIdType>& neighbors)
//{
//    vtkSmartPointer<vtkIdList> PointCellIds = vtkSmartPointer<vtkIdList>::New();
//    dataSet->GetPointCells(pointId, PointCellIds);
//    for (vtkIdType i = 0; i < PointCellIds->GetNumberOfIds(); i++)
//    {
//        vtkIdType cellId = PointCellIds->GetId(i);
//        vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
//        dataSet->GetCellPoints(cellId, cellPointIds);
//        for (vtkIdType j = 0; j < cellPointIds->GetNumberOfIds(); j++)
//        {
//            vtkIdType pId = cellPointIds->GetId(j);
//            if (pId != pointId)
//                neighbors.push_back(pId);
//        }
//    }
//}

//vtkSmartPointer<vtkIdList> GetConnectedVertices(vtkSmartPointer<vtkPolyData> mesh, int id)
//{
//    vtkSmartPointer<vtkIdList> connectedVertices = vtkSmartPointer<vtkIdList>::New();

//    //get all cells that vertex 'id' is a part of
//    vtkSmartPointer<vtkIdList> cellIdList = vtkSmartPointer<vtkIdList>::New();
//    mesh->GetPointCells(id, cellIdList);

//    /*
//     cout << "Vertex 0 is used in cells ";
//     for(vtkIdType i = 0; i < cellIdList->GetNumberOfIds(); i++)
//     {
//     cout << cellIdList->GetId(i) << ", ";
//     }
//     cout << endl;
//     */

//    for (vtkIdType i = 0; i < cellIdList->GetNumberOfIds(); i++)
//    {
//        //cout << "id " << i << " : " << cellIdList->GetId(i) << endl;

//        vtkSmartPointer<vtkIdList> pointIdList = vtkSmartPointer<vtkIdList>::New();
//        mesh->GetCellPoints(cellIdList->GetId(i), pointIdList);

//        //cout << "End points are " << pointIdList->GetId(0) << " and " << pointIdList->GetId(1) << endl;

//        if (pointIdList->GetId(0) != id)
//        {
//            //cout << "Connected to " << pointIdList->GetId(0) << endl;
//            connectedVertices->InsertNextId(pointIdList->GetId(0));
//        }
//        else
//        {
//            //cout << "Connected to " << pointIdList->GetId(1) << endl;
//            connectedVertices->InsertNextId(pointIdList->GetId(1));
//        }
//    }

//    return connectedVertices;
//}
//bool IsPointInPoints(vtkIdType pointId, const std::vector<vtkIdType>& Ids)
//{
//    return std::find(Ids.begin(), Ids.end(), pointId) != Ids.end();
//}

//bool IsCutPoint(vtkIdType pointId, const std::vector<vtkIdType>& Ids, vtkUnstructuredGrid* dataSet)
//{
//    std::vector<vtkIdType> neighbors;
//    std::sort(neighbors.begin(), neighbors.end());
//    std::vector<vtkIdType>::iterator iter = std::unique(neighbors.begin(), neighbors.end());
//    neighbors.resize(std::distance(neighbors.begin(), iter));
//    GetPointNeighborPointIds(pointId, dataSet, neighbors);
//    for (size_t i = 0; i < neighbors.size(); i++)
//    {
//        vtkIdType id = neighbors.at(i);
//        if (!IsPointInPoints(id, Ids))
//        {
//            return true;
//        }
//    }
//    return false;
//}
