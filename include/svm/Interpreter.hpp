#pragma once

#include <svm/ByteFile.hpp>
#include <svm/Exception.hpp>
#include <svm/Macro.hpp>
#include <svm/Type.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

namespace svm {
	class Stack final {
	private:
		std::vector<std::uint8_t> m_Data;
		std::size_t m_Used = 0;

	public:
		Stack() noexcept = default;
		explicit Stack(std::size_t size);
		Stack(Stack&& stack) noexcept;
		~Stack() = default;

	public:
		Stack& operator=(Stack&& stack) noexcept;

	public:
		void Allocate(std::size_t size);
		void Reallocate(std::size_t newSize);
		void Deallocate() noexcept;

		template<typename T>
		bool Push(const T& value) noexcept {
			if (GetFreeSize() < sizeof(T)) return false;

			*reinterpret_cast<T*>(&*(m_Data.rbegin() + (m_Used += sizeof(T)) - 1)) = value;
			return true;
		}
		template<typename T>
		std::optional<T> Pop() noexcept {
			T* result = GetTop<T>();
			if (!result) return std::nullopt;

			m_Used -= sizeof(T);
			return *result;
		}
		template<typename T>
		const T* Get(std::size_t offset) const noexcept {
			if (m_Used < offset - sizeof(T)) return nullptr;
			else return reinterpret_cast<const T*>(&*(m_Data.rbegin() + offset - 1));
		}
		template<typename T>
		T* Get(std::size_t offset) noexcept {
			if (m_Used < offset - sizeof(T)) return nullptr;
			else return reinterpret_cast<T*>(&*(m_Data.rbegin() + offset - 1));
		}
		template<typename T>
		const T* GetTop() const noexcept {
			return Get<T>(m_Used);
		}
		template<typename T>
		T* GetTop() noexcept {
			return Get<T>(m_Used);
		}
		const Type* GetTopType() const noexcept;
		Type* GetTopType() noexcept;
		std::size_t GetSize() const noexcept;
		std::size_t GetUsedSize() const noexcept;
		std::size_t GetFreeSize() const noexcept;
		void RemoveTo(std::size_t newSize) noexcept;
		void Remove(std::size_t delta) noexcept;
		bool Add(std::size_t delta) noexcept;
	};

	struct StackFrame final {
		svm::Type Type = NoneType;
		std::size_t StackBegin = 0;
		std::size_t VariableBegin = 0;
		std::size_t Caller = std::numeric_limits<std::size_t>::max();
		const svm::Function* Function = nullptr;
		const svm::Instructions* Instructions = nullptr;
	};
}

namespace svm {
	class Interpreter final {
	public:
		using Result = std::variant<std::monostate, std::uint32_t, std::uint64_t, double, void*, const StructureObject*>;

	private:
		ByteFile m_ByteFile;

		Stack m_Stack;
		StackFrame m_StackFrame;
		std::size_t m_Depth = 0;
		std::uint64_t m_InstructionIndex = 0;

		std::vector<std::size_t> m_LocalVariables;

		std::optional<InterpreterException> m_Exception;

	public:
		Interpreter() noexcept = default;
		explicit Interpreter(ByteFile&& byteFile) noexcept;
		Interpreter(Interpreter&& interpreter) noexcept;
		~Interpreter() = default;

	public:
		Interpreter& operator=(Interpreter&& interpreter) noexcept;
		bool operator==(const ByteFile&) = delete;
		bool operator!=(const ByteFile&) = delete;

	public:
		void Clear() noexcept;
		void Load(ByteFile&& byteFile) noexcept;
		void AllocateStack(std::size_t size = 1 * 1024 * 1024);
		void ReallocateStack(std::size_t newSize);
		const ByteFile& GetByteFile() const noexcept;

		bool Interpret();
		const InterpreterException& GetException() const noexcept;
		Result GetResult() const noexcept;
		std::vector<StackFrame> GetCallStacks() const;

	private:
		void OccurException(std::uint32_t code) noexcept;
		bool IsLocalVariable(std::size_t delta = 0) const noexcept;

		void PushStructure(std::uint32_t code) noexcept;
		void InitStructure(Structure structure, Type* type) const noexcept;
		void CopyStructure(const Type& type) noexcept;
		void CopyStructure(const Type& from, Type& to) const noexcept;
		template<typename T>
		void DRefAndAssign(Type* rhsTypePtr) noexcept;
		template<>
		void DRefAndAssign<StructureObject>(Type* rhsTypePtr) noexcept;
		template<typename T>
		bool GetTwoSameType(Type rhsType, T*& lhs) noexcept;

		template<typename T>
		bool PopTwoSameType(Type& rhsType, T& lhs, T& rhs) noexcept;
		template<typename T>
		IntObject CompareTwoSameType(T lhs, T rhs) noexcept;
		template<typename T>
		void JumpCondition(std::uint32_t operand);

		void InterpretPush(std::uint32_t operand);
		void InterpretPop();
		void InterpretLoad(std::uint32_t operand);
		void InterpretStore(std::uint32_t operand);
		void InterpretLea(std::uint32_t operand);
		void InterpretFLea(std::uint32_t operand);
		void InterpretTLoad();
		void InterpretTStore();
		void InterpretCopy();
		void InterpretSwap();

		void InterpretAdd();
		void InterpretSub();
		void InterpretMul();
		void InterpretIMul();
		void InterpretDiv();
		void InterpretIDiv();
		void InterpretMod();
		void InterpretIMod();
		void InterpretNeg();
		void InterpretIncDec(int delta);

		void InterpretAnd();
		void InterpretOr();
		void InterpretXor();
		void InterpretNot();
		void InterpretShl();
		void InterpretSal();
		void InterpretShr();
		void InterpretSar();

		void InterpretCmp();
		void InterpretICmp();
		void InterpretJmp(std::uint32_t operand);
		void InterpretJe(std::uint32_t operand);
		void InterpretJne(std::uint32_t operand);
		void InterpretJa(std::uint32_t operand);
		void InterpretJae(std::uint32_t operand);
		void InterpretJb(std::uint32_t operand);
		void InterpretJbe(std::uint32_t operand);
		void InterpretCall(std::uint32_t operand);
		void InterpretRet();

		void InterpretToI();
		void InterpretToL();
		void InterpretToD();
		void InterpretToP();
	};
}