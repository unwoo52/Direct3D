#pragma once
#include "../DebugLog.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
using namespace std;
using namespace Microsoft::WRL;

class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC description;
};

class AdapterReader
{
	static vector<AdapterData> adapters;
public:
	static vector<AdapterData> GetAdapters();
};

