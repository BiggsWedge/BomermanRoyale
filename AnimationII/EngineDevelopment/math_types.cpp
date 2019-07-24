#include "math_types.h"

namespace end
{
	float2& float2::operator-(float2 _other)
	{
		float2 temp;
		temp = { this->x - _other.x, this->y - _other.y };
		return temp;
	}

	float2& float2::operator=(float2 _other)
	{
		if (this != &_other)
		{
			this->x = _other.x;
			this->y = _other.y;
		}
		return *this;
	}

	float distance(float3 _a, float3 _b)
	{
		float result = 0;
		result += pow(_b.x - _a.x, 2);
		result += pow(_b.y - _a.y, 2);
		result += pow(_b.z - _a.z, 2);
		result = sqrt(result);
		return result;
	}

	float3 normalize(float3 _toNorm)
	{
		float det = sqrt(pow(_toNorm.x, 2) + pow(_toNorm.y, 2) + pow(_toNorm.z, 2));
		float3 result = { _toNorm.x / det, _toNorm.y / det, _toNorm.z / det };
		return result;
	}

	float3& float3::operator*(float _val)
	{
		float3 temp;
		temp.x = this->x * _val;
		temp.y = this->y * _val;
		temp.z = this->z * _val;
		return temp;
	}

	float3& float3::operator+(float3 _other)
	{
		float3 toRet;
		toRet.x = this->x + _other.x;
		toRet.y = this->y + _other.y;
		toRet.z = this->z + _other.z;
		return toRet;
	}

	float3& float3::operator=(float3 _other)
	{
		if (this != &_other)
		{
			this->x = _other.x;
			this->y = _other.y;
			this->z = _other.z;
		}
		return *this;
	}

	float3& float3::operator-(float3 _other)
	{
		float3 temp;
		temp.x = this->x - _other.x;
		temp.y = this->y - _other.y;
		temp.z = this->z - _other.z;
		return temp;
	}

	float4& float4::operator=(float4 _other)
	{
		if (this != &_other)
		{
			this->x = _other.x;
			this->y = _other.y;
			this->z = _other.z;
			this->w = _other.w;
		}
		return *this;
	}

	float3 positionLerp(float3 _a, float3 _b, float ratio)
	{
		float3 _temp;
		_temp =
		{
			(_b.x - _a.x) * ratio + _a.x,
			(_b.y - _a.y) * ratio + _a.y,
			(_b.z - _a.z) * ratio + _a.z
		};
		return _temp;
	}

	DirectX::XMMATRIX LookAt(float3 _pos, float3 _target, float3 _localUp)
	{
		float3 z = _target - _pos;
		DirectX::XMVECTOR zAxis = { z.x, z.y, z.z };
		DirectX::XMVECTOR up = { _localUp.x, _localUp.y, _localUp.z };
		DirectX::XMVECTOR xAxis = DirectX::XMVector3Cross(up, zAxis);
		DirectX::XMVECTOR yAxis = DirectX::XMVector3Cross(zAxis, xAxis);

		DirectX::XMMATRIX toReturn;
		DirectX::XMFLOAT4X4 _toRet;

		DirectX::XMFLOAT3 _x, _y, _z;

		DirectX::XMStoreFloat3(&_x, DirectX::XMVector3Normalize(xAxis));
		DirectX::XMStoreFloat3(&_y, DirectX::XMVector3Normalize(yAxis));
		DirectX::XMStoreFloat3(&_z, DirectX::XMVector3Normalize(zAxis));

		_toRet._11 = _x.x;
		_toRet._12 = _x.y;
		_toRet._13 = _x.z;
		_toRet._14 = 0.0f;
		_toRet._21 = _y.x;
		_toRet._22 = _y.y;
		_toRet._23 = _y.z;
		_toRet._24 = 0.0f;
		_toRet._31 = _z.x;
		_toRet._32 = _z.y;
		_toRet._33 = _z.z;
		_toRet._34 = 0.0f;
		_toRet._41 = _pos.x;
		_toRet._42 = _pos.y;
		_toRet._43 = _pos.z;
		_toRet._44 = 1.0f;

		toReturn = DirectX::XMLoadFloat4x4(&_toRet);
		return toReturn;
	}

	DirectX::XMMATRIX TurnTo(DirectX::XMMATRIX _mat, float3 _target, float speed)
	{
		DirectX::XMMATRIX toRet = _mat;

		DirectX::XMFLOAT4 _pos;
		DirectX::XMStoreFloat4(&_pos, _mat.r[3]);

		float3 pos = { _pos.x, _pos.y, _pos.z };

		float3 targVec = _target - pos;
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

	DirectX::XMMATRIX MouseLook(DirectX::XMMATRIX camMat, float2 angles)
	{
		DirectX::XMFLOAT4 camPos;
		DirectX::XMStoreFloat4(&camPos, camMat.r[3]);

		DirectX::XMMATRIX toRet = camMat;

		toRet.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		toRet = DirectX::XMMatrixRotationX(angles.x) * toRet;
		toRet = toRet * DirectX::XMMatrixRotationY(angles.y);

		toRet.r[3] = DirectX::XMLoadFloat4(&camPos);

		return toRet;
	}




	void SimpleModel::initialize(ID3D11Device* _device)
	{
		D3D11_BUFFER_DESC vertBufferDesc;
		D3D11_SUBRESOURCE_DATA vertBuffSubResc;

		ZeroMemory(&vertBufferDesc, sizeof(vertBufferDesc));
		ZeroMemory(&vertBuffSubResc, sizeof(vertBuffSubResc));

		vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertBufferDesc.ByteWidth = sizeof(Simple_Vertex) * _vertices.size();
		vertBufferDesc.CPUAccessFlags = 0;
		vertBufferDesc.MiscFlags = 0;
		vertBufferDesc.StructureByteStride = 0;
		vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		vertBuffSubResc.pSysMem = _vertices.data();

		_device->CreateBuffer(&vertBufferDesc, &vertBuffSubResc, &_vertexBuffer);

		D3D11_BUFFER_DESC indexBufferDesc;
		D3D11_SUBRESOURCE_DATA indexBufferSubResourceData;

		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		ZeroMemory(&indexBufferSubResourceData, sizeof(indexBufferSubResourceData));

		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * _indices.size();
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		indexBufferSubResourceData.pSysMem = _indices.data();

		_device->CreateBuffer(&indexBufferDesc, &indexBufferSubResourceData, &_indexBuffer);


		for (int i = 0; i < mats.size(); ++i)
		{
			for (int j = 0; j < material_t::COMPONENT::COUNT; ++j)
			{
				if (mats[i][j].input < 0)
					continue;
				std::experimental::filesystem::path filePath;
				filePath = filePaths[mats[i][j].input].data();
				HRESULT result = DirectX::CreateWICTextureFromFile(_device, filePath.wstring().c_str(), nullptr, &_srv[j]);
				if (!SUCCEEDED(result))
					std::cout << "Failed to make texture!" << std::endl;
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
		_device->CreateSamplerState(&sampDesc, &_samState);

	}


	void SimpleModel::loadModel(const char* modelFile, const char* matFile, const char* animFile)
	{
		std::fstream file{ modelFile, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		file.read((char*)&numIndices, sizeof(int));
		_indices.resize(numIndices);
		file.read((char*)_indices.data(), sizeof(int) * numIndices);

		file.read((char*)&numVerts, sizeof(int));
		_vertices.resize(numVerts);
		file.read((char*)_vertices.data(), sizeof(Simple_Vertex) * numVerts);

		file.close();


		for (auto& v : _vertices)
		{
			v.Position.x = v.Position.x;
			v.Normal.x = -v.Normal.x;
			v.Texture.y = 1.0f - v.Texture.y;
		}

		std::fstream inMatFile{ matFile, std::ios_base::in | std::ios_base::binary };

		assert(inMatFile.is_open());

		if (!inMatFile.is_open())
		{
			assert(false);
			return;
		}

		int numMatFiles;
		inMatFile.read((char*)&numMatFiles, sizeof(int));


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
					memcpy(_mat.surfaceDiffuse, temp[j].value, sizeof(float) * 3);
					_mat.diffuseFactor = temp[j].factor;
				}
				if (j == material_t::COMPONENT::EMISSIVE)
				{
					memcpy(_mat.surfaceEmissive, temp[j].value, sizeof(float) * 3);
					_mat.emissiveFactor = temp[j].factor;
				}
				if (j == material_t::COMPONENT::SPECULAR)
				{
					memcpy(_mat.surfaceSpecular, temp[j].value, sizeof(float) * 3);
					_mat.specularFactor = temp[j].factor;
				}
				if (j == material_t::COMPONENT::SHININESS)
				{
					memcpy(_mat.surfaceShiny, temp[j].value, sizeof(float) * 3);
					_mat.shininess = temp[j].factor;
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
			filePaths.push_back(_file);
		}
		inMatFile.close();


		if (animFile)
		{
			std::fstream inAnimFile(animFile, std::ios_base::in | std::ios_base::binary);

			assert(inAnimFile.is_open());

			int num;
			inAnimFile.read((char*)&num, sizeof(int));
			_bindPose.resize(num);
			for (int i = 0; i < num; ++i)
			{
				inAnimFile.read((char*)&_bindPose[i].parentIndex, sizeof(int));
				DirectX::XMFLOAT4X4 mat;
				inAnimFile.read((char*)mat.m, sizeof(float) * 16);
				_bindPose[i]._mat = DirectX::XMLoadFloat4x4(&mat);
			}




			double _dur;
			inAnimFile.read((char*)&_dur, sizeof(double));
			_anim.duration = _dur;
			int numFrames;
			inAnimFile.read((char*)&numFrames, sizeof(int));

			_anim.frames.resize(numFrames);
			for (int i = 0; i < numFrames; ++i)
			{
				KeyFrame _key;
				inAnimFile.read((char*)&_key.time, sizeof(double));
				int numJoints;
				inAnimFile.read((char*)&numJoints, sizeof(int));
				_key.joints.resize(numJoints);

				for (int j = 0; j < numJoints; ++j)
				{
					inAnimFile.read((char*)&_key.joints[j].parentIndex, sizeof(int));
					DirectX::XMFLOAT4X4 _j;
					inAnimFile.read((char*)_j.m, sizeof(float) * 16);
					_key.joints[j]._mat = DirectX::XMLoadFloat4x4(&_j);
				}
				_anim.frames[i] = _key;
			}
			inAnimFile.close();
			_anim.frames.push_back(_anim.frames[0]);
			_anim.frames[_anim.frames.size() - 1].time = _anim.duration;
		}
	}

	void SimpleModel::render(ID3D11DeviceContext* _context)
	{
		UINT strides = sizeof(Simple_Vertex);
		UINT offsets = 0;

		_context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &strides, &offsets);

		_context->PSSetSamplers(0, 1, &_samState);
		_context->PSSetShaderResources(0, 1, &_srv[DIFFUSE]);
		_context->PSSetShaderResources(1, 1, &_srv[EMISSIVE]);
		_context->PSSetShaderResources(2, 1, &_srv[SPECULAR]);

		_context->DrawIndexed(numIndices, 0, 0);
	}


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

}