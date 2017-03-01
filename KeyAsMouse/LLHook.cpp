#include "LLHook.h"

namespace KeyAsMouse
{
	namespace LLHook
	{
		double dpiscale;
	}
}

LRESULT CALLBACK KeyAsMouse::LLHook::LLKBProc (_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx (NULL, nCode, wParam, lParam);

	static KBDLLHOOKSTRUCT *hs;
	hs = (KBDLLHOOKSTRUCT *)lParam;

	switch (wParam)
	{
		case WM_KEYDOWN:
		{
			MKBState::UpdKeyDown (static_cast<int>(hs->vkCode));

			if (MKBState::ShouldIntercept (hs->vkCode))
				return -1;
			else
				break;
		}

		case WM_KEYUP:
		{
			MKBState::UpdKeyUp (static_cast<int>(hs->vkCode));

			if (MKBState::ShouldIntercept (hs->vkCode))
				return -1;
			else
				break;
		}

		default:
			break;
	}

	return CallNextHookEx (NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyAsMouse::LLHook::LLMouseProc (_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx (NULL, nCode, wParam, lParam);

	static MSLLHOOKSTRUCT *hs;
	static POINT p;
	hs = (MSLLHOOKSTRUCT *)lParam;
	GetCursorPos (&p);

	switch (wParam)
	{
		case WM_MOUSEMOVE:
		{
			if (!(hs->flags & LLMHF_INJECTED))
				MKBState::UpdMousePos (static_cast<double>(p.x), static_cast<double>(p.y));

			break;
		}

		default:
			break;
	}

	return CallNextHookEx (NULL, nCode, wParam, lParam);
}