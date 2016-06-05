/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XC_Mesh/XCMesh.h"

class XCMeshAssimp : public XCMesh
{
public:
    DECLARE_OBJECT_CREATION(XCMeshAssimp)

    XCMeshAssimp()
    {}

protected:
    virtual bool            LoadMesh();
};