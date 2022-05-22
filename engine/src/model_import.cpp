#include "stdafx.h"
#include "model_import.h"
#include <d3d11.h>
#include <renderer.h>

using namespace DirectX;

bool startsWith(const std::string& line, const std::string& test) {
    for (int i = 0; i < test.length(); i++) {
        if (line[i] != test[i]) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> parts;
    int idx = 0;
    char c = -1;
    std::string currentPart = "";
    while (c != 0) {
        while ((c = input[idx++]) != delimiter) {
            currentPart += c;
            if (idx == (input.length())) {
                c= 0;
                break;
            }
        }
        parts.push_back(currentPart);
        currentPart = "";
    }
    return parts;
}

static void parsePosition(const std::string& line, ModelImportData& modelImportData) {
    auto parts = split(line, ' ');
    float x = atof(parts[1].c_str());
    float y = atof(parts[2].c_str());
    float z = atof(parts[3].c_str());
    modelImportData.positions.push_back( {x, y, z});
}

static void parseTexture(const std::string& line, ModelImportData& modelImportData) {
    auto parts = split(line, ' ');
    float x = atof(parts[1].c_str());
    float y = atof(parts[2].c_str());
    modelImportData.uvs.push_back( {x, y});
}

static void parseNormals(const std::string& line, ModelImportData& modelImportData) {
    auto parts = split(line, ' ');
    float x = atof(parts[1].c_str());
    float y = atof(parts[2].c_str());
    float z = atof(parts[3].c_str());
    modelImportData.normals.push_back( {x, y, z});
}

static void parseFace(const std::string& line, ModelImportData& modelImportData) {
    auto parts = split(line, ' ');
    for (auto p : parts) {
        if (p == "f") continue;
        auto indexParts = split(p, '/');
        auto v = atoi(indexParts[0].c_str()) - 1;
        auto vt = atoi(indexParts[1].c_str()) - 1;
        auto vn = atoi(indexParts[2].c_str()) - 1;
        modelImportData.indices.push_back( v);
        modelImportData.uvsFinal[v] = modelImportData.uvs[vt];
        modelImportData.normalsFinal[v] = modelImportData.normals[vn];
    }

}


static void parseLine(const std::string& line, ModelImportData& modelImportData) {
    if (startsWith(line, "mtllib") || startsWith(line, "#") ||
            startsWith(line, "o ") ||
            startsWith(line, "usemtl") ||
            startsWith(line, "s")) {
        return;
    }
    if (startsWith(line, "v ")) {
        parsePosition(line, modelImportData);
    }
    else if (startsWith(line, "vt ")) {
        parseTexture(line, modelImportData);
    }
    else if (startsWith(line, "vn ")) {
        parseNormals(line, modelImportData);
    }
    else if (startsWith(line, "f")) {
        static bool firstFace = false;
        if (!firstFace) {
            firstFace = true;
            modelImportData.uvsFinal.resize(modelImportData.positions.size());
            modelImportData.normalsFinal.resize(modelImportData.positions.size());
        }
        parseFace(line, modelImportData);
    }
}

bool importObjModel(const std::string& file,
	std::vector<XMFLOAT3>& positions,
	std::vector<XMFLOAT2>& uvsFinal,
	std::vector<XMFLOAT3>& normalsFinal,
	std::vector<UINT>& indices) {
    char* rawContents = readTextFile(file);

    std::vector<XMFLOAT2> uvs;
    std::vector<XMFLOAT3> normals;

    auto data = ModelImportData{positions, uvs, normals, uvsFinal, normalsFinal, indices};

    char c;
    int idx = 0;
    std::vector<std::string> lines;
    while ((c = rawContents[idx]) != 0 ) {
        auto line = std::string {};
        while ((c = rawContents[idx++]) != '\n') {
            line.push_back(c++);
        }
        parseLine(line, data);
    }

//	unsigned int numMeshes = scene->mNumMeshes;
//	for (int i = 0; i < numMeshes; ++i) {
//		aiMesh* mesh = scene->mMeshes[i];
//		for (int v = 0; v < mesh->mNumVertices; ++v) {
//			aiVector3D vertex = mesh->mVertices[v];
//			aiVector3D normal = mesh->mNormals[v];
//			aiVector3D texcoord = mesh->mTextureCoords[0][v];
//
//			positions.push_back({ vertex.x, vertex.y, vertex.z });
//			normals.push_back({ normal.x, normal.y, normal.z });
//			uvs.push_back({ texcoord.x, texcoord.y });
//
//		}
//
//		for (int f = 0; f < mesh->mNumFaces; ++f) {
//			aiFace face = mesh->mFaces[f];
//			indices.push_back({ face.mIndices[0] });
//			indices.push_back({ face.mIndices[1] });
//			indices.push_back({ face.mIndices[2] });
//		}
//	}

    return true;
}


bool importObjModel(const std::string& file, Model* model, Renderer& renderer) {
	std::vector<XMFLOAT3> pos;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> normals;
	std::vector<UINT> ind;
	importObjModel(file, pos, uvs, normals, ind);
	model->positions = pos;
	model->normals = normals;
	model->uvs = uvs;
	model->indices = ind;

	// pos buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(XMFLOAT3) * pos.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	renderer.getDevice()->CreateBuffer(&bd, NULL, &model->posBuffer);
	D3D11_MAPPED_SUBRESOURCE ms;
	renderer.getContext()->Map(model->posBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, pos.data(), sizeof(XMFLOAT3) * pos.size());
	renderer.getContext()->Unmap(model->posBuffer, NULL);

	// uv buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT2) * uvs.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA uvData;
	uvData.pSysMem = uvs.data();
	uvData.SysMemPitch = 0;
	uvData.SysMemSlicePitch = 0;
	renderer.getDevice()->CreateBuffer(&bd, &uvData, &model->uvBuffer);

	// normal buffer
	
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT3) * normals.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA normalsData;
	normalsData.pSysMem = normals.data();
	normalsData.SysMemPitch = 0;
	normalsData.SysMemSlicePitch = 0;
	renderer.getDevice()->CreateBuffer(&bd, &normalsData, &model->normalBuffer);

	// index buffer
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned int) * ind.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = ind.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	ID3D11Buffer* indexBuffer;
	renderer.getDevice()->CreateBuffer(&ibd, &indexData, &model->indexBuffer);

	return true;
}

char *readTextFile(const std::string &fileName) {
    FILE *f = fopen(fileName.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char * contents = (char*)  malloc(fsize + 1);
    fread(contents, fsize, 1, f);
    fclose(f);

    contents[fsize] = 0;
    return contents;
}
