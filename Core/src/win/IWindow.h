#pragma once 
#include "ChilWin.h"
#include <future>
#include <string>
#include <optional>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/spa/Vec2.h>

namespace chil::win
{
	class IWindow
	{
		// allow WindowClasses access to the message handling function 
		friend class IWindowClass;
	public:
		// types 
		struct IocParams
		{
			std::shared_ptr<IWindowClass> pClass;
			std::optional<std::wstring> name;
			std::optional<spa::DimensionsI> size;
			std::optional<spa::Vec2I> position;
		};
		// functions 
		virtual ~IWindow() = default;
		virtual HWND GetHandle() const = 0;
		virtual bool IsClosing() const = 0;
		virtual std::future<void> SetTitle(std::wstring title) = 0;
	protected:
		virtual LRESULT HandleMessage_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept = 0;
	};
}