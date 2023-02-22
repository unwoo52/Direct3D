#include "SkinedMeshRenderer.h"
#include "../../EngineUtils.h"
#include "../../Time.h"
#include "../Object/GameObject.h"

bool SkinedMeshRenderer::LoadFile(const string& filePath)
{
    directory = EngineUtils::GetDirectoryFromPath(filePath);
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
    if (pScene == nullptr) return false;
    ProcessNode(pScene->mRootNode, pScene, XMMatrixIdentity(), 0, 0);

    vector<AnimationClipData> anims;
    LoadAnimation(pScene, anims);
    MakeAnimationClip(anims);

    try
    {
        HRESULT hr = boneData.Initialize(device, dc);
        COM_ERROR_IF_FAILED(hr, L"본 정보 상수 버퍼 생성에 실패 하였습니다.");
    }
    catch (ComException& ex)
    {
        Debug::Log(ex.what());
        return false;
    }

    if (animClips.size() > 0)
    {
        boneData.data.hasAnimation = 1;
    }

    return true;
}

void SkinedMeshRenderer::Draw(const XMMATRIX vpMat)
{
    ProcessKeyFrameAnimation();
    UINT offset = 0;
    dc->VSSetConstantBuffers(0, 1, cb_transform->GetAddressOf());
    dc->PSSetShaderResources(0, 1, &texture);

    for (Mesh<SkinedVertex> mesh : meshes)
    {
        cb_transform->data.world = gameObject->GetTransform()->GetWorld();
        cb_transform->data.vp = vpMat;
        cb_transform->Update();
        mesh.Draw();
    }
}

void SkinedMeshRenderer::Update()
{
    animStateInfo.weight += Time::GetDeltaTime() * animClips[animStateInfo.idx].tickPerSec;
    if (animStateInfo.weight >= 1.0f)
    {
        animStateInfo.weight = 0.0f;
        animStateInfo.currentFrame++;
        animStateInfo.nextFrame++;
        if (animStateInfo.nextFrame == animClips[animStateInfo.idx].duration)
        {
            animStateInfo.currentFrame = 0;
            animStateInfo.nextFrame = 1;
        }
    }
}

void SkinedMeshRenderer::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parent, UINT idx, UINT parentIdx)
{
    BoneNode _node;
    _node.name = node->mName.C_Str();
    _node.idx = idx;
    _node.parentIdx = parentIdx;
    _node.offset = XMMatrixIdentity();

    nodes.push_back(_node);

    XMMATRIX local = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parent;
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(this->ProcessMesh(mesh, scene, local));
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene, local, nodes.size(), idx);
    }
}
Mesh<SkinedVertex> SkinedMeshRenderer::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& mat)
{
    vector<SkinedVertexWeights> weights(mesh->mNumVertices);
    ProcessBone(mesh, weights);

    vector<SkinedVertex> vertices;
    vector<DWORD> indices;

    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        SkinedVertex v;
        v.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x,mesh->mNormals[i].y ,mesh->mNormals[i].z };
        if (mesh->mTextureCoords[0])
        {
            v.uv.x = (float)mesh->mTextureCoords[0][i].x;
            v.uv.y = (float)mesh->mTextureCoords[0][i].y;
        }

        weights[i].Normalize();
        v.boneIDs.x = static_cast<float>(weights[i].boneIDs[0]);
        v.boneIDs.y = static_cast<float>(weights[i].boneIDs[1]);
        v.boneIDs.z = static_cast<float>(weights[i].boneIDs[2]);
        v.boneIDs.w = static_cast<float>(weights[i].boneIDs[3]);

        v.weights.x = weights[i].weights[0];
        v.weights.y = weights[i].weights[1];
        v.weights.z = weights[i].weights[2];
        v.weights.w = weights[i].weights[3];

        vertices.push_back(v);
    }

    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (UINT n = 0; n < face.mNumIndices; ++n)
        {
            indices.push_back(face.mIndices[n]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> textures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);

    return Mesh<SkinedVertex>(device, dc, vertices, indices, textures, mat);
}

TextureStorageType SkinedMeshRenderer::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int idx, aiTextureType type)
{
    if (pMat->GetTextureCount(type) == 0) return TextureStorageType::None;

    aiString path;
    pMat->GetTexture(type, idx, &path);
    string texturePath = path.C_Str();
    if (texturePath[0] == '*')
    {
        if (pScene->mTextures[0]->mHeight == 0)
        {
            return TextureStorageType::EmbeddedIndexCompressed;
        }
        else
        {
            assert("Support does not exist yet for indexed no compressed textures!" && 0);
            return TextureStorageType::EmbeddedIndexNonCompressed;
        }
    }
    if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
    {
        if (pTex->mHeight == 0)
        {
            return TextureStorageType::EmbeddedCompressed;
        }
        else
        {
            assert("Support does not exist yet for no compressed textures!" && 0);
            return TextureStorageType::EmbeddedNonCompressed;
        }
    }
    if (texturePath.find('.') != string::npos)
    {
        return TextureStorageType::Disk;
    }

    return TextureStorageType::Invalid;
}

vector<Texture> SkinedMeshRenderer::LoadMaterialTextures(aiMaterial* pMat, aiTextureType type, const aiScene* pScene)
{
    vector<Texture> matTextures;
    TextureStorageType storageType = TextureStorageType::Invalid;
    unsigned int texCount = pMat->GetTextureCount(type);
    if (texCount == 0)
    {
        storageType = TextureStorageType::None;
        aiColor3D aiColor = { 0, 0, 0 };
        switch (type)
        {
        case aiTextureType_DIFFUSE:
            pMat->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
            if (aiColor.IsBlack())
            {
                matTextures.push_back(Texture(device, COLORS::UnloadedTextureColor, type));
                return matTextures;
            }
            matTextures.push_back(Texture(device, ColorClass(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), type));
            return matTextures;
        }
    }
    else
    {
        for (UINT i = 0; i < texCount; ++i)
        {
            aiString path;
            pMat->GetTexture(type, i, &path);
            TextureStorageType storageType = DetermineTextureStorageType(pScene, pMat, i, type);
            switch (storageType)
            {
            case TextureStorageType::Disk:
            {
                string filename = directory + '/' + path.C_Str();
                matTextures.push_back(Texture(device, filename, type));
                break;
            }
            case TextureStorageType::EmbeddedCompressed:
            {
                const aiTexture* pTex = pScene->GetEmbeddedTexture(path.C_Str());
                Texture embeddedTex(device, reinterpret_cast<uint8_t*>(pTex->pcData), pTex->mWidth, type);
                matTextures.push_back(embeddedTex);
                //matTextures.push_back(Texture(device,COLORS::UnloadedTextureColor,type));
                break;
            }
            case TextureStorageType::EmbeddedIndexCompressed:
            {
                int idx = GetTextureIndex(&path);
                Texture embeddedIdxTex(device, reinterpret_cast<uint8_t*>(pScene->mTextures[idx]->pcData),
                    pScene->mTextures[idx]->mWidth, type);
                matTextures.push_back(embeddedIdxTex);
                break;
            }
            }
        }
    }

    if (matTextures.size() == 0)
    {
        matTextures.push_back(Texture(device, COLORS::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
    }

    return matTextures;
}

int SkinedMeshRenderer::GetTextureIndex(aiString* pStr)
{
    assert(pStr->length >= 2);
    return atoi(&pStr->C_Str()[1]);
}

void SkinedMeshRenderer::ProcessBone(aiMesh* mesh, vector<SkinedVertexWeights>& weights)
{
    for (UINT i = 0; i < mesh->mNumBones; ++i)
    {
        UINT boneIdx = 0;
        string name = mesh->mBones[i]->mName.C_Str();
        if (boneMap.find(name) == boneMap.end())
        {
            boneIdx = boneMap[name] = boneMap.size();
            
            BoneNode node;
            node.name = name;
            node.offset = XMMatrixTranspose(XMMATRIX(&mesh->mBones[i]->mOffsetMatrix.a1));
            bones.push_back(node);            
        }
        else
        {
            boneIdx = boneMap[name];
        }

        for (UINT n = 0; n < mesh->mBones[i]->mNumWeights; ++n)
        {
            UINT vertexID = mesh->mBones[i]->mWeights[n].mVertexId;
            float w = mesh->mBones[i]->mWeights[n].mWeight;
            weights[vertexID].Add(boneIdx, w);
        }
    }
}

void SkinedMeshRenderer::LoadAnimation(const aiScene* pScene, vector<AnimationClipData>& animDatas)
{
    for (UINT i = 0; i < pScene->mNumAnimations; ++i)
    {
        aiAnimation* srcAnim = pScene->mAnimations[i];
        AnimationClipData info;
        info.name = srcAnim->mName.C_Str();
        info.duration = static_cast<float>(srcAnim->mDuration);
        info.tickPerSec = static_cast<float>(srcAnim->mTicksPerSecond);
        for (UINT n = 0; n < srcAnim->mNumChannels; ++n)
        {
            aiNodeAnim* srcNodeAnim = srcAnim->mChannels[n];
            AnimationNodeData keyFrames;
            keyFrames.name = srcNodeAnim->mNodeName.C_Str();
            for (UINT m = 0; m < srcNodeAnim->mNumScalingKeys; ++m)
            {
                KeyScale scale;
                scale.scale.x = srcNodeAnim->mScalingKeys[m].mValue.x;
                scale.scale.y = srcNodeAnim->mScalingKeys[m].mValue.y;
                scale.scale.z = srcNodeAnim->mScalingKeys[m].mValue.z;
                scale.time = static_cast<float>(srcNodeAnim->mScalingKeys[m].mTime);
                keyFrames.scales.push_back(scale);
            }            

            for (UINT m = 0; m < srcNodeAnim->mNumRotationKeys; ++m)
            {
                KeyRotation rot;
                rot.rot.x = srcNodeAnim->mRotationKeys[m].mValue.x;
                rot.rot.y = srcNodeAnim->mRotationKeys[m].mValue.y;
                rot.rot.z = srcNodeAnim->mRotationKeys[m].mValue.z;
                rot.rot.w = srcNodeAnim->mRotationKeys[m].mValue.w;
                rot.time = static_cast<float>(srcNodeAnim->mRotationKeys[m].mTime);
                keyFrames.rotations.push_back(rot);
            }

            for (UINT m = 0; m < srcNodeAnim->mNumPositionKeys; ++m)
            {
                KeyPosition pos;
                pos.pos.x = srcNodeAnim->mPositionKeys[m].mValue.x;
                pos.pos.y = srcNodeAnim->mPositionKeys[m].mValue.y;
                pos.pos.z = srcNodeAnim->mPositionKeys[m].mValue.z;
                pos.time = static_cast<float>(srcNodeAnim->mPositionKeys[m].mTime);
                keyFrames.positions.push_back(pos);
            }
            info.keyFrames.emplace(keyFrames.name, keyFrames);
        }
        animDatas.push_back(info);
    }
}

void SkinedMeshRenderer::MakeAnimationClip(vector<AnimationClipData>& animDatas)
{
    UINT size = animClips.size();
    KeyFramesIter iter;
    unordered_map<string, UINT>::iterator iter2;

    for (UINT i = 0; i < animDatas.size(); ++i)
    {
        UINT idx = size + i;
        animClips.emplace_back(AnimationClip());
        animClips[idx].duration = animDatas[i].duration + 1.0f;
        animClips[idx].name = animDatas[i].name;
        animClips[idx].tickPerSec = animDatas[i].tickPerSec;
        UINT durationCount = static_cast<UINT>(animClips[idx].duration);
        animClips[idx].keyFrames = make_unique<unique_ptr<AnimationTransform[]>[]>(durationCount);        
        for (UINT n = 0; n < durationCount; ++n)
        {
            animClips[idx].keyFrames[n] = make_unique<AnimationTransform[]>(nodes.size());
            for (UINT m = 0; m < nodes.size(); ++m)
            {
                XMFLOAT4 rot;
                XMFLOAT3 scale, pos;

                iter = animDatas[i].keyFrames.find(nodes[m].name);
                if (iter != animDatas[i].keyFrames.end())
                {
                    pos = CalcInterpolatedPosition(iter, (float)n, animClips[idx].duration);
                    rot = CalcInterpolatedRotation(iter, (float)n, animClips[idx].duration);
                    scale = CalcInterpolatedScale(iter, (float)n, animClips[idx].duration);
                }
                else
                {
                    // 11 12 13 14
                    // 21 22 23 24
                    // 31 32 33 34
                    // 41 42 43 44 position
                    XMFLOAT4X4 mat;
                    XMStoreFloat4x4(&mat, nodes[m].offset);
                    pos.x = mat._41;
                    pos.y = mat._42;
                    pos.z = mat._43;

                    XMStoreFloat4(&rot, XMQuaternionRotationMatrix(nodes[m].offset));
                    
                    XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&rot));
                    XMMATRIX S = nodes[m].offset * XMMatrixInverse(nullptr, R);
                    XMStoreFloat4x4(&mat, S);
                    scale.x = mat._11;
                    scale.y = mat._22;
                    scale.z = mat._33;
                }

                animClips[idx].keyFrames[n][m].pos = pos;
                animClips[idx].keyFrames[n][m].rot = rot;
                animClips[idx].keyFrames[n][m].scale = scale;

                iter2 = boneMap.find(nodes[m].name);
                if (iter2 != boneMap.end())
                {
                    int boneIdx = iter2->second;
                    bones[boneIdx].idx = m;
                    bones[boneIdx].parentIdx = nodes[m].parentIdx;
                }
            }
        }
    }
}

void SkinedMeshRenderer::ProcessKeyFrameAnimation()
{
    Update();
    vector<XMMATRIX> nodeMatrixes(nodes.size());
    for (UINT i = 0; i < nodes.size(); ++i)
    {
        XMVECTOR p1 = XMLoadFloat3(&animClips[animStateInfo.idx].keyFrames[animStateInfo.currentFrame][i].pos);
        XMVECTOR p2 = XMLoadFloat3(&animClips[animStateInfo.idx].keyFrames[animStateInfo.nextFrame][i].pos);
        XMVECTOR p = XMVectorLerp(p1, p2, animStateInfo.weight);

        XMVECTOR r1 = XMLoadFloat4(&animClips[animStateInfo.idx].keyFrames[animStateInfo.currentFrame][i].rot);
        XMVECTOR r2 = XMLoadFloat4(&animClips[animStateInfo.idx].keyFrames[animStateInfo.nextFrame][i].rot);
        XMVECTOR r = XMQuaternionSlerp(r1, r2, animStateInfo.weight);

        XMVECTOR s1 = XMLoadFloat3(&animClips[animStateInfo.idx].keyFrames[animStateInfo.currentFrame][i].scale);
        XMVECTOR s2 = XMLoadFloat3(&animClips[animStateInfo.idx].keyFrames[animStateInfo.nextFrame][i].scale);
        XMVECTOR s = XMVectorLerp(s1, s2, animStateInfo.weight);

        XMFLOAT3 pos,scale;
        XMStoreFloat3(&pos, p);
        XMStoreFloat3(&scale, s);
        XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
        XMMATRIX R = XMMatrixRotationQuaternion(r);
        XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
        nodeMatrixes[i] = S * R * T;
    }
    
    for (UINT i = 0; i < nodes.size(); ++i)
    {        
        if (nodes[i].parentIdx >= 0)
        {            
            nodeMatrixes[i] *= nodeMatrixes[nodes[i].parentIdx];
        }       
    }
    
    for (UINT i = 0; i < bones.size(); ++i)
    {
        boneData.data.bones[i] = bones[i].offset * nodeMatrixes[bones[i].idx];
    }
    if (boneData.Update())
    {
        dc->VSSetConstantBuffers(1, 1, boneData.GetAddressOf());
    }
}
