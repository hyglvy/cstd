#include <windows.h>
const char* AppName = "AppName";
const char* AppName2 = "AppName2";
void Modal(HWND last)
{
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  EnableWindow(last, TRUE);
  SetForegroundWindow(last);
}
LRESULT CALLBACK test_Modal_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_LBUTTONDOWN: {
    HWND hwnd2 = NULL;
    int id;
    int ent;
    ent = HIWORD(wParam);//֪ͨ��
    id = LOWORD(wParam);//�Ӵ���ID
    //�жϰ�ť�ĵ����¼�
    EnableWindow(hwnd, FALSE);
    hwnd2 = CreateWindowEx(NULL, AppName2, TEXT("���ڱ���"), WS_OVERLAPPEDWINDOW, 150, 150, 500, 500, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 0);
    if (hwnd2 == NULL) {
      MessageBox(NULL, TEXT("��������ʧ��!"), TEXT("����"), MB_ICONERROR);
      return 0;
    }
    //SetWindowLong(hwnd2, GWL_WNDPROC, (LONG)test_Modal_WndProc2);
    ShowWindow(hwnd2, 5);
    UpdateWindow(hwnd2);
    Modal(hwnd);
  }
  break;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK test_Modal_WndProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int test_Modal()
{
  HINSTANCE hInstance = GetModuleHandle(0);
  WNDCLASSA wc;
  //InitCommonControls();
  //�������ڹ���������ṹ
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = test_Modal_WndProc;//���ڻص�����ָ��
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;//ʵ�����
  wc.hIcon = LoadIcon(hInstance, TEXT("ICON_1"));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);//Ĭ��ָ��
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);//Ĭ�ϱ�����ɫ
  wc.lpszMenuName = NULL;
  wc.lpszClassName = AppName;//��������
  //ע�ᴰ����
  if (!RegisterClass(&wc)) {
    MessageBox(NULL, TEXT("ע�ᴰ����ʧ��!"), TEXT("����"), MB_ICONERROR);
    return 0;
  }
  //ע��ڶ������ڵĴ�����
  wc.lpfnWndProc = test_Modal_WndProc2;
  wc.lpszClassName = AppName2;
  if (!RegisterClass(&wc)) {
    MessageBox(NULL, TEXT("ע�ᴰ����ʧ��!"), TEXT("����"), MB_ICONERROR);
    return 0;
  }
  //��������
  HWND hwnd;
  hwnd = CreateWindowEx(0, AppName, //����������
      TEXT("The Hello Program"), //���ڱ���
      WS_OVERLAPPEDWINDOW | WS_VSCROLL, //���ڷ��
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance, //ʵ�����
      NULL);
  ShowWindow(hwnd, 1) ;
  UpdateWindow(hwnd) ;
  //��Ϣѭ��
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) { //����Ϣ������ȡ��Ϣ
    TranslateMessage(&msg) ; //ת����Ϣ
    DispatchMessage(&msg) ; //�ɷ���Ϣ
  }
  return 0;
}

