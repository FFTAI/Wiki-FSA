#ifndef UTILS_COMMON_WIN_H
#define UTILS_COMMON_WIN_H

#include <Shlobj.h>
#include <Windows.h>
#include <shlwapi.h>

#include <regex>
#include <string>

namespace utils {
std::wstring GetCacheDirectory() {
  wchar_t cache[MAX_PATH] = {0};
  SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, cache);
  return cache;
}

std::wstring GetAllUsersCacheDirectory() {
  wchar_t cache[MAX_PATH] = {0};
  SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, cache);
  return cache;
}

std::string GetCmdOutput(const std::string &cmd) {
  SECURITY_ATTRIBUTES sa;
  HANDLE hRead, hWrite;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;

  if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
    return std::string();
  }

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  GetStartupInfoA(&si);
  si.hStdError = hWrite;
  si.hStdOutput = hWrite;
  si.wShowWindow = SW_HIDE;
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

  std::string command = "cmd.exe /C " + cmd;
  char *path = new char[command.size() + 1];
  ZeroMemory(path, (command.size() + 1) * sizeof(char));
  memcpy_s(path, command.size(), command.c_str(), command.size());

  if (!CreateProcessA(NULL, path, NULL, NULL, TRUE, NULL, NULL, NULL, &si,
                      &pi)) {
    delete[] path;
    return std::string();
  }
  CloseHandle(hWrite);

  char buffer[4096] = {0};
  std::string strOutput;
  DWORD bytesRead;

  while (true) {
    memset(buffer, 0, 4096);
    if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL) break;
    strOutput += buffer;
  }
  CloseHandle(hRead);
  delete[] path;

  return strOutput;
}

namespace file {
long long GetCurrentTimestamp() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}
long long GetCurrentTimestampInSeconds() {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

std::wstring GetFileName(const std::wstring &filePath) {
  if (filePath.size() == 0) {
    return std::wstring();
  }
  auto pos = filePath.rfind('\\');
  return filePath.substr(pos + 1, filePath.length() - pos - 1);
}

std::wstring GetExecName() {
  wchar_t cache[MAX_PATH] = {0};
  GetModuleFileName(NULL, cache, MAX_PATH);
  std::wstring exe_name = GetFileName(cache);
  return exe_name.substr(0, exe_name.length() - 4);
}

std::wstring GetExecDirectory() {
  wchar_t cache[MAX_PATH] = {0};
  GetModuleFileName(NULL, cache, MAX_PATH);
  std::wstring path = cache;
  return path.substr(0, path.find_last_of('\\'));
}

bool GetLinkTarget(const wchar_t *lpszLinkPath, wchar_t *lpszTargetPath,
                   unsigned long dwTargetPathLen, wchar_t *lpszCmdline,
                   unsigned long dwCmdlineLen, bool bInitCom) {
  bool bRet = false;
  if (bInitCom) CoInitialize(nullptr);
  IShellLink *psl;
  if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                 IID_IShellLink, (LPVOID *)&psl))) {
    IPersistFile *ppf;
    if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf))) {
      if (SUCCEEDED(ppf->Load(lpszLinkPath, STGM_READ))) {
        if (SUCCEEDED(psl->Resolve(NULL, SLR_NO_UI))) {
          psl->GetArguments(lpszCmdline, dwCmdlineLen);
          if (SUCCEEDED(psl->GetPath(lpszTargetPath, dwTargetPathLen, NULL,
                                     SLGP_RAWPATH)))
            bRet = true;
        }
      }
      ppf->Release();
    }
    psl->Release();
  }
  if (bInitCom) CoUninitialize();
  return bRet;
}

bool PathExists(const std::wstring &path) {
  return (GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES);
}

bool DirectoryExists(const std::wstring &path) {
  DWORD fileattr = GetFileAttributes(path.c_str());
  if (fileattr != INVALID_FILE_ATTRIBUTES)
    return (fileattr & FILE_ATTRIBUTE_DIRECTORY) != 0;
  return false;
}

bool CreateDirectories(const wchar_t *directories) {
  SECURITY_ATTRIBUTES sa;
  SECURITY_DESCRIPTOR sd;
  InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
  // SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
  SetSecurityDescriptorDacl(
      &sd, FALSE, NULL, TRUE);  //详见MSDN上SetSecurityDescriptorDacl的使用说明
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = FALSE;
  sa.lpSecurityDescriptor = &sd;

  SHCreateDirectoryEx(NULL, directories, &sa);

  return DirectoryExists(directories);
}

bool RemoveDirTree(const std::wstring &dir, bool delete_root = true) {
  std::wstring findFile = dir + L"\\" + L"*.*";
  WIN32_FIND_DATA data;
  ZeroMemory(&data, sizeof(WIN32_FIND_DATA));
  HANDLE hFind = ::FindFirstFile(findFile.c_str(), &data);
  if (hFind == INVALID_HANDLE_VALUE) return true;

  bool ret = true;
  while (::FindNextFile(hFind, &data) != 0) {
    std::wstring fileName = data.cFileName;
    if (fileName == L"." || fileName == L"..") continue;

    std::wstring fullPath = dir + L"\\" + fileName;
    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (!RemoveDirTree(fullPath)) {
        ret = false;
        break;
      }
    } else {
      ::DeleteFile(fullPath.c_str());
    }
  }
  if (ret && delete_root) {
    ::RemoveDirectory(dir.c_str());
  }
  ::FindClose(hFind);

  return ret;
}

void SplitString(const std::string data, std::vector<std::string> *vec_data,
                 std::string re_str = "\\s+") {
  std::regex re(re_str);
  std::vector<std::string> v(
      std::sregex_token_iterator(data.begin(), data.end(), re, -1),
      std::sregex_token_iterator());
}
}  // namespace file

}  // namespace utils
#endif