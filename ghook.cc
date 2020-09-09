#include <assert.h>
#include <node_api.h>

#include <stdio.h>
#define _WIN32_WINNT 0X0500
#include <windows.h>

HHOOK g_hlowKeyHook = NULL;
HHOOK g_hMouse = NULL;
HWND g_HWnd = NULL;

LRESULT CALLBACK keyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lparam;
	//如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
	//if (nCode == HC_ACTION){
	if (pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000){
		return 1;
	}
	else if (pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000){
		return 1;
	}
	else if (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN){
		return 1;
	}
	else if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags &LLKHF_ALTDOWN){
		return 1;
	}
	else if (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN){
		return 1;
	}
	else if (pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN){
		return 1;
	}
	else if (pkbhs->vkCode == VK_SPACE)
	{
		MessageBox(NULL, "xxx", "xxxx", 0);
		return 1;
	}
	return CallNextHookEx(g_hlowKeyHook, nCode, wparam, lparam);
}

napi_value ClearHook(napi_env env, napi_callback_info info)
{
	UnhookWindowsHookEx(g_hlowKeyHook);
	return napi_value();
}

napi_value SetHook(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);


  napi_valuetype valuetype0;
  status = napi_typeof(env, args[0], &valuetype0);
  assert(status == napi_ok);

  void* data = nullptr;
  size_t length = 0;
	
  status = napi_get_buffer_info(env, args[0], &data,&length);
  assert(status == napi_ok);
  g_HWnd = reinterpret_cast<HWND>(data);
  g_hlowKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, GetModuleHandle(nullptr), 0);
  return args[1];
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

  napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc = DECLARE_NAPI_METHOD("SetHook", SetHook);
  status = napi_define_properties(env, exports, 1, &desc);

  napi_property_descriptor clear = DECLARE_NAPI_METHOD("ClearHook", ClearHook);
  status = napi_define_properties(env, exports, 2, &clear);
  
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
