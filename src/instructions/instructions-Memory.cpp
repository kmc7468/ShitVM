#include <svm/Interpreter.hpp>

#include <svm/detail/InterpreterExceptionCode.hpp>

namespace svm {
	void Interpreter::InterpretNull() noexcept {
		if (!m_Stack.Push<PointerObject>(nullptr)) {
			OccurException(SVM_IEC_STACK_OVERFLOW);
		}
	}
	void Interpreter::InterpretNew(std::uint32_t operand) {
		const Structures& structures = m_ByteFile.GetStructures();

		if (operand >= structures.GetStructureCount() + 10) {
			OccurException(SVM_IEC_TYPE_OUTOFRANGE);
			return;
		}

		if (m_Stack.GetFreeSize() < sizeof(PointerObject)) {
			OccurException(SVM_IEC_STACK_OVERFLOW);
			return;
		}

		const Type type = GetTypeFromTypeCode(structures, static_cast<TypeCode>(operand));
		void* address = m_Heap.AllocateUnmanagedHeap(type->Size);

		m_Stack.Push<PointerObject>(address);

		if (!address) return;
		else if (type.IsFundamentalType()) {
			*static_cast<Type*>(address) = type;
		} else if (type.IsStructure()) {
			InitStructure(structures, structures[operand - 10], static_cast<Type*>(address));
		}
	}
	void Interpreter::InterpretDelete() noexcept {
		if (IsLocalVariable()) {
			OccurException(SVM_IEC_STACK_EMPTY);
			return;
		}

		const Type* const typePtr = m_Stack.GetTopType();
		if (!typePtr) {
			OccurException(SVM_IEC_STACK_EMPTY);
			return;
		} else if (*typePtr != PointerType) {
			OccurException(SVM_IEC_POINTER_NOTPOINTER);
			return;
		}

		void* const address = reinterpret_cast<const PointerObject*>(typePtr)->Value;
		if (address && !m_Heap.DeallocateUnmanagedHeap(reinterpret_cast<const PointerObject*>(typePtr)->Value)) {
			OccurException(SVM_IEC_POINTER_UNKNOWNADDRESS);
		}

		m_Stack.Reduce(sizeof(PointerObject));
	}
	void Interpreter::InterpretGCNull() noexcept {
		if (!m_Stack.Push<GCPointerObject>(nullptr)) {
			OccurException(SVM_IEC_STACK_OVERFLOW);
		}
	}
	void Interpreter::InterpretGCNew(std::uint32_t operand) {
		const Structures& structures = m_ByteFile.GetStructures();

		if (operand >= structures.GetStructureCount() + 10) {
			OccurException(SVM_IEC_TYPE_OUTOFRANGE);
			return;
		}

		if (m_Stack.GetFreeSize() < sizeof(PointerObject)) {
			OccurException(SVM_IEC_STACK_OVERFLOW);
			return;
		}

		const Type type = GetTypeFromTypeCode(structures, static_cast<TypeCode>(operand));
		void* const address = m_Heap.AllocateManagedHeap(*this, type->Size);
		Type* const addressReal = reinterpret_cast<Type*>(static_cast<ManagedHeapInfo*>(address) + 1);

		m_Stack.Push<GCPointerObject>(address);

		if (!address) return;
		else if (type.IsFundamentalType()) {
			*addressReal = type;
		} else if (type.IsStructure()) {
			InitStructure(structures, structures[operand - 10], addressReal);
		}
	}
}