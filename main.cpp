#include "webui.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <windows.h>

// 窗口信息结构体
struct WindowInfo {
  HWND hwnd;
  std::wstring title;
  std::wstring className;
  bool isVisible;
  bool isMinimized;
  bool isMaximized;
  DWORD processId;
  std::wstring processName;
};

// 获取进程名称
std::wstring GetProcessName(DWORD processId) {
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE, processId);
  if (hProcess == NULL) {
    return L"Unknown";
  }

  wchar_t processName[MAX_PATH];
  DWORD size = MAX_PATH;

  if (QueryFullProcessImageNameW(hProcess, 0, processName, &size)) {
    std::wstring fullPath(processName);
    size_t pos = fullPath.find_last_of(L'\\');
    if (pos != std::wstring::npos) {
      CloseHandle(hProcess);
      return fullPath.substr(pos + 1);
    }
  }

  CloseHandle(hProcess);
  return L"Unknown";
}

// 检查窗口是否应该在任务栏显示
bool ShouldShowInTaskbar(HWND hwnd) {
  // 检查窗口是否可见
  if (!IsWindowVisible(hwnd)) {
    return false;
  }

  // 获取窗口样式
  LONG style = GetWindowLong(hwnd, GWL_STYLE);
  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

  // 排除工具窗口（除非有 WS_EX_APPWINDOW 样式）
  if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) {
    return false;
  }

  // 检查是否有父窗口（排除子窗口）
  HWND parent = GetParent(hwnd);
  if (parent != NULL && parent != GetDesktopWindow()) {
    return false;
  }

  // 检查窗口是否有标题
  int titleLength = GetWindowTextLengthW(hwnd);

  // 如果有 WS_EX_APPWINDOW 样式，即使没有标题也显示
  if (exStyle & WS_EX_APPWINDOW) {
    return true;
  }

  // 必须有标题才显示
  if (titleLength == 0) {
    return false;
  }

  // 排除一些特殊的窗口类
  wchar_t className[256];
  GetClassNameW(hwnd, className, sizeof(className) / sizeof(wchar_t));
  std::wstring classNameStr(className);

  // 排除一些系统窗口类
  if (classNameStr == L"Windows.UI.Core.CoreWindow" ||
      classNameStr == L"ApplicationFrameWindow") {
    return false;
  }

  return true;
}

// 窗口枚举回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
  std::vector<WindowInfo> *windows =
      reinterpret_cast<std::vector<WindowInfo> *>(lParam);

  // 检查是否应该在任务栏显示
  if (!ShouldShowInTaskbar(hwnd)) {
    return TRUE; // 继续枚举
  }

  WindowInfo info;
  info.hwnd = hwnd;

  // 获取窗口标题
  int titleLength = GetWindowTextLengthW(hwnd);
  if (titleLength > 0) {
    std::vector<wchar_t> titleBuffer(titleLength + 1);
    GetWindowTextW(hwnd, titleBuffer.data(), titleLength + 1);
    info.title = std::wstring(titleBuffer.data());
  } else {
    info.title = L"(无标题)";
  }

  // 获取窗口类名
  wchar_t className[256];
  if (GetClassNameW(hwnd, className, sizeof(className) / sizeof(wchar_t))) {
    info.className = std::wstring(className);
  }

  // 获取窗口状态
  info.isVisible = IsWindowVisible(hwnd);
  info.isMinimized = IsIconic(hwnd);
  info.isMaximized = IsZoomed(hwnd);

  // 获取进程ID和进程名
  GetWindowThreadProcessId(hwnd, &info.processId);
  info.processName = GetProcessName(info.processId);

  windows->push_back(info);

  return TRUE; // 继续枚举
}

// 将宽字符串转换为多字节字符串（用于控制台输出）
std::string WStringToString(const std::wstring &wstr) {
  if (wstr.empty())
    return std::string();

  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
                                        NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0],
                      size_needed, NULL, NULL);
  return strTo;
}

std::string HwndToHexString(HWND hwnd) {
  std::ostringstream oss;
  oss << "0x" << std::hex << std::uppercase
      << reinterpret_cast<uintptr_t>(hwnd);
  return oss.str();
}

HWND HexStringToHwnd(const std::string &hexStr) {
  uintptr_t handle;
  std::istringstream iss(hexStr);
  iss >> std::hex >> handle;
  return reinterpret_cast<HWND>(handle);
}

void get_windows(webui::window::event *e) {
  std::vector<WindowInfo> windows;
  if (!EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows))) {
    std::cerr << "枚举窗口失败！\n";
  }

  using json = nlohmann::json;
  json result;
  result["windows"] = json::array();
  for (const auto &info : windows) {
    json windowJson;

    // 将宽字符串转换为UTF-8字符串
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, info.title.c_str(), -1,
                                          NULL, 0, NULL, NULL);
    if (size_needed > 0) {
      std::string titleUtf8(size_needed - 1, 0);
      WideCharToMultiByte(CP_UTF8, 0, info.title.c_str(), -1, &titleUtf8[0],
                          size_needed, NULL, NULL);
      windowJson["title"] = titleUtf8;
    } else {
      windowJson["title"] = "(无标题)";
    }
    windowJson["handle"] = HwndToHexString(info.hwnd);
    result["windows"].push_back(windowJson);
  }
  e->return_string(result.dump(2));
}

void activate_window(webui::window::event *e) {
  std::string handle = e->get_string();
  HWND hwnd = HexStringToHwnd(handle);
  if (IsIconic(hwnd)) {
    ShowWindow(hwnd, SW_RESTORE);
  }
  keybd_event(VK_MENU, 0, 0, 0);
  keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

  SetForegroundWindow(hwnd);
}

int main() {
  webui::window my_window;
  my_window.set_size(300, 400);

  my_window.bind("get_windows", get_windows);
  my_window.bind("activate_window", activate_window);
  my_window.show("index.html");
  webui::wait();

  return 0;
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline,
                     int cmdshow) {
  main();
}
#endif
