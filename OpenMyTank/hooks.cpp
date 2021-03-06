
//-----------------------------------------------------------------------------

#include "hooks.h"
#include "chat_writer.h"

//-----------------------------------------------------------------------------

void ToggleMenuVisible();

//-----------------------------------------------------------------------------
//    KeyboardHook
//-----------------------------------------------------------------------------

void ToggleFullScreenMode();
void SaveScreenshot();

extern ChatWriter* GlobalChatWriter; // may be NULL, when flash window not exists
extern SHORT GlobalSnapshotKey; // controlled from module 'flash.cpp'

HHOOK KeyboardHook::Hook = NULL;

//-----------------------------------------------------------------------------

KeyboardHook::KeyboardHook()
{
  if (Hook == NULL)
  {
    Hook = ::SetWindowsHookEx(WH_KEYBOARD,
                              (HOOKPROC)(KeyboardHook::HookProcedure),
                              NULL,
                              ::GetCurrentThreadId());
  }
}

//-----------------------------------------------------------------------------

KeyboardHook::~KeyboardHook()
{
  if (Hook != NULL)
  {
    ::UnhookWindowsHookEx(Hook);
    Hook = NULL;
  }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
KeyboardHook::HookProcedure(int code, WPARAM wParam, LPARAM lParam)
{
  if (code == HC_ACTION)
  {
    // lParam: 0??????? - down event
    // lParam: 4??????? - pressing events
    // lParam: C??????? - up event

    if (VK_F1 <= wParam && wParam <= VK_F12)
    {
      if ((lParam & ((KF_REPEAT + KF_UP) << 16)) == 0)
      {
        if (wParam == VK_F11)
        {
          ToggleFullScreenMode();
        }
        else if (wParam == VK_F12)
        {
          ToggleMenuVisible();
        }
        else
        {
          if (GlobalChatWriter != NULL)
          {
            GlobalChatWriter->DispatchKeyboardMessage(wParam, lParam);
          }
        }
      }
      return 1; // ignore this key
    }

    if (wParam == GlobalSnapshotKey)
    {
      // Key VK_SNAPSHOT send only one message: wParam: 2C, lParam: C1370001 !
      SaveScreenshot(); // reenterant
      return 1; // ignore this key
    }
  }

  return ::CallNextHookEx(Hook, code, wParam, lParam);
}

//-----------------------------------------------------------------------------
//    MouseHook
//-----------------------------------------------------------------------------

HHOOK MouseHook::Hook = NULL;

//-----------------------------------------------------------------------------

MouseHook::MouseHook()
{
  if (Hook == NULL)
  {
    Hook = ::SetWindowsHookEx(WH_MOUSE,
                              (HOOKPROC)(MouseHook::HookProcedure),
                              NULL,
                              ::GetCurrentThreadId());
  }
}

//-----------------------------------------------------------------------------

MouseHook::~MouseHook()
{
  if (Hook != NULL)
  {
    ::UnhookWindowsHookEx(Hook);
    Hook = NULL;
  }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
MouseHook::HookProcedure(int code, WPARAM wParam, LPARAM lParam)
{
  if (code == HC_ACTION)
  {
    if (wParam == WM_LBUTTONDOWN)
    {
      const MOUSEHOOKSTRUCT* info = reinterpret_cast<const MOUSEHOOKSTRUCT*>(lParam);
      if (info->pt.y == 0)
      {
        ToggleMenuVisible();
      }
    }
  }

  return ::CallNextHookEx(Hook, code, wParam, lParam);
}

//-----------------------------------------------------------------------------
