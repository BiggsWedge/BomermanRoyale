/*******************************************************
*	Filename:			EntityManager.h
*	Date:				07/16/2019
*	Mod. Date:			07/16/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contains the implementation
*						for the Entity Manager and all
*						of sub-systems
*******************************************************/

#include "EntityManager.h"
#include <fstream>
#include "CGame.h"

TRenderer3DModel CEntityManager::InitializeModel(TMeshTemplate tTemplate, DirectX::XMFLOAT3 position)
{
	TRenderer3DModel tToReturn;

	D3D11_BUFFER_DESC d3dVBuffDesc;
	D3D11_SUBRESOURCE_DATA d3dVBuffResource;
	ZeroMemory(&d3dVBuffDesc, sizeof(d3dVBuffDesc));
	ZeroMemory(&d3dVBuffResource, sizeof(d3dVBuffResource));
	d3dVBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dVBuffDesc.ByteWidth = sizeof(simpleVertex) * tTemplate.vtVertices.size();
	d3dVBuffDesc.CPUAccessFlags = 0;
	d3dVBuffDesc.MiscFlags = 0;
	d3dVBuffDesc.StructureByteStride = 0;
	d3dVBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	d3dVBuffResource.pSysMem = tTemplate.vtVertices.data();

	(*p_d3dGameDevice)->CreateBuffer(&d3dVBuffDesc, &d3dVBuffResource, &(tToReturn.tRendererMesh.p_d3dVertexBuffer));

	D3D11_BUFFER_DESC d3dIBuffDesc;
	D3D11_SUBRESOURCE_DATA d3dIBuffResource;
	ZeroMemory(&d3dIBuffDesc, sizeof(d3dIBuffDesc));
	ZeroMemory(&d3dIBuffResource, sizeof(d3dIBuffResource));
	d3dIBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dIBuffDesc.ByteWidth = sizeof(unsigned int) * tTemplate.vuIndices.size();
	d3dIBuffDesc.CPUAccessFlags = 0;
	d3dIBuffDesc.MiscFlags = 0;
	d3dIBuffDesc.StructureByteStride = 0;
	d3dIBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dIBuffResource.pSysMem = tTemplate.vuIndices.data();

	(*p_d3dGameDevice)->CreateBuffer(&d3dIBuffDesc, &d3dIBuffResource, &tToReturn.tRendererMesh.p_d3dIndexBuffer);
	tToReturn.tRendererMesh.numIndices = tTemplate.vuIndices.size();

	tToReturn.tRendererShaderSet.SetVertexShaderSet(*tTemplate.p_d3dUsedVertexShader, *tTemplate.p_d3dUsedInputLayout);
	tToReturn.tRendererTextureSet.SetResourceAt(tTemplate.v_d3dDiffuses[numCurrPlayers - 1], TRendererTextureSet::RESOURCE::DIFFUSE);
	tToReturn.tRendererTextureSet.SetResourceAt(tTemplate.d3dNormal, TRendererTextureSet::RESOURCE::NORMAL);
	tToReturn.tRendererTextureSet.SetResourceAt(tTemplate.d3dEmissive, TRendererTextureSet::RESOURCE::EMISSIVE);
	tToReturn.tRendererTextureSet.SetResourceAt(tTemplate.d3dSpecular, TRendererTextureSet::RESOURCE::SPECULAR);

	D3D11_BUFFER_DESC d3dVCBuffDesc;
	ZeroMemory(&d3dVCBuffDesc, sizeof(d3dVCBuffDesc));

	d3dVCBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dVCBuffDesc.ByteWidth = sizeof(TVertexConstBuffer);
	d3dVCBuffDesc.CPUAccessFlags = 0;
	d3dVCBuffDesc.MiscFlags = 0;
	d3dVCBuffDesc.StructureByteStride = 0;
	d3dVCBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	(*p_d3dGameDevice)->CreateBuffer(&d3dVCBuffDesc, nullptr, &tToReturn.p_d3dVertexConstBuffer);

	D3D11_BUFFER_DESC d3dPCBuffDesc;
	ZeroMemory(&d3dPCBuffDesc, sizeof(d3dPCBuffDesc));

	d3dPCBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dPCBuffDesc.ByteWidth = sizeof(tToReturn.tPixelConstBuffer);
	d3dPCBuffDesc.CPUAccessFlags = 0;
	d3dPCBuffDesc.MiscFlags = 0;
	d3dPCBuffDesc.StructureByteStride = 0;
	d3dPCBuffDesc.Usage = D3D11_USAGE_DEFAULT;

	(*p_d3dGameDevice)->CreateBuffer(&d3dPCBuffDesc, nullptr, &tToReturn.p_d3dPixelConstBuffer);

	(*p_d3dGameDevice)->CreateDeferredContext(0, &tToReturn.p_d3dDeferredContext);

	tToReturn.d3dPosition = position;

	tToReturn.p_d3dSamplerState = tTemplate.d3dSamState;

	tToReturn.d3dModelMatrix = DirectX::XMMatrixTranslation(tToReturn.d3dPosition.x, tToReturn.d3dPosition.y, tToReturn.d3dPosition.z);
	return tToReturn;
}

void CEntityManager::AddPlayerModel(AssetSet playerStuff)
{
	TMeshTemplate model;
	model.uID = vtMeshTemplates.size();
	vtMeshTemplates.resize(model.uID + 1);

	std::fstream file{ playerStuff.meshFile, std::ios_base::in | std::ios_base::binary };

	if (!file.is_open())
	{
		//log not open
	}

	int numIndices;
	file.read((char*)&numIndices, sizeof(int));
	model.vuIndices.resize(numIndices);
	file.read((char*)model.vuIndices.data(), sizeof(int) * numIndices);
	int numVerts;
	file.read((char*)&numVerts, sizeof(int));
	model.vtVertices.resize(numVerts);
	file.read((char*)model.vtVertices.data(), sizeof(simpleVertex) * numVerts);

	file.close();

	for (auto& v : model.vtVertices)
	{
		v.position.x = -v.position.x;
		v.normal.x = -v.normal.x;
		v.texCoords.y = 1.0f - v.texCoords.y;
	}


	std::fstream inMatFile{ playerStuff.matFile, std::ios_base::in | std::ios_base::binary };

	if (!inMatFile.is_open())
	{
		//log that not open
	}
	int numMatFiles;

	inMatFile.read((char*)&numMatFiles, sizeof(int));

	std::vector<material_t> mats;

	for (int i = 0; i < numMatFiles; ++i)
	{
		material_t temp;
		for (int j = 0; j < 4; ++j)
		{
			inMatFile.read((char*)&temp[j].value, sizeof(float) * 3);
			inMatFile.read((char*)&temp[j].factor, sizeof(float));
			inMatFile.read((char*)&temp[j].input, sizeof(int64_t));
			if (j == material_t::COMPONENT::DIFFUSE)
			{

				memcpy(model._mat.surfaceDiffuse, temp[j].value, sizeof(float) * 3);
				model._mat.diffuseFactor = temp[j].factor;
			}
			if (j == material_t::COMPONENT::EMISSIVE)
			{
				memcpy(model._mat.surfaceEmissive, temp[j].value, sizeof(float) * 3);
				model._mat.emissiveFactor = temp[j].factor;
			}
			if (j == material_t::COMPONENT::SPECULAR)
			{
				memcpy(model._mat.surfaceSpecular, temp[j].value, sizeof(float) * 3);
				model._mat.specularFactor = temp[j].factor;
			}
			if (j == material_t::COMPONENT::SHININESS)
			{
				memcpy(model._mat.surfaceShiny, temp[j].value, sizeof(float) * 3);
				model._mat.shininess = temp[j].factor;
			}
		}
		mats.push_back(temp);
	}

	int numPathsFiles;
	inMatFile.read((char*)&numPathsFiles, sizeof(int));

	for (int i = 0; i < numPathsFiles; ++i)
	{
		file_path_t _file;
		inMatFile.read(_file.data(), sizeof(char) * 260);
		model.filePaths.push_back(_file);
	}
	inMatFile.close();

	model.v_d3dDiffuses.resize(1);

	for (int i = 0; i < mats.size(); ++i)
	{
		for (int j = 0; j < material_t::COMPONENT::COUNT; ++j)
		{
			if (mats[i][j].input < 0)
				continue;

			std::experimental::filesystem::path filePath;
			filePath = model.filePaths[mats[i][j].input].data();
			HRESULT result;
			if (j == 0)
				result = DirectX::CreateWICTextureFromFile((*p_d3dGameDevice), filePath.wstring().c_str(), nullptr, &model.v_d3dDiffuses[0]);

		}
	}

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	(*p_d3dGameDevice)->CreateSamplerState(&sampDesc, &model.d3dSamState);



	std::vector<D3D11_INPUT_ELEMENT_DESC> regShaderDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"JOINTS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	(*p_d3dGameDevice)->CreateInputLayout(regShaderDesc.data(), regShaderDesc.size(), BasicVertex, sizeof(BasicVertex), model.p_d3dUsedInputLayout);


	vtMeshTemplates[model.uID] = model;
}

void CEntityManager::Initialize(CGame* _theGame)
{
	AssetSet battleMageAssetSet;
	battleMageAssetSet.meshFile = ".//Assets//BattleMage.mesh";
	battleMageAssetSet.matFile = ".//Assets//BattleMage.mat";
	battleMageAssetSet.usedPixelShader = _theGame->GetRenderer()->GetPixelShaderAt(PIXEL_SHADER::BASIC);
	battleMageAssetSet.usedVertexShader = _theGame->GetRenderer()->GetertexShaderAt(VERTEX_SHADER::BASIC);

	AddPlayerModel(battleMageAssetSet);

}

CEntityManager::CEntityManager()
{

}
