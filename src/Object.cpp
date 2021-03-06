#include <svm/Object.hpp>

namespace svm {
	Object::Object(Type type) noexcept
		: m_Type(type) {}
	Object::Object(const Object& object) noexcept
		: m_Type(object.m_Type) {}

	Object& Object::operator=(const Object& object) noexcept {
		m_Type = object.m_Type;
		return *this;
	}

	Type Object::GetType() const noexcept {
		return m_Type;
	}
}

namespace svm {
	IntObject::IntObject() noexcept
		: Object(IntType) {}
	IntObject::IntObject(std::uint32_t value) noexcept
		: Object(IntType), Value(value) {}
	IntObject::IntObject(const IntObject& object) noexcept
		: Object(object), Value(object.Value) {}

	IntObject& IntObject::operator=(const IntObject& object) noexcept {
		Object::operator=(object);

		Value = object.Value;
		return *this;
	}
}

namespace svm {
	LongObject::LongObject() noexcept
		: Object(LongType) {}
	LongObject::LongObject(std::uint64_t value) noexcept
		: Object(LongType), Value(value) {}
	LongObject::LongObject(const LongObject& object) noexcept
		: Object(object), Value(object.Value) {}

	LongObject& LongObject::operator=(const LongObject& object) noexcept {
		Object::operator=(object);

		Value = object.Value;
		return *this;
	}
}

namespace svm {
	DoubleObject::DoubleObject() noexcept
		: Object(DoubleType) {}
	DoubleObject::DoubleObject(double value) noexcept
		: Object(DoubleType), Value(value) {}
	DoubleObject::DoubleObject(const DoubleObject& object) noexcept
		: Object(object), Value(object.Value) {}

	DoubleObject& DoubleObject::operator=(const DoubleObject& object) noexcept {
		Object::operator=(object);

		Value = object.Value;
		return *this;
	}
}

namespace svm {
	PointerObject::PointerObject() noexcept
		: Object(PointerType) {}
	PointerObject::PointerObject(void* value) noexcept
		: Object(PointerType), Value(value) {}
	PointerObject::PointerObject(const PointerObject& object) noexcept
		: Object(object), Value(object.Value) {}

	PointerObject& PointerObject::operator=(const PointerObject& object) noexcept {
		Object::operator=(object);

		Value = object.Value;
		return *this;
	}
}

namespace svm {
	GCPointerObject::GCPointerObject() noexcept
		: Object(GCPointerType) {}
	GCPointerObject::GCPointerObject(void* value) noexcept
		: Object(GCPointerType), Value(value) {}
	GCPointerObject::GCPointerObject(const GCPointerObject& object) noexcept
		: Object(object), Value(object.Value) {}

	GCPointerObject& GCPointerObject::operator=(const GCPointerObject& object) noexcept {
		Object::operator=(object);

		Value = object.Value;
		return *this;
	}
}

namespace svm {
	ArrayObject::ArrayObject(std::size_t count) noexcept
		: Object(ArrayType), Count(count) {}
}

namespace svm {
	StructureObject::StructureObject(Type type) noexcept
		: Object(type) {}
}