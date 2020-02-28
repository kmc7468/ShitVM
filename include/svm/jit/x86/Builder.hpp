#pragma once
#include <svm/Macro.hpp>

#ifdef SVM_X86

#include <svm/jit/x86/Address.hpp>
#include <svm/jit/x86/Instruction.hpp>
#include <svm/jit/x86/Register.hpp>

#include <functional>
#include <type_traits>
#include <variant>
#include <vector>

namespace svm::jit::x86 {
	using RMSize = RegisterSize;

	class RM final {
	private:
		std::variant<Register, std::reference_wrapper<const Address>> m_Data;

	public:
		RM(Register reg) noexcept;
		RM(const Address& addr) noexcept;
		RM(const RM&) = delete;
		~RM() = default;

	public:
		RM& operator=(const RM&) = delete;
		bool operator==(const RM&) = delete;
		bool operator!=(const RM&) = delete;

	public:
		bool IsRegister() const noexcept;
		bool IsAddress() const noexcept;
		Register GetRegister() const noexcept;
		const Address& GetAddress() const noexcept;

		RMSize GetSize() const noexcept;
	};
}

namespace svm::jit::x86 {
	class Builder final {
	private:
		std::vector<Instruction> m_Instructions;

	public:
		Builder() noexcept = default;
		Builder(Builder&& builder) noexcept;
		~Builder() = default;

	public:
		Builder& operator=(Builder&& builder) noexcept;
		bool operator==(const Builder&) = delete;
		bool operator!=(const Builder&) = delete;

	public:
		std::size_t GetResult(void* buffer) noexcept;
		std::size_t GetResultLength() const noexcept;

		void Mov(Register a, const RM& b);
		void Mov(const Address& a, Register b);
#ifdef SVM_X64
		void Mov(Register a, std::uint64_t b);
#endif
		void Mov(Register a, std::uint32_t b);
		void Mov(const Address& a, std::uint32_t b);

		void Push(Register a);
		void Push(const Address& a);
		void Push(std::uint32_t a);
		void Pop(Register a);
		void Pop(const Address& a);

		void Add(Register a, const RM& b);
		void Add(const Address& a, Register b);
		void Add(const RM& a, std::uint32_t b);
		void Sub(Register a, const RM& b);
		void Sub(const Address& a, Register b);
		void Sub(const RM& a, std::uint32_t b);
		void Mul(const RM& a);
		void IMul(const RM& a);
		void Div(const RM& a);
		void IDiv(const RM& a);
		void Neg(const RM& a);

		void And(Register a, const RM& b);
		void And(const Address& a, Register b);
		void And(const RM& a, std::uint32_t b);
		void Or(Register a, const RM& b);
		void Or(const Address& a, Register b);
		void Or(const RM& a, std::uint32_t b);
		void Xor(Register a, const RM& b);
		void Xor(const Address& a, Register b);
		void Xor(const RM& a, std::uint32_t b);
		void Not(const RM& a);

		void Shl(const RM& a);
		void Shl(const RM& a, std::uint8_t b);
		void Shl(const RM& a, Register b);
		void Sal(const RM& a);
		void Sal(const RM& a, std::uint8_t b);
		void Sal(const RM& a, Register b);
		void Shr(const RM& a);
		void Shr(const RM& a, std::uint8_t b);
		void Shr(const RM& a, Register b);
		void Sar(const RM& a);
		void Sar(const RM& a, std::uint8_t b);
		void Sar(const RM& a, Register b);

	private:
		void GenerateModRM(const RM& rm, REX& rex, ModRM& modRM) noexcept;
		void GenerateModRM(Register reg, REX& rex, ModRM& modRM) noexcept;
		void GenerateModRM(const Address& addr, REX& rex, ModRM& modRM) noexcept;
		void GenerateModRM(Register reg, const RM& rm, REX& rex, ModRM& modRM) noexcept;
		void GenerateModRM(Register reg1, Register reg2, REX& rex, ModRM& modRM) noexcept;
		void GenerateModRM(Register reg, const Address& addr, REX& rex, ModRM& modRM) noexcept;
		bool GenerateSIB(const Address& addr, REX& rex, SIB& sib) noexcept;
		void GenerateDisplacement(const Address& addr, DispImm& disp, std::uint8_t& dispSize) noexcept;

		void PushPopInternal(std::uint8_t opCode, Register a);
		void PushPopInternal(std::uint8_t opCode, std::uint8_t opCodeExt, const Address& a);

		void AddSubInternal(std::uint8_t opCode, Register a, const RM& b);
		void AddSubInternal(std::uint8_t opCode, const Address& a, Register b);
		void AddSubInternal(std::uint8_t opCode, const RM& a, std::uint32_t b);

		void MulDivInternal(std::uint8_t opCode, const RM& a);

		void BitShiftInternal(std::uint8_t opCode, std::uint8_t opCodeExt, const RM& a);
		void BitShiftInternal(std::uint8_t opCode, const RM& a);
		void BitShiftInternal(std::uint8_t opCode, const RM& a, std::uint8_t b);
		void BitShiftInternal(std::uint8_t opCode, const RM& a, Register b);
	};
}

#endif