/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/BasicGeometry/CubeMesh.h"

#if !defined(XC_ORBIS)
#include <DirectXCollision.h>
#endif

class OrientedBoundingBox : public CubeMesh
{
public:
    static const int MAX_OBB_CORNER_POINTS_COUNT = 8;

    OrientedBoundingBox();
    virtual ~OrientedBoundingBox();

    virtual void               Init();
    virtual void               Load();
    virtual void               Update(float dt);
    virtual void               Draw(RenderContext& context);

    //Oriented Bounding Box
    BoundingOrientedBox        m_bBox;
    BoundingOrientedBox        m_TransformedBox;

    XCVecIntrinsic4            m_boxCenter;
    XCVecIntrinsic4            m_boxExtends;

    void                       CreateBoundBox();
    void                       CreateBoundBox(XCVecIntrinsic4 min, XCVecIntrinsic4 max);
    void                       CreateBoundBox(OrientedBoundingBox* const boundBox);
    void                       Transform(XCMatrix4 translateMat, XCMatrix4 rotateMatrix);
};