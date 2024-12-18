#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
			iRefCnt = pInstance->AddRef();

		return iRefCnt;
	}


	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();
			pInstance = nullptr;
		}

		return iRefCnt;
	}


	template<typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete[] pPointer;
			pPointer = nullptr;
		}
	}

	Engine::_float4x4 Matrix_Inverse(const Engine::_float4x4& Matrix);

	_float3 Get_Cursor_Pos(HWND hWnd);

	void XMFloat4x4ToFloatArray(const XMFLOAT4X4& mat, float out[16]);

	void FloatArrayToXMFloat4x4(const float in[16], XMFLOAT4X4& mat);
	
	template<typename T>
	std::ostream& write_typed_data(std::ostream& stream, const T& value) {
		return stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	template<typename T>
	std::istream& read_typed_data(std::istream& stream, T& value) {
		return stream.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

	template<typename T>
		void MEMORY_COPY(T* dest, const T* src, size_t count = 1) {
			memcpy(dest, src, count * sizeof(T));
	}

	_float fRand();
	_float fRand(_float min, _float max);

}

