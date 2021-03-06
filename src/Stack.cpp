#include <svm/Stack.hpp>

#include <utility>

namespace svm {
	Stack::Stack(std::size_t size) {
		Allocate(size);
	}
	Stack::Stack(Stack&& stack) noexcept
		: m_Data(std::move(stack.m_Data)), m_Used(stack.m_Used) {}

	Stack& Stack::operator=(Stack&& stack) noexcept {
		m_Data = std::move(stack.m_Data);
		m_Used = stack.m_Used;
		return *this;
	}

	void Stack::Allocate(std::size_t size) {
		m_Data.resize(size);
		m_Used = 0;
	}
	void Stack::Reallocate(std::size_t size) {
		m_Data.resize(size);
	}
	void Stack::Deallocate() noexcept {
		m_Data.clear();
		m_Used = 0;
	}

	const Type* Stack::GetTopType() const noexcept {
		return GetTop<Type>();
	}
	Type* Stack::GetTopType() noexcept {
		return GetTop<Type>();
	}

	std::size_t Stack::GetSize() const noexcept {
		return m_Data.size();
	}
	std::size_t Stack::GetUsedSize() const noexcept {
		return m_Used;
	}
	void Stack::SetUsedSize(std::size_t newUsedSize) noexcept {
		m_Used = newUsedSize;
	}
	std::size_t Stack::GetFreeSize() const noexcept {
		return GetSize() - GetUsedSize();
	}

	bool Stack::Expand(std::size_t delta) noexcept {
		if (GetFreeSize() < delta) return false;

		m_Used += delta;
		return true;
	}
	void Stack::Reduce(std::size_t delta) noexcept {
		m_Used -= delta;
	}

	const std::uint8_t* Stack::Begin() const noexcept {
		return &*m_Data.begin();
	}
	std::uint8_t* Stack::Begin() noexcept {
		return &*m_Data.begin();
	}
	const std::uint8_t* Stack::Last() const noexcept {
		return &*(m_Data.end() - 1);
	}
	std::uint8_t* Stack::Last() noexcept {
		return &*(m_Data.end() - 1);
	}
}