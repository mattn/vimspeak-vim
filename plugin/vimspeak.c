#include <windows.h>
#include <objbase.h>
#include <ctype.h>
#include <stdlib.h>
#include <memory.h>

BSTR
utf8_to_bstr(const char* str) {
  DWORD size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
  if (size == 0) return NULL;
  BSTR bs = SysAllocStringLen(NULL, size - 1);
  MultiByteToWideChar(CP_UTF8, 0, str, -1, bs, size);
  return bs;
}

static IID IID_IAgentEx = {0x48D12BA0,0x5B77,0x11d1,{0x9E,0xC1,0x00,0xC0,0x4F,0xD7,0x08,0x1F}};
static CLSID CLSID_AgentServer = {0xD45FD2FC,0x5C6E,0x11D1,{0x9E,0xC1,0x00,0xC0,0x4F,0xD7,0x08,0x1F}};
static IDispatch* pAgentEx = NULL;
static IDispatch* pCharacterEx = NULL;

LPSTR _declspec (dllexport)
Speak(char* text) {
  HRESULT hr;
  BSTR name;
  static long char_id, request_id;
  static DISPID dispid;
  static VARIANTARG args[3];
  static VARIANT result;
  static DISPPARAMS param;
  param.rgvarg = &args[0];
  int x = -1, y = -1;

  char* ptr = text;
  char* tmp;

  tmp = ptr;
  if (ptr = strchr(ptr, '\t')) {
    *ptr++ = 0;
    x = atoi(tmp);
    text = ptr;
  }
  tmp = ptr;
  if (ptr = strchr(ptr, '\t')) {
    *ptr++ = 0;
    y = atoi(tmp);
    text = ptr;
  }
  if (ptr) {
    text = ptr;
  }
  
  if (!pAgentEx || !pCharacterEx) {
    hr = CoInitialize(NULL);

    hr = CoCreateInstance(
      &CLSID_AgentServer,
      NULL,
      CLSCTX_SERVER,
      &IID_IAgentEx,
      (LPVOID *)&pAgentEx);
  
    // Load
    name = SysAllocString(L"Load");
    hr = pAgentEx->lpVtbl->GetIDsOfNames(pAgentEx, &IID_NULL, &name, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    SysFreeString(name);
  
    VariantInit(&args[2]);
    V_VT(&args[2]) = VT_EMPTY;
    V_BSTR(&args[2]) = SysAllocString(L"merlin.acs"); 
  
    VariantInit(&args[1]);
    V_VT(&args[1]) = VT_I4 | VT_BYREF;
    V_I4REF(&args[1]) = &char_id;
  
    VariantInit(&args[0]);
    V_VT(&args[0]) = VT_I4 | VT_BYREF;
    V_I4REF(&args[0]) = &request_id;
  
    param.cArgs = 3;
    hr = pAgentEx->lpVtbl->Invoke(pAgentEx, dispid, &IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
        &param, &result, NULL, NULL);
  
    SysFreeString(V_BSTR(&args[2]));
  
    // GetCharacterEx
    name = SysAllocString(L"GetCharacterEx");
    hr = pAgentEx->lpVtbl->GetIDsOfNames(pAgentEx, &IID_NULL, &name, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    SysFreeString(name);
  
    VariantInit(&args[1]);
    V_VT(&args[1]) = VT_I4;
    V_I4(&args[1]) = char_id;
  
    VariantInit(&args[0]);
    V_VT(&args[0]) = VT_DISPATCH | VT_BYREF;
    V_DISPATCHREF(&args[0]) = &pCharacterEx;
  
    param.cArgs = 2;
    hr = pAgentEx->lpVtbl->Invoke(pAgentEx, dispid, &IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
        &param, &result, NULL, NULL);

    if (!SUCCEEDED(hr)) return FALSE;
  
    if (x != -1 && y != -1) {
      // SetPosition
      name = SysAllocString(L"SetPosition");
      hr = pCharacterEx->lpVtbl->GetIDsOfNames(pCharacterEx, &IID_NULL, &name, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
      SysFreeString(name);
    
      VariantInit(&args[1]);
      V_VT(&args[1]) = VT_I4;
      V_I4(&args[1]) = x;
    
      VariantInit(&args[0]);
      V_VT(&args[0]) = VT_I4;
      V_I4(&args[0]) = y;
    
      hr = pCharacterEx->lpVtbl->Invoke(pCharacterEx, dispid, &IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
          &param, &result, NULL, NULL);
    }
  } 
  
  // Show
  name = SysAllocString(L"Show");
  hr = pCharacterEx->lpVtbl->GetIDsOfNames(pCharacterEx, &IID_NULL, &name, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
  SysFreeString(name);
  
  VariantInit(&args[1]);
  V_VT(&args[1]) = VT_BOOL;
  V_BOOL(&args[1]) = VARIANT_FALSE;
  
  VariantInit(&args[0]);
  V_VT(&args[0]) = VT_I4 | VT_BYREF;
  V_I4REF(&args[0]) = &request_id;
  
  hr = pCharacterEx->lpVtbl->Invoke(pCharacterEx, dispid, &IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
    &param, &result, NULL, NULL);

  // Speak
  name = SysAllocString(L"Speak");
  hr = pCharacterEx->lpVtbl->GetIDsOfNames(pCharacterEx, &IID_NULL, &name, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
  SysFreeString(name);

  VariantInit(&args[2]);
  V_VT(&args[2]) = VT_BSTR;
  V_BSTR(&args[2]) = utf8_to_bstr(text);
  
  VariantInit(&args[1]);
  V_VT(&args[1]) = VT_BSTR;
  
  VariantInit(&args[0]);
  V_VT(&args[0]) = VT_I4 | VT_BYREF;
  V_I4REF(&args[0]) = &request_id;
  
  param.cArgs = 3;
  hr = pCharacterEx->lpVtbl->Invoke(pCharacterEx, dispid, &IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
    &param, &result, NULL, NULL);

  SysFreeString(V_BSTR(&args[2]));

  return FALSE;
}

// vim:set et sw=2 ts=2 ai:
