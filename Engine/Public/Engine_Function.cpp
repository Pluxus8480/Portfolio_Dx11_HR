#include "Engine_Defines.h"

Engine::_float4x4 Engine::Matrix_Inverse(const Engine::_float4x4& Matrix)
{
	auto LoadedMatrix = XMLoadFloat4x4(&Matrix);
	LoadedMatrix = XMMatrixInverse(nullptr, LoadedMatrix);
	Engine::_float4x4 MatRet;
	XMStoreFloat4x4(&MatRet, LoadedMatrix);
	return MatRet;
}

_float3 Engine::Get_Cursor_Pos(HWND hWnd)
{
	POINT pPt = {};
	GetCursorPos(&pPt);
	ScreenToClient(hWnd, &pPt);
	return _float3{ (float)pPt.x,(float)pPt.y, 0.f };

}

void Engine::XMFloat4x4ToFloatArray(const XMFLOAT4X4& mat, float out[16])
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			out[i * 4 + j] = mat.m[i][j];
		}
	}
}

void Engine::FloatArrayToXMFloat4x4(const float in[16], XMFLOAT4X4& mat)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			mat.m[i][j] = in[i * 4 + j];
		}
	}
}

_float Engine::fRand()
{
	return rand() % 1000 * 0.001f;
}

_float Engine::fRand(_float min, _float max)
{
	return min + fRand() * (max - min);
}
