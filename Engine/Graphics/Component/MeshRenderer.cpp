#include "MeshRenderer.h"
#include "../../../ComException.h"
#include "../Object/GameObject.h"
#include "../../EngineUtils.h"

bool MeshRenderer::LoadFile(const string& filePath)
{
    directory = EngineUtils::GetDirectoryFromPath(filePath);
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
    if (pScene == nullptr) return false;
    ProcessNode(pScene->mRootNode, pScene, XMMatrixIdentity());
    return true;
}
void MeshRenderer::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parent)
{
    XMMATRIX local = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parent;
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(this->ProcessMesh(mesh, scene, local));
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene, local);
    }
}
Mesh<Vertex> MeshRenderer::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& mat)
{
    vector<Vertex> vertices;
    vector<DWORD> indices;

    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex v;
        v.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = { mesh->mNormals[i].x,mesh->mNormals[i].y ,mesh->mNormals[i].z };
        if (mesh->mTextureCoords[0])
        {
            v.uv.x = (float)mesh->mTextureCoords[0][i].x;
            v.uv.y = (float)mesh->mTextureCoords[0][i].y;
        }
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

    return Mesh<Vertex>(device, dc, vertices, indices, textures, mat);
}
TextureStorageType MeshRenderer::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int idx, aiTextureType type)
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
vector<Texture> MeshRenderer::LoadMaterialTextures(aiMaterial* pMat, aiTextureType type, const aiScene* pScene)
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

int MeshRenderer::GetTextureIndex(aiString* pStr)
{
    assert(pStr->length >= 2);
    return atoi(&pStr->C_Str()[1]);
}

void MeshRenderer::Update()
{
}

bool MeshRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, ConstantBuffer<CB_VS_Transform>& cb_vs_transform)
{
    this->device = device;
    this->dc = dc;
    texture = tex;
    cb_transform = &cb_vs_transform;
    try
    {
        Vertex v[] =
        {
            //Front
            Vertex(-0.5f, 0.5f, -0.5f, XMFLOAT3(1,1,1),{0,0}),//0
            Vertex(0.5f, 0.5f, -0.5f, XMFLOAT3(1,1,1), {1,0}),//1
            Vertex(-0.5f, -0.5f, -0.5f, XMFLOAT3(1,1,1), {0,1}),//2        
            Vertex(0.5f, -0.5f, -0.5f, XMFLOAT3(1,1,1), {1,1}),//3        
            //Back
            Vertex(-0.5f, 0.5f, 0.5f, XMFLOAT3(1,1,1),{1,0}),//4
            Vertex(0.5f, 0.5f, 0.5f, XMFLOAT3(1,1,1), {0,0}),//5
            Vertex(-0.5f, -0.5f, 0.5f, XMFLOAT3(1,1,1), {1,1}),//6        
            Vertex(0.5f, -0.5f, 0.5f, XMFLOAT3(1,1,1), {0,1}),//7 
            //Up
            Vertex(-0.5f, 0.5f, 0.5f, XMFLOAT3(1,1,1),{0,0}),//8
            Vertex(0.5f, 0.5f, 0.5f, XMFLOAT3(1,1,1), {1,0}),//9
            Vertex(-0.5f, 0.5f, -0.5f, XMFLOAT3(1,1,1), {0,1}),//10        
            Vertex(0.5f, 0.5f, -0.5f, XMFLOAT3(1,1,1), {1,1}),//11 
            //Down
            Vertex(-0.5f, -0.5f, 0.5f, XMFLOAT3(1,1,1),{1,0}),//12
            Vertex(0.5f, -0.5f, 0.5f, XMFLOAT3(1,1,1), {0,0}),//13
            Vertex(-0.5f, -0.5f, -0.5f, XMFLOAT3(1,1,1), {1,1}),//14        
            Vertex(0.5f, -0.5f, -0.5f, XMFLOAT3(1,1,1), {0,1}),//15 
            //Right
            Vertex(0.5f, 0.5f, -0.5f, XMFLOAT3(1,1,1),{0,0}),//16
            Vertex(0.5f, 0.5f, 0.5f, XMFLOAT3(1,1,1), {1,0}),//17
            Vertex(0.5f, -0.5f, -0.5f, XMFLOAT3(1,1,1), {0,1}),//18        
            Vertex(0.5f, -0.5f, 0.5f, XMFLOAT3(1,1,1), {1,1}),//19 
            //Left
            Vertex(-0.5f, 0.5f, -0.5f, XMFLOAT3(1,1,1),{0,0}),//20
            Vertex(-0.5f, 0.5f, 0.5f, XMFLOAT3(1,1,1), {1,0}),//21
            Vertex(-0.5f, -0.5f, -0.5f, XMFLOAT3(1,1,1), {0,1}),//22        
            Vertex(-0.5f, -0.5f, 0.5f, XMFLOAT3(1,1,1), {1,1}),//23 

        };

        HRESULT hr = vb.Initialize(device, v, ARRAYSIZE(v));
        COM_ERROR_IF_FAILED(hr, L"버텍스 버퍼 생성에 실패 하였습니다.");

        DWORD indices[] =
        {
            //Front
            0, 1, 2,
            1, 3, 2,
            //Back
            4, 6, 5,
            5, 6, 7,
            //Up
            8, 9, 10,
            9, 11, 10,
            //Down
            12, 14, 13,
            13, 14, 15,
            //Right
            16, 17, 18,
            17, 19, 18,
            //Left
            20, 22, 21,
            21, 22, 23
        };

        hr = ib.Initialize(device, indices, ARRAYSIZE(indices));
        COM_ERROR_IF_FAILED(hr, L"인덱스 버퍼 생성에 실패 하였습니다.");
    }
    catch (ComException& ex)
    {
        Debug::Log(ex.what());
        return false;
    }
}

void MeshRenderer::Draw(const XMMATRIX vpMat)
{
    UINT offset = 0;
    dc->VSSetConstantBuffers(0, 1, cb_transform->GetAddressOf());
    dc->PSSetShaderResources(0, 1, &texture);

    for (Mesh<Vertex> mesh : meshes)
    {
        cb_transform->data.world = mesh.GetLocal() * gameObject->GetTransform()->GetWorld();
        cb_transform->data.vp = vpMat;
        cb_transform->Update();
        mesh.Draw();
    }
}
