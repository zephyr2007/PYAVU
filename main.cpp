#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include "class.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

HWND hEditInput = nullptr;   // поле ввода
HWND hEditOutput = nullptr;  // поле вывода

static void simulate_command(const std::string& line, BankSystem& bank, Account*& current) {
  std::istringstream ss(line);
  std::string cmd;
  ss >> cmd;

  std::ostringstream out;
  std::streambuf* oldCout = std::cout.rdbuf(out.rdbuf());

  // Вставь сюда почти весь логический код из твоего main.cpp
  if (cmd == "exit") {
    PostMessage(GetActiveWindow(), WM_CLOSE, 0, 0);
  }
  else if (cmd == "register") {
    std::string login, pass;
    ss >> login >> pass;
    if (bank.registerAccount(login, pass)) {
      out << "Аккаунт создан\n";
    }
    else {
      out << "Ошибка регистрации\n";
    }
  }
  else if (cmd == "login") {
    std::string login, pass;
    ss >> login >> pass;
    if (bank.login(login, pass, current)) {
      out << "Вход выполнен\n";
    }
    else {
      out << "Ошибка входа\n";
    }
  }
  else {
    out << "Неизвестная команда\n";
  }

  std::cout.rdbuf(oldCout);

  // Выводим результат в поле вывода
  SendMessage(hEditOutput, EM_SETSEL, -1, -1); // конец текста
  SendMessage(hEditOutput, EM_REPLACESEL, 0, (LPARAM)out.str().c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_CREATE: {
    hEditInput = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER
      | ES_LEFT, 10, 10, 600, 25, hWnd, 0, 0, 0);

    hEditOutput = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER
      | ES_LEFT | ES_MULTILINE | ES_READONLY | WS_VSCROLL, 10, 50, 600, 300, hWnd, 0, 0, 0);

    CreateWindowExA(0, "BUTTON", "Выполнить", WS_CHILD | WS_VISIBLE
      | WS_BORDER, 620, 10, 100, 25, hWnd, (HMENU)1, 0, 0);
    break;
  }
  case WM_COMMAND:
    if (LOWORD(wParam) == 1) {
      // кнопка "Выполнить"
      char buf[1024];
      GetWindowTextA(hEditInput, buf, 1024);
      std::string cmd = buf;

      static BankSystem bank;
      static Account* current = nullptr;
      simulate_command(cmd, bank, current);

      SetWindowTextA(hEditInput, ""); // очистить поле ввода
    }
    break;
  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
  LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASSEX wc = { 0 };
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "BankApp";
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

  RegisterClassEx(&wc);

  HWND hWnd = CreateWindowExA(0, "BankApp", "Банковское приложение",
    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 750, 400, NULL, NULL, hInstance, NULL);

  ShowWindow(hWnd, nCmdShow);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}