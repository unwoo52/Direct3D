#include "AdapterReader.h"

vector<AdapterData> AdapterReader::adapters;

AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;
	HRESULT hr = pAdapter->GetDesc(&description);
	if (FAILED(hr))
	{
		Debug::Log(hr, L"������ ������ �о� ���µ� ���� �Ͽ����ϴ�.");
	}
}

vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adapters.size() > 0)
	{
		return adapters;
	}

	ComPtr<IDXGIFactory> pFactory;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		Debug::Log(hr, L"Failed to create DXGIFactory for enumerating adapters.");
		exit(-1);
	}

	IDXGIAdapter* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters(index++, &pAdapter)))
	{
		adapters.push_back(AdapterData(pAdapter));		
	}

	return adapters;
}
