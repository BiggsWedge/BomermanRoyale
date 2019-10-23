#include "Utilities.h"

using namespace std;
using namespace GW;
using namespace SYSTEM;
using namespace AUDIO;

constexpr size_t MAX_LINE_VERTS = 9000;

size_t line_vert_count = 0;
std::array< TLineVertex, MAX_LINE_VERTS> line_verts;


GLog* g_pLogger = nullptr;
GWindow* g_pWindow = nullptr;
GInput* g_pInputRecord = nullptr;
GAudio* g_pAudioHolder = nullptr;
GMusic* g_pMusicStream = nullptr;
GMusic* g_pMusicStream1 = nullptr;
GMusic* g_pMusicStream2 = nullptr;
GSound* g_pSoundPlayer = nullptr;
GSound* walkSound1;
GSound* bombPlaceSound1;
GSound* spawnSound1;
GSound* bombPlaceSound2;
GSound* spawnSound2;
GSound* warnSound;
GSound* fallingSound;
GSound* playerfallingSound;
GSound* WinScreenSound;
GSound* DeathSound;

std::vector<GSound*> MenuSounds;
std::vector<GSound*> explosionSound;
std::vector<GSound*> bombPlaceSound;
std::vector<GSound*> powerUpSound;
GController* g_pControllerInput = nullptr;

std::vector<TMeshTemplate> v_tMeshTemplates = {};

std::vector<unsigned int> availablePlayerModels = { MODELS::CHICKEN, MODELS::GOAT, MODELS::BOAR };
std::vector<unsigned int> availablePlayerTextures = { DIFFUSE_TEXTURES::CHICKEN1, DIFFUSE_TEXTURES::GOAT, DIFFUSE_TEXTURES::BOAR };

string GetCurrentDateAndTime()
{
	string currDateTime;
	time_t now = time(0);
	tm lmt;
	localtime_s(&lmt, &now);

	currDateTime.clear();
	currDateTime.append(std::to_string(1 + lmt.tm_mon));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_mday));
	currDateTime.append(".");
	currDateTime.append(std::to_string(1900 + lmt.tm_year));
	currDateTime.append("_");
	currDateTime.append(std::to_string(lmt.tm_hour));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_min));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_sec));

	return currDateTime;
}

bool InitializeLogger()
{
	string cOutFileName;
	cOutFileName.append("ErrorLog");
	cOutFileName.append(GetCurrentDateAndTime());
	cOutFileName.append(".log");

	if (G_SUCCESS(CreateGLog(cOutFileName.c_str(), &g_pLogger)))
	{
#ifdef _DEBUG
		g_pLogger->EnableConsoleLogging(true);
#else
		g_pLogger->EnableConsoleLogging(false);
#endif // !_DEBUG

		g_pLogger->EnableVerboseLogging(true);
		g_pLogger->LogCatergorized("SUCCESS", "Error Logging system successfully initialized.");
		return true;
	}

	return false;
}

bool InitializeWindow() {
	if (G_SUCCESS(CreateGWindow(100, 100, 1024, 720, GWindowStyle::WINDOWEDBORDERED, &g_pWindow))) {
		g_pLogger->LogCatergorized("SUCCESS", "Window successfully created.");
		return true;
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Window unsuccessfully created.");
		return false;
	}
}

bool InitializeInput()
{
	if (G_SUCCESS(CreateGInput(g_pWindow, sizeof(g_pWindow), &g_pInputRecord))) {
		g_pLogger->LogCatergorized("SUCCESS", "Input Manager successfully created.");
		return true;
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Input Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeControllerInput() {
	if (G_SUCCESS(CreateGController(G_XBOX_CONTROLLER, &g_pControllerInput)))
	{
		g_pLogger->LogCatergorized("SUCCESS", "Controller Input Manager successfully created.");
		return true;
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Controller Input Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeAudio() {
	if (G_SUCCESS(CreateGAudio(&g_pAudioHolder))) {
		if (G_SUCCESS(g_pAudioHolder->Init(2))) {
			g_pLogger->LogCatergorized("SUCCESS", "Audio Manager successfully created.");
			return true;
		}
		else
		{
			g_pLogger->LogCatergorized("FAILURE", "Audio Manager unsuccessfully created.");
			return false;
		}
	}
	else {
		g_pLogger->LogCatergorized("FAILURE", "Audio Manager unsuccessfully created.");
		return false;
	}
}

bool InitializeGlobals()
{
	if (!InitializeLogger())
		return false;
	if (!InitializeWindow())
		return false;
	if (!InitializeInput())
		return false;
	if (!InitializeAudio())
		return false;
	if (!InitializeControllerInput())
		return false;
	g_d3dData = new DirectXData();
	if (!g_d3dData->Initialize())
		return false;

	return true;
}

void LoadModels()
{
	for (int i = 0; i < modelLoadInfos.size(); ++i)
		LoadModel(modelLoadInfos[i]);
}

void LoadModel(TMeshLoadInfo loadInfo)
{
	int index = v_tMeshTemplates.size();
	int numIndices;
	int numVerts;

	TMeshTemplate temp/* = TMeshTemplate()*/;
	temp.uID = index;
	temp.sName = loadInfo.name;

	using namespace DirectX;
	if (loadInfo.meshFile)
	{
		std::fstream file{ loadInfo.meshFile, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		file.read((char*)&numIndices, sizeof(int));
		temp.v_iIndices.resize(numIndices);
		file.read((char*)temp.v_iIndices.data(), sizeof(int) * numIndices);

		file.read((char*)&numVerts, sizeof(int));
		temp.v_tVertices.resize(numVerts);
		file.read((char*)temp.v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);

		file.close();

		for (auto& v : temp.v_tVertices)
		{
			v.fPosition.x = -v.fPosition.x;
		}
		for (int i = 0; i < numIndices; i += 3)
		{
			XMFLOAT3 vEdge1, vEdge2;
			vEdge1.x = temp.v_tVertices[temp.v_iIndices[i + 1]].fPosition.x - temp.v_tVertices[temp.v_iIndices[i]].fPosition.x;
			vEdge1.y = temp.v_tVertices[temp.v_iIndices[i + 1]].fPosition.y - temp.v_tVertices[temp.v_iIndices[i]].fPosition.y;
			vEdge1.z = temp.v_tVertices[temp.v_iIndices[i + 1]].fPosition.z - temp.v_tVertices[temp.v_iIndices[i]].fPosition.z;

			vEdge2.x = temp.v_tVertices[temp.v_iIndices[i + 2]].fPosition.x - temp.v_tVertices[temp.v_iIndices[i]].fPosition.x;
			vEdge2.y = temp.v_tVertices[temp.v_iIndices[i + 2]].fPosition.y - temp.v_tVertices[temp.v_iIndices[i]].fPosition.y;
			vEdge2.z = temp.v_tVertices[temp.v_iIndices[i + 2]].fPosition.z - temp.v_tVertices[temp.v_iIndices[i]].fPosition.z;

			XMFLOAT2 tEdge1, tEdge2;
			tEdge1.x = temp.v_tVertices[temp.v_iIndices[i + 1]].fTexCoord.x - temp.v_tVertices[temp.v_iIndices[i]].fTexCoord.x;
			tEdge1.y = temp.v_tVertices[temp.v_iIndices[i + 1]].fTexCoord.y - temp.v_tVertices[temp.v_iIndices[i]].fTexCoord.y;

			tEdge2.x = temp.v_tVertices[temp.v_iIndices[i + 2]].fTexCoord.x - temp.v_tVertices[temp.v_iIndices[i]].fTexCoord.x;
			tEdge2.y = temp.v_tVertices[temp.v_iIndices[i + 2]].fTexCoord.y - temp.v_tVertices[temp.v_iIndices[i]].fTexCoord.y;

			float ratio = 1.0f / (tEdge1.x * tEdge2.y - tEdge1.y * tEdge2.x);

			XMFLOAT3 uDirection = XMFLOAT3
			(
				(tEdge2.y * vEdge1.x - tEdge1.y * vEdge2.x) * ratio,
				(tEdge2.y * vEdge1.y - tEdge1.y * vEdge2.y) * ratio,
				(tEdge2.y * vEdge1.z - tEdge1.y * vEdge2.z) * ratio
			);

			XMFLOAT3 vDirection = XMFLOAT3
			(
				(tEdge1.x * vEdge2.x - tEdge2.x * vEdge1.x) * ratio,
				(tEdge1.x * vEdge2.y - tEdge2.x * vEdge1.y) * ratio,
				(tEdge1.x * vEdge2.z - tEdge2.x * vEdge1.z) * ratio
			);

			XMVECTOR uDirVec = XMVector3Normalize(XMLoadFloat3(&uDirection));
			XMVECTOR vDirVec = XMVector3Normalize(XMLoadFloat3(&vDirection));

			for (int j = 0; j < 3; ++j)
			{
				XMVECTOR dotResult = XMVector3Dot(XMLoadFloat3(&temp.v_tVertices[temp.v_iIndices[i + j]].fNormal), uDirVec);

				XMVECTOR tang = uDirVec - XMLoadFloat3(&temp.v_tVertices[temp.v_iIndices[i + j]].fNormal) * dotResult;

				tang = XMVector3Normalize(tang);

				XMVECTOR cross = XMVector3Cross(XMLoadFloat3(&temp.v_tVertices[temp.v_iIndices[i + j]].fNormal), uDirVec);
				XMVECTOR handedness = vDirVec;

				dotResult = XMVector3Dot(cross, handedness);

				XMFLOAT4 dotResFLoats;
				XMStoreFloat4(&dotResFLoats, dotResult);

				XMFLOAT4 _tang;
				XMStoreFloat4(&_tang, tang);

				_tang.w = (dotResFLoats.x < 0.0f) ? -1.0f : 1.0f;

				temp.v_tVertices[temp.v_iIndices[i + j]].fTangents = _tang;
			}
		}

	}
	v_tMeshTemplates.push_back(temp);
}

void LoadMenuScreen(int width, int height, int numbuttons, const char* matFile) {
	int index = v_tMeshTemplates.size();
	int numIndices = 6;
	int numVerts = 4;
	float SSpaceWidth = (float)width * 0.5f;
	float SSpaceHeight = (float)height * 0.5f;

	TMeshTemplate temp/* = TMeshTemplate()*/;

	temp.uID = index;
	temp.v_iIndices.resize(numIndices);
	temp.v_tVertices.resize(numVerts);
	temp.v_tVertices.at(0).fPosition = DirectX::XMFLOAT3(-(SSpaceWidth), -(SSpaceHeight), 0.0f);
	temp.v_tVertices.at(1).fPosition = DirectX::XMFLOAT3((SSpaceWidth), -(SSpaceHeight), 0.0f);
	temp.v_tVertices.at(2).fPosition = DirectX::XMFLOAT3(-(SSpaceWidth), (SSpaceHeight), 0.0f);
	temp.v_tVertices.at(3).fPosition = DirectX::XMFLOAT3((SSpaceWidth), (SSpaceHeight), 0.0f);

	temp.v_tVertices.at(0).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	temp.v_tVertices.at(1).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	temp.v_tVertices.at(2).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	temp.v_tVertices.at(3).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	temp.v_tVertices.at(0).fTexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
	temp.v_tVertices.at(1).fTexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
	temp.v_tVertices.at(2).fTexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
	temp.v_tVertices.at(3).fTexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);

	temp.v_iIndices.at(0) = 0;
	temp.v_iIndices.at(1) = 2;
	temp.v_iIndices.at(2) = 1;
	temp.v_iIndices.at(3) = 1;
	temp.v_iIndices.at(4) = 2;
	temp.v_iIndices.at(5) = 3;

	v_tMeshTemplates.push_back(temp);

	for (int i = 0; i < numbuttons; ++i) {
		float bWidth = width / 3.0f;
		float bHeight = height / 10.0f;
		bWidth = bWidth / 2.0f;
		bHeight = bHeight / 2.0f;

		temp.uID = index;
		temp.v_iIndices.resize(numIndices);
		temp.v_tVertices.resize(numVerts);

		temp.v_tVertices.at(0).fPosition = DirectX::XMFLOAT3(-(bWidth), -(bHeight), 0.0f);
		temp.v_tVertices.at(1).fPosition = DirectX::XMFLOAT3((bWidth), -(bHeight), 0.0f);
		temp.v_tVertices.at(2).fPosition = DirectX::XMFLOAT3(-(bWidth), (bHeight), 0.0f);
		temp.v_tVertices.at(3).fPosition = DirectX::XMFLOAT3((bWidth), (bHeight), 0.0f);

		temp.v_tVertices.at(0).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		temp.v_tVertices.at(1).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		temp.v_tVertices.at(2).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		temp.v_tVertices.at(3).fNormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

		temp.v_tVertices.at(0).fTexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
		temp.v_tVertices.at(1).fTexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);
		temp.v_tVertices.at(2).fTexCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
		temp.v_tVertices.at(3).fTexCoord = DirectX::XMFLOAT2(0.0f, 0.0f);

		temp.v_iIndices.at(0) = 0;
		temp.v_iIndices.at(1) = 2;
		temp.v_iIndices.at(2) = 1;
		temp.v_iIndices.at(3) = 1;
		temp.v_iIndices.at(4) = 2;
		temp.v_iIndices.at(5) = 3;

		v_tMeshTemplates.push_back(temp);
	}
}

void LoadTextures()
{
	for (int i = 0; i < (int)diffuseTextures.size(); ++i)
	{
		if (FAILED(DirectX::CreateWICTextureFromFile(g_d3dData->d3dDevice, diffuseTextures[i], nullptr, &g_d3dData->d3dDiffuseTextures[i])))
		{
			std::string outS = "Failed to load texture ";
			outS.append(std::to_string(i));
			outS.append(" of ");
			outS.append(std::to_string(diffuseTextures.size()));
			g_pLogger->LogCatergorized("FAILURE", outS.c_str());
		}
	}
}

void add_line(DirectX::XMFLOAT3 point_a, DirectX::XMFLOAT3 point_b, DirectX::XMFLOAT4 color_a, DirectX::XMFLOAT4 color_b) {
	line_verts[line_vert_count].pos = point_a;
	line_verts[line_vert_count].color = color_a;
	line_vert_count += 1;

	line_verts[line_vert_count].pos = point_b;
	line_verts[line_vert_count].color = color_b;
	line_vert_count += 1;
}

void clear_lines()
{
	line_vert_count = 0;
}

const TLineVertex* get_line_verts()
{
	return line_verts.data();
}

size_t get_line_vert_count()
{
	return line_vert_count;
}

size_t get_line_vert_capacity()
{
	return MAX_LINE_VERTS;
}

void drawAABB(float3 point_a, float3 point_b, float3 point_c, float3 point_d, float3 point_e, float3 point_f, float3 point_g, float3 point_h, float4 color1, float4 color2)
{
	add_line(point_a, point_b, color1, color2);
	add_line(point_b, point_c, color1, color2);
	add_line(point_c, point_d, color1, color2);
	add_line(point_d, point_a, color1, color2);
	add_line(point_e, point_f, color1, color2);
	add_line(point_f, point_g, color1, color2);
	add_line(point_g, point_h, color1, color2);
	add_line(point_h, point_e, color1, color2);
	add_line(point_a, point_e, color1, color2);
	add_line(point_b, point_f, color1, color2);
	add_line(point_c, point_g, color1, color2);
	add_line(point_d, point_h, color1, color2);
}

void drawAABB(DirectX::XMFLOAT3 tlf, DirectX::XMFLOAT3 tlb, DirectX::XMFLOAT3 trf, DirectX::XMFLOAT3 trb, DirectX::XMFLOAT3 blf, DirectX::XMFLOAT3 brf, DirectX::XMFLOAT3 blb, DirectX::XMFLOAT3 brb, DirectX::XMFLOAT4 color)
{
	add_line(tlf, tlb, color);
	add_line(tlf, trf, color);
	add_line(tlf, blf, color);

	add_line(brb, trb, color);
	add_line(brb, blb, color);
	add_line(brb, brf, color);
}

float3 XMVector2Float3(DirectX::XMVECTOR vector)
{
	float3 point;

	point.x = DirectX::XMVectorGetX(vector);
	point.y = DirectX::XMVectorGetY(vector);
	point.z = DirectX::XMVectorGetZ(vector);

	return point;
}

DirectX::XMVECTOR Float32XMVector(float3 point)
{
	DirectX::XMVECTOR vector = { point.x, point.y, point.z, 1.0f };

	return vector;
}

void GetCorners(float3 _center, float3 _extents, float3*& corners)
{
	float newX, newY, newZ;

	newX = _center.x - _extents.x;
	newY = _center.y - _extents.y;
	newZ = _center.z - _extents.z;

	corners[0] = { newX,newY,newZ };

	newX = _center.x + _extents.x;
	newY = _center.y + _extents.y;
	newZ = _center.z + _extents.z;

	corners[1] = { newX,newY,newZ };

	corners[2] = { corners[0].x,corners[0].y,corners[1].z };
	corners[3] = { corners[0].x,corners[1].y,corners[0].z };
	corners[4] = { corners[1].x,corners[0].y,corners[0].z };
	corners[5] = { corners[0].x,corners[1].y,corners[1].z };
	corners[6] = { corners[1].x,corners[0].y,corners[1].z };
	corners[7] = { corners[1].x,corners[1].y,corners[0].z };
}

void TMeshTemplate::loadModel(const char* modelFile, const char* matFile, const char* animFile, float scale)
{
	using namespace DirectX;
	if (modelFile)
	{
		std::fstream file{ modelFile, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		file.read((char*)&numIndices, sizeof(int));
		v_iIndices.resize(numIndices);
		file.read((char*)v_iIndices.data(), sizeof(int) * numIndices);

		file.read((char*)&numVerts, sizeof(int));
		v_tVertices.resize(numVerts);
		file.read((char*)v_tVertices.data(), sizeof(TSimpleVertex) * numVerts);

		file.close();
	}
}


/*
void TMeshTemplate::render(ID3D11DeviceContext* _context, double timepassed)
{

	animTime += timepassed;
	totalTime += timepassed;

	float fractionalTime = timepassed - (int)timepassed;

	UINT strides = sizeof(TSimpleVertex);
	UINT offsets = 0;

	if (currKeyFrame == _anim.frames.size())
		currKeyFrame = 0;

	while (animTime > _anim.frames[currFrameIndex + 1].time)
	{
		currFrameIndex++;
		if (currFrameIndex == _anim.frames.size() - 1)
		{
			animTime -= _anim.duration;
			currFrameIndex = 0;
		}
	}

	float startTime, endTime;
	startTime = _anim.frames[currFrameIndex].time;
	endTime = _anim.frames[currFrameIndex + 1].time;

	float ratio = (animTime - startTime) / (endTime - startTime);

	KeyFrame _key = _anim.frames[currFrameIndex];

	jointCB _jointsConst;
	_jointsConst.numJoints = _anim.frames[currFrameIndex].joints.size();

	for (int i = 0; i < _anim.frames[currFrameIndex].joints.size(); ++i)
	{

		DirectX::XMMATRIX tween = matLerp(_anim.frames[currFrameIndex].joints[i]._mat, _anim.frames[currFrameIndex + 1].joints[i]._mat, ratio);

		//debug_renderer::drawMatrix(tween);

		_jointsConst._joints[i] = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, _bindPose[i]._mat) * tween);

		if (_key.joints[i].parentIndex >= 0)
		{
			DirectX::XMFLOAT4X4 startMat, endMat;

			DirectX::XMStoreFloat4x4(&startMat, matLerp(_anim.frames[currFrameIndex].joints[i]._mat, _anim.frames[currFrameIndex + 1].joints[i]._mat, ratio));
			DirectX::XMStoreFloat4x4(&endMat, matLerp(_anim.frames[currFrameIndex].joints[_anim.frames[currFrameIndex].joints[i].parentIndex]._mat, _anim.frames[currFrameIndex + 1].joints[_anim.frames[currFrameIndex + 1].joints[i].parentIndex]._mat, ratio));
		}


	}

	MVP_t debugConstBuff;
	//debugConstBuff.world = DirectX::XMMatrixTranspose(mObjMatrix);

	if (g_d3dData->bUseDebugRenderCamera)
		debugConstBuff.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(0, g_d3dData->debugCamMat));
	else
		debugConstBuff.view = DirectX::XMMatrixTranspose(g_d3dData->viewMat);

	debugConstBuff.projection = DirectX::XMMatrixTranspose(g_d3dData->projMat);

#pragma endregion

	Light pointLight;
	pointLight.position = DirectX::XMFLOAT3(0.0, 6.0f, -2.0f);
	pointLight.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	pointLight.intensity = 0.25f;
	pointLight.type = 0;

	LightBuffer _light;
	_light.light = pointLight;

	//MatBuffer mat;
	//mat.material = _mat;

	debugConstBuff.world = DirectX::XMMatrixIdentity();
	debugConstBuff.world  *= DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	debugConstBuff.world = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180));
	debugConstBuff.world = debugConstBuff.world * DirectX::XMMatrixTranslation(6.0f, 0.0f, 0.0f);
	debugConstBuff.world = DirectX::XMMatrixTranspose(debugConstBuff.world);

	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MVP_t], 0, nullptr, &debugConstBuff, 0, 0);
	_context->VSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MVP_t]);

	_context->IASetInputLayout(g_d3dData->d3dInputLayout[INPUT_LAYOUT::BASIC]);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::LIGHTS], 0, nullptr, &_light, 0, 0);
	_context->PSSetConstantBuffers(0, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::LIGHTS]);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MATERIAL], 0, nullptr, &_mat, 0, 0);
	_context->PSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::MATERIAL]);

	_context->UpdateSubresource(g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS], 0, nullptr, &_jointsConst, 0, 0);
	_context->VSSetConstantBuffers(1, 1, &g_d3dData->d3dConstBuffers[CONSTANT_BUFFER::JOINTS]);

	_context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &strides, &offsets);

	_context->VSSetShader(g_d3dData->d3dVertexShader[VERTEX_SHADER::ANIM], 0, 0);
	_context->PSSetShader(g_d3dData->d3dPixelShader[PIXEL_SHADER::ANIM], 0, 0);
	_context->PSSetSamplers(0, 1, &_samState);
	_context->PSSetShaderResources(0, 1, &_srv[DIFFUSE]);
	_context->PSSetShaderResources(1, 1, &_srv[EMISSIVE]);
	_context->PSSetShaderResources(2, 1, &_srv[SPECULAR]);

	_context->DrawIndexed(numIndices, 0, 0);
}
*/

//TMeshTemplate::~TMeshTemplate()
//{
//	SAFE_RELEASE(_vertexBuffer);
//	SAFE_RELEASE(_indexBuffer);
//	SAFE_RELEASE(_samState);
//	for (int i = 0; i < TEXTURES::COUNT; i++)
//	{
//		SAFE_RELEASE(_srv[i]);
//		SAFE_RELEASE(_textures[i]);
//	}
//
//
//}
//
//TMeshTemplate::TMeshTemplate()
//{
//	_vertexBuffer = nullptr;
//	_indexBuffer = nullptr;
//	_samState = nullptr;
//	for (int i = 0; i < TEXTURES::COUNT; i++)
//	{
//		_srv[i] = nullptr;
//		_textures[i] = nullptr;
//
//	}
//}

float lerp(float x, float y, float ratio)
{
	return (y - x) * ratio + x;

}

DirectX::XMMATRIX matLerp(DirectX::XMMATRIX x, DirectX::XMMATRIX y, float ratio)
{
	DirectX::XMMATRIX toRet = x;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			toRet.r[i].m128_f32[j] = lerp(x.r[i].m128_f32[j], y.r[i].m128_f32[j], ratio);
		}
	}
	/*
	for (int i = 0; i < 3; ++i)
		toRet.r[3].m128_f32[i] = lerp(x.r[3].m128_f32[i], y.r[3].m128_f32[i], ratio);
	*/

	return toRet;
}

DirectX::XMVECTOR RPYFromVector(DirectX::XMVECTOR RPY)
{
	DirectX::XMFLOAT3 angles = DirectX::XMFLOAT3((RPY.m128_f32[0] * 180.0f) + 180.0f, 0.0f, (RPY.m128_f32[2] * 180.0f) + 180.0f);
	DirectX::XMVECTOR toRet = DirectX::XMLoadFloat3(&angles);
	return toRet;
}

DirectX::XMMATRIX TurnTo(DirectX::XMMATRIX _mat, DirectX::XMVECTOR _target, float speed)
{
	DirectX::XMMATRIX toRet = _mat;

	DirectX::XMFLOAT4 _pos;
	DirectX::XMStoreFloat4(&_pos, _mat.r[3]);

	DirectX::XMFLOAT3 pos = { _pos.x, _pos.y, _pos.z };

	DirectX::XMFLOAT3 targVec = DirectX::XMFLOAT3(_target.m128_f32[0] - pos.x, _target.m128_f32[1] - pos.y, _target.m128_f32[2] - pos.z);
	DirectX::XMFLOAT3 _t = { targVec.x, targVec.y, targVec.z };

	DirectX::XMVECTOR
		xAxis = DirectX::XMVector3Normalize(_mat.r[0]),
		yAxis = DirectX::XMVector3Normalize(_mat.r[1]),
		zAxis = DirectX::XMVector3Normalize(_mat.r[2]),
		_targ = DirectX::XMLoadFloat3(&_t);

	float yRotation = DirectX::XMVectorGetX(DirectX::XMVector3Dot(xAxis, _targ));
	float xRotation = DirectX::XMVectorGetX(DirectX::XMVector3Dot(yAxis, _targ));

	DirectX::XMVECTOR thisPos = toRet.r[3];
	toRet.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	toRet = toRet * DirectX::XMMatrixRotationY(yRotation * speed);
	toRet = DirectX::XMMatrixRotationX(xRotation * -speed) * toRet;

	toRet.r[3] = thisPos;
	return toRet;
}

void CleanGlobals()
{
	//for (int i = 0; i < v_tMeshTemplates.size(); i++)
	//{
	//	if(v_tMeshTemplates.at(i)._vertexBuffer)
	//	SAFE_RELEASE(v_tMeshTemplates.at(i)._vertexBuffer);
	//	SAFE_RELEASE(v_tMeshTemplates[i]._indexBuffer);
	//	SAFE_RELEASE(v_tMeshTemplates[i]._samState);
	//	for (int j = 0; j < 3; j++)
	//	{
	//		SAFE_RELEASE(v_tMeshTemplates[i]._textures[j]);
	//		SAFE_RELEASE(v_tMeshTemplates[i]._srv[j]);
	//	}
	//
	//}
	v_tMeshTemplates.clear();
	g_d3dData->Cleanup();
	delete g_d3dData;
	g_d3dData = nullptr;

	unsigned int windowRefCount;
	while (true)
	{
		g_pWindow->GetCount(windowRefCount);
		if (windowRefCount >= 1)
		{
			g_pWindow->DecrementCount();
			if (windowRefCount == 1)
				break;
		}
	}

	GW_SAFE_RELEASE(g_pControllerInput);
	GW_SAFE_RELEASE(g_pMusicStream);
	GW_SAFE_RELEASE(g_pSoundPlayer);
	GW_SAFE_RELEASE(g_pInputRecord);
	GW_SAFE_RELEASE(g_pAudioHolder);

	GW_SAFE_RELEASE(g_pLogger);
}

void LoadAnimations()
{
	for (int i = 0; i < ChickenAnimFiles.size(); ++i)
	{
		AnimationClip temp;
		temp = ProcessAnimationFile(ChickenAnimFiles[i].file);
		temp.AnimationName = ChickenAnimFiles[i].animationName;
		temp.loops = ChickenAnimFiles[i].loopable;
		v_tMeshTemplates[MODELS::CHICKEN]._animations.push_back(temp);
	}
	for (int i = 0; i < GoatAnimFiles.size(); ++i)
	{
		AnimationClip temp;
		temp = ProcessAnimationFile(GoatAnimFiles[i].file);
		temp.AnimationName = GoatAnimFiles[i].animationName;
		temp.loops = GoatAnimFiles[i].loopable;
		v_tMeshTemplates[MODELS::GOAT]._animations.push_back(temp);
	}
	for (int i = 0; i < BoarAnimFiles.size(); ++i)
	{
		AnimationClip temp;
		temp = ProcessAnimationFile(BoarAnimFiles[i].file);
		temp.AnimationName = BoarAnimFiles[i].animationName;
		temp.loops = BoarAnimFiles[i].loopable;
		v_tMeshTemplates[MODELS::BOAR]._animations.push_back(temp);
	}
	for (int i = 0; i < RabbitAnimFiles.size(); ++i)
	{
		AnimationClip temp;
		temp = ProcessAnimationFile(RabbitAnimFiles[i].file);
		temp.AnimationName = RabbitAnimFiles[i].animationName;
		temp.loops = RabbitAnimFiles[i].loopable;
		v_tMeshTemplates[MODELS::RABBIT]._animations.push_back(temp);
	}
}

AnimationClip ProcessAnimationFile(const char * animFileName)
{
	AnimationClip temp;

	std::fstream inAnimFile(animFileName, std::ios_base::in | std::ios_base::binary);

	assert(inAnimFile.is_open());

	unsigned int numBindJoints;
	inAnimFile.read((char*)&numBindJoints, sizeof(unsigned int));
	temp._bindPose.resize(numBindJoints);

	for (int j = 0; j < numBindJoints; ++j)
	{
		inAnimFile.read((char*)&temp._bindPose[j]._mat, sizeof(float) * 16);
		inAnimFile.read((char*)&temp._bindPose[j].parentIndex, sizeof(int));
	}
	double _dur;
	inAnimFile.read((char*)&_dur, sizeof(double));
	temp.duration = _dur;
	unsigned int numFrames;
	inAnimFile.read((char*)&numFrames, sizeof(unsigned int));

	temp.frames.resize(numFrames);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		KeyFrame _key;
		inAnimFile.read((char*)&_key.time, sizeof(double));
		unsigned int numJoints;
		inAnimFile.read((char*)&numJoints, sizeof(unsigned int));
		_key.joints.resize(numJoints);

		for (int j = 0; j < numJoints; ++j)
		{
			inAnimFile.read((char*)&_key.joints[j].parentIndex, sizeof(int));

			DirectX::XMFLOAT4X4 _j;
			inAnimFile.read((char*)_j.m, sizeof(float) * 16);
			_key.joints[j]._mat = _j;
		}
		temp.frames[i] = _key;
	}
	inAnimFile.close();

	return temp;
}


