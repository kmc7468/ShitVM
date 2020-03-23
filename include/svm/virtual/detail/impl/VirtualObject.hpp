#pragma once
#include <svm/virtual/VirtualObject.hpp>

#include <svm/detail/Overload.hpp>

namespace svm {
	template<typename T, typename F>
	T VirtualObject::Native(F&& function) const noexcept {
		if (std::holds_alternative<std::monostate>(m_Object)) return 0;
		else return std::visit(function, std::visit(detail::Overload{
			[](std::monostate) noexcept -> ObjectVariant {
				return IntObject();
			},
			[](const auto& object) noexcept -> ObjectVariant {
				return GetObject(object);
			}
		}, m_Object));
	}
	template<typename T>
	T VirtualObject::NativeArithmetic() const noexcept {
		return Native<T>(detail::Overload{
			[](const PointerObject& pointer) noexcept {
				return static_cast<T>(reinterpret_cast<std::uintptr_t>(pointer.Value));
			},
			[](const GCPointerObject& pointer) noexcept {
				return static_cast<T>(reinterpret_cast<std::uintptr_t>(pointer.Value));
			},
			[](const auto& object) noexcept {
				return static_cast<T>(object.Value);
			}
		});
	}

	template<typename T>
	int VirtualObject::Compare(T lhs, T rhs) noexcept {
		if (lhs > rhs) return 1;
		else if (lhs == rhs) return 0;
		else return -1;
	}
	template<typename F>
	VirtualObject VirtualObject::ArithmeticOperation(const VirtualObject& rhs, F&& function) const noexcept {
		if (IsEmpty() || rhs.IsEmpty()) return VNULL;

		ObjectVariant lhsObject = std::visit(detail::Overload{
			[](std::monostate) noexcept -> ObjectVariant {
				return IntObject();
			},
			[](const auto& object) noexcept -> ObjectVariant {
				return GetObject(object);
			}
		}, m_Object);
		ObjectVariant rhsObject = std::visit(detail::Overload{
			[](std::monostate) noexcept -> ObjectVariant {
				return IntObject();
			},
			[](const auto& object) noexcept -> ObjectVariant {
				return GetObject(object);
			}
		}, rhs.m_Object);
		if (!MakeSameType(lhsObject, rhsObject)) return VNULL;

		return std::visit(detail::Overload{
			[](const PointerObject&) noexcept -> VirtualObject { return VNULL; },
			[](const GCPointerObject&) noexcept -> VirtualObject { return VNULL; },
			[rhsObject, &function](const auto& lhs) noexcept -> VirtualObject {
				return function(lhs.Value, std::get<std::decay_t<decltype(lhs)>>(rhsObject).Value);
			}
		}, lhsObject);
	}
}