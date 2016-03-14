/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "XCMeshCustom.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"

// default pp steps
unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
                       aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
                       aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
                       aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
                       aiProcess_RemoveRedundantMaterials | // remove redundant materials
                       aiProcess_FindDegenerates | // remove degenerated polygons from the import
                       aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
                       aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
                       aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
                       aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
                       aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
                       aiProcess_OptimizeMeshes | // join small meshes, if possible;
                       aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
                       0;

void XCMeshCustom::Load(const void * buffer)
{
    Logger("Loading Custom mesh");

    const FBXCMesh* fbXCMesh = (const FBXCMesh*)buffer;
    
    m_userFriendlyName = fbXCMesh->MeshName()->c_str();
    m_resourcePath = getPlatformPath(fbXCMesh->MeshPath()->c_str());
    m_texture = (Texture2D*)m_resourceManager->GetResource(fbXCMesh->TextureRes()->c_str());
    m_initialScaling = fbXCMesh->InitialScaling();
    m_initialRotation = XCVec3Unaligned(fbXCMesh->InitialRotation()->x(), fbXCMesh->InitialRotation()->x(), fbXCMesh->InitialRotation()->x());

    const char* meshFilename = fbXCMesh->MeshPath()->c_str();

#if defined(USE_CUSTOM_XCMESH_JSON)
    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadMeshFile(XCMESHJSON_SCHEMA_FILEPATH);
    fbSystem.ParseAndLoadMeshFile(meshFilename);

    auto FBXCMeshRoot = GetFBMesh(fbSystem.GetBufferFromLoadedData());

    for (unsigned int meshIndex = 0; meshIndex < FBXCMeshRoot->meshes()->size(); ++meshIndex)
    {
        SubMesh* submesh = XCNEW(SubMesh);

        submesh->setObjectName(fbXCMesh->MeshName()->c_str());

        for (auto mesh = FBXCMeshRoot->meshes()->begin(); mesh != FBXCMeshRoot->meshes()->end(); )
        {
            submesh->setNoOfVertices(mesh->vertices()->size());
            //Read the vertices first
            for (auto vertex = mesh->vertices()->begin(); vertex != mesh->vertices()->end(); )
            {
                submesh->addVertex(vertex.operator->(), (++vertex).operator->(), (++vertex).operator->());
            }

            submesh->setNoOfFaces(mesh->faces()->size());
            //Read the face for indices
            for (auto face = mesh->faces()->begin(); face != mesh->faces()->end(); )
            {
                submesh->addFace(face.operator->(), (++face).operator->(), (++face).operator->(), 0);
            }

            //TODO : For every uv coord channel
            auto texCoord = mesh->texturecoords()->begin();
            for (auto uvIndex = texCoord->texcoords()->begin(); uvIndex != texCoord->texcoords()->begin(); )
            {
                submesh->addMapCoord(uvIndex.operator->(), (++uvIndex).operator->());
            }
        }

        m_subMeshes.push_back(submesh);
    }
#else
    //Assimp loader

    aiPropertyStore* props = aiCreatePropertyStore();
    aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
    aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 70.0f);
    aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

    aiSetImportPropertyInteger(props, AI_CONFIG_GLOB_MEASURE_TIME, 1);

    // Call ASSIMPs C-API to load the file
    m_scene = (aiScene*)aiImportFileExWithProperties(meshFilename,

        ppsteps | /* configurable pp steps */
        aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
        aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
        aiProcess_Triangulate | // triangulate polygons with more than 3 edges
        aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
        aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
        0,
        NULL,
        props);

    aiReleasePropertyStore(props);

#if 0
    // instruct aiProcess_FindDegenerates to drop degenerates 
    m_importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
    // instruct aiProcess_SortByPrimitiveType to drop line and point meshes
    m_importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

    // instruct aiProcess_GenSmoothNormals to not smooth normals with an angle of more than 70deg
    m_importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 70.0f);
    // instruct aiProcess_CalcTangents to not smooth normals with an angle of more than 70deg
    m_importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 70.0f);

    m_scene = m_importer.ReadFile(meshFilename, aiProcessPreset_TargetRealtime_MaxQuality
        | aiProcess_FlipWindingOrder 
        | aiProcess_MakeLeftHanded 
        | aiProcess_FlipUVs);

#endif

    XCASSERT(m_scene);
    
    aiMesh** mesh = m_scene->mMeshes;
    m_sceneAnimator = new SceneAnimator(m_scene, 0);

#if CUSTOM_ANIMATION
    //Copy all the nodes to our root node
    m_meshRootNode = new MeshNode(m_scene->mRootNode, nullptr, true);
#endif

    for (unsigned int meshIndex = 0; meshIndex < m_scene->mNumMeshes; ++meshIndex)
    {
        SubMesh* submesh = createAndGetSubMesh();
        {
            submesh->setObjectName(fbXCMesh->MeshName()->c_str());
            submesh->setNoOfVertices(mesh[meshIndex]->mNumVertices);
            submesh->setNoOfFaces(mesh[meshIndex]->mNumFaces);
            submesh->setNoOfBones(mesh[meshIndex]->mNumBones);

            SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
            submesh->m_boneBuffer = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbBoneBuffer)));

#if CUSTOM_ANIMATION
            // collect weights on all vertices. 
            std::vector<std::vector<aiVertexWeight> > weightsPerVertex(mesh[meshIndex]->mNumVertices);

            //Tranverse every bone and calculate the weight of every vertex in that bone
            for (unsigned int boneIndex = 0; boneIndex < mesh[meshIndex]->mNumBones; ++boneIndex)
            {
                const aiBone* bone = mesh[meshIndex]->mBones[boneIndex];

                for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
                {
                    weightsPerVertex[bone->mWeights[weightIndex].mVertexId].push_back(aiVertexWeight(boneIndex, bone->mWeights[weightIndex].mWeight));
                }

                submesh->m_boneNodes.push_back(bone);

                //Fill the bone hierarchy by finding the bone nodes.
                const MeshNode* node = m_meshRootNode->FindNode(bone->mName.C_Str());
                submesh->m_boneHierarchy.m_boneNodes[bone->mName.C_Str()] = new MeshNode(node, nullptr, false);
                
            }
#endif

            //fill the vertices & uv coords
            aiVector3D* vertex = mesh[meshIndex]->mVertices;
            aiVector3D** uvCoord = mesh[meshIndex]->mTextureCoords;
            
            float boneIndex[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            float boneWeight[] = { 0.0f, 0.0f, 0.0f, 0.0f };

            for (unsigned int vertexIndex = 0; vertexIndex < mesh[meshIndex]->mNumVertices; ++vertexIndex)
            {
                //Add vertex
                submesh->addVertex(vertex[vertexIndex].x, vertex[vertexIndex].y, vertex[vertexIndex].z);

                //Add UV
                if (mesh[meshIndex]->HasTextureCoords(0))
                {
                    submesh->addMapCoord(abs(uvCoord[0][vertexIndex].x), abs(uvCoord[0][vertexIndex].y));
                }

#if CUSTOM_ANIMATION
                //Add bone info
                if (mesh[meshIndex]->HasBones())
                {
                    XCASSERT(weightsPerVertex[vertexIndex].size() <= 4);

                    for (unsigned int weightIndex = 0; weightIndex < weightsPerVertex[vertexIndex].size(); ++weightIndex)
                    {
                        boneIndex[weightIndex]  = (float)weightsPerVertex[vertexIndex][weightIndex].mVertexId;
                        boneWeight[weightIndex] = (float)weightsPerVertex[vertexIndex][weightIndex].mWeight;
                    }

                    submesh->addBoneInfo(XCVec4Unaligned(boneIndex[0], boneIndex[1], boneIndex[2], boneIndex[3])
                                       , XCVec4Unaligned(boneWeight[0], boneWeight[1], boneWeight[2], boneWeight[3]));
                }
#endif
            }


            //fill the faces
            aiFace* face = mesh[meshIndex]->mFaces;
            unsigned int* indices = nullptr;
            for (unsigned int faceIndex = 0; faceIndex < mesh[meshIndex]->mNumFaces; ++faceIndex)
            {
                indices = face[faceIndex].mIndices;
                submesh->addFace(indices[0], indices[1], indices[2], 0);
            }
        }

#if CUSTOM_ANIMATION
        //Initialize the Mesh Animator;
        m_meshAnimator = new MeshAnimator(m_meshRootNode, m_scene);
#endif
    }
#endif

    //filterSubMeshes();
}

void XCMeshCustom::createBuffers(EVertexFormat formatType)
{
    XCMesh::createBuffers(VertexFormat_PositionNormalTextureBlendIndexBlendWeight);

    if (!m_areBuffersCreated)
    {

    }
}

void XCMeshCustom::DrawSubMeshes(RenderContext& renderContext, SHADERTYPE shaderType)
{
    XCMesh::DrawSubMeshes(renderContext, shaderType);
}