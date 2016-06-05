/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define NOMINMAX

 //std, stl
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <queue>

 //Thread
#include <future>
#include <atomic>
#include <mutex>

#include "Base/Serializer/IBase.h"
#include "Base/BaseTypes.h"
#include "Base/XCMath/XCMathIncludes.h"
#include "Base/ErrorHandling/ErrorHandler.h"

#if defined(WIN_32)
#include "ApplicationFramework/Win32/pch_win32.h"
#elif defined(DURANGO)
#include "ApplicationFramework/Durango/pch_durango.h"
#elif defined(XC_ORBIS)
#include "ApplicationFramework/Orbis/pch_orbis.h"
#endif

#include "Engine/System/SystemLocator.h"

//Define Graphics API to use Dx/Opengl
#define XCGRAPHICS_DX12

#if defined(XCGRAPHICS_DX12)
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3d11_2.h>
#include <pix.h>
#include <D3Dcompiler.h>
#elif defined(XCGRAPHICS_DX11)
#include <d3d11.h>
#include <D3Dcompiler.h>
#elif defined(XCGRAPHICS_GL)
#include <GL/glew.h>
#include <GL/glut.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

#include "Graphics/GraphicsSpecificDefines.h"