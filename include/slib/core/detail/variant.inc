/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

namespace slib
{
	
	struct _priv_Variant_Const
	{
		sl_uint64 value;
		VariantType type;
		sl_int32 lock;
	};
	
	extern const _priv_Variant_Const _priv_Variant_Null;
	
	template <class... ARGS>
	String String::format(const sl_char8* szFormat, ARGS&&... args) noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return formatBy(szFormat, params, sizeof...(args));
	}
	
	template <class... ARGS>
	String String::format(const String& strFormat, ARGS&&... args) noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return formatBy(strFormat, params, sizeof...(args));
	}
	
	template <class... ARGS>
	String16 String16::format(const sl_char16* szFormat, ARGS&&... args) noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return formatBy(szFormat, params, sizeof...(args));
	}
	
	template <class... ARGS>
	String16 String16::format(const String16& strFormat, ARGS&&... args) noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return formatBy(strFormat, params, sizeof...(args));
	}
	
	template <class... ARGS>
	String String::arg(ARGS&&... args) const noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return argBy(params, sizeof...(args));
	}
	
	template <class... ARGS>
	String16 String16::arg(ARGS&&... args) const noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return argBy(params, sizeof...(args));
	}
	
	template <class... ARGS>
	String Atomic<String>::arg(ARGS&&... args) const noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return argBy(params, sizeof...(args));
	}
	
	template <class... ARGS>
	String16 Atomic<String16>::arg(ARGS&&... args) const noexcept
	{
		Variant params[] = {Forward<ARGS>(args)...};
		return argBy(params, sizeof...(args));
	}


	template <class T>
	Variant::Variant(const Ref<T>& ref) noexcept
	{
		if (ref.isNotNull()) {
			_type = VariantType::Object;
			new (reinterpret_cast<Ref<T>*>(&_value)) Ref<T>(ref);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	Variant::Variant(const AtomicRef<T>& _ref) noexcept
	{
		Ref<T> ref(_ref);
		if (ref.isNotNull()) {
			_type = VariantType::Object;
			new (reinterpret_cast<Ref<T>*>(&_value)) Ref<T>(ref);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	Variant::Variant(const WeakRef<T>& weak) noexcept
	{
		if (weak.isNotNull()) {
			_type = VariantType::Weak;
			new (reinterpret_cast<WeakRef<T>*>(&_value)) WeakRef<T>(weak);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	Variant::Variant(const AtomicWeakRef<T>& _weak) noexcept
	{
		Ref<T> weak(_weak);
		if (weak.isNotNull()) {
			_type = VariantType::Weak;
			new (reinterpret_cast<WeakRef<T>*>(&_value)) WeakRef<T>(weak);
		} else {
			_type = VariantType::Null;
		}
	}
	
	SLIB_INLINE const Variant& Variant::null() noexcept
	{
		return *(reinterpret_cast<Variant const*>(&_priv_Variant_Null));
	}
	
	template <class T>
	Variant Variant::fromRef(const Ref<T>& ref) noexcept
	{
		return ref;
	}
	
	template <class T>
	Variant Variant::fromWeakRef(const WeakRef<T>& weak) noexcept
	{
		return weak;
	}
	
	template <class T>
	SLIB_INLINE Variant& Variant::operator=(const T& value) noexcept
	{
		set(value);
		return *this;
	}
	
	
	SLIB_INLINE VariantType Variant::getType() const noexcept
	{
		return _type;
	}
	
	SLIB_INLINE sl_bool Variant::isNull() const noexcept
	{
		return _type == VariantType::Null;
	}
	
	SLIB_INLINE sl_bool Variant::isNotNull() const noexcept
	{
		return _type != VariantType::Null;
	}
	
	
	template <class T>
	Ref<T> Variant::getObject(const Ref<T>& def) const noexcept
	{
		return CastRef<T>(getObject(), def);
	}
	
	template <class T>
	void Variant::setObject(const Ref<T>& ref) noexcept
	{
		_free(_type, _value);
		if (ref.isNotNull()) {
			_type = VariantType::Object;
			new (reinterpret_cast<Ref<T>*>(&_value)) Ref<T>(ref);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	void Variant::setWeak(const WeakRef<T>& weak) noexcept
	{
		_free(_type, _value);
		if (weak.isNotNull()) {
			_type = VariantType::Weak;
			new (reinterpret_cast<WeakRef<T>*>(&_value)) WeakRef<T>(weak);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	void Variant::get(Ref<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Variant::set(const Ref<T>& _in) noexcept
	{
		setObject(_in);
	}
	
	template <class T>
	void Variant::get(AtomicRef<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Variant::set(const AtomicRef<T>& _in) noexcept
	{
		setObject(_in);
	}
	
	template <class T>
	void Variant::get(WeakRef<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Variant::set(const WeakRef<T>& _in) noexcept
	{
		setWeak(_in);
	}
	
	template <class T>
	void Variant::get(AtomicWeakRef<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Variant::set(const AtomicWeakRef<T>& _in) noexcept
	{
		setWeak(_in);
	}

	
	template <class T>
	Atomic<Variant>::Atomic(const Ref<T>& ref) noexcept
	{
		if (ref.isNotNull()) {
			_type = VariantType::Object;
			new (reinterpret_cast<Ref<T>*>(&_value)) Ref<T>(ref);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	Atomic<Variant>::Atomic(const AtomicRef<T>& _ref) noexcept
	{
		Ref<T> ref(_ref);
		if (ref.isNotNull()) {
			_type = VariantType::Object;
			new (reinterpret_cast<Ref<T>*>(&_value)) Ref<T>(ref);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	Atomic<Variant>::Atomic(const WeakRef<T>& weak) noexcept
	{
		if (weak.isNotNull()) {
			_type = VariantType::Weak;
			new (reinterpret_cast<WeakRef<T>*>(&_value)) WeakRef<T>(weak);
		} else {
			_type = VariantType::Null;
		}
	}
	
	template <class T>
	Atomic<Variant>::Atomic(const AtomicWeakRef<T>& _weak) noexcept
	{
		WeakRef<T> weak(_weak);
		if (weak.isNotNull()) {
			_type = VariantType::Weak;
			new (reinterpret_cast<WeakRef<T>*>(&_value)) WeakRef<T>(weak);
		} else {
			_type = VariantType::Null;
		}
	}
	
	SLIB_INLINE const AtomicVariant& Atomic<Variant>::null() noexcept
	{
		return *(reinterpret_cast<AtomicVariant const*>(&_priv_Variant_Null));
	}
	
	template <class T>
	SLIB_INLINE AtomicVariant& Atomic<Variant>::operator=(const T& value) noexcept
	{
		set(value);
		return *this;
	}
	
	SLIB_INLINE VariantType Atomic<Variant>::getType() const noexcept
	{
		return _type;
	}
	
	SLIB_INLINE sl_bool Atomic<Variant>::isNull() const noexcept
	{
		return _type == VariantType::Null;
	}
	
	SLIB_INLINE sl_bool Atomic<Variant>::isNotNull() const noexcept
	{
		return _type != VariantType::Null;
	}
	
	template <class T>
	Ref<T> Atomic<Variant>::getObject(const Ref<T>& def) const noexcept
	{
		Variant var(*this);
		return var.getObject(def);
	}
	
	template <class T>
	void Atomic<Variant>::setObject(const Ref<T>& object) noexcept
	{
		if (object.isNotNull()) {
			sl_int64 v;
			new (reinterpret_cast<Ref<T>*>(&v)) Ref<T>(object);
			_replace(VariantType::Object, v);
		} else {
			_replace(VariantType::Null, 0);
		}
	}
	
	template <class T>
	void Atomic<Variant>::setWeak(const WeakRef<T>& weak) noexcept
	{
		if (weak.isNotNull()) {
			sl_int64 v;
			new (reinterpret_cast<WeakRef<T>*>(&v)) WeakRef<T>(weak);
			_replace(VariantType::Weak, v);
		} else {
			_replace(VariantType::Null, 0);
		}
	}
	
	template <class T>
	void Atomic<Variant>::get(Ref<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Atomic<Variant>::set(const Ref<T>& _in) noexcept
	{
		setObject(_in);
	}
	
	template <class T>
	void Atomic<Variant>::get(AtomicRef<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Atomic<Variant>::set(const AtomicRef<T>& _in) noexcept
	{
		setObject(_in);
	}
	
	template <class T>
	void Atomic<Variant>::get(WeakRef<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Atomic<Variant>::set(const WeakRef<T>& _in) noexcept
	{
		setWeak(_in);
	}
	
	template <class T>
	void Atomic<Variant>::get(AtomicWeakRef<T>& _out) const noexcept
	{
		_out = CastRef<T>(getObject());
	}
	
	template <class T>
	void Atomic<Variant>::set(const AtomicWeakRef<T>& _in) noexcept
	{
		setWeak(_in);
	}
	
	
	extern const char _priv_VariantMap_ClassID[];
	
	template <>
	SLIB_INLINE sl_object_type IMap<String, Variant>::ObjectType() noexcept
	{
		return _priv_VariantMap_ClassID;
	}
	
	template <>
	SLIB_INLINE sl_bool IMap<String, Variant>::checkObjectType(sl_object_type type) noexcept
	{
		if (type == _priv_VariantMap_ClassID || type == _priv_Map_ClassID) {
			return sl_true;
		}
		return Object::checkObjectType(type);
	}
	
	template <>
	SLIB_INLINE sl_object_type IMap<String, Variant>::getObjectType() const noexcept
	{
		return _priv_VariantMap_ClassID;
	}
	
	template <>
	SLIB_INLINE sl_bool IMap<String, Variant>::isInstanceOf(sl_object_type type) const noexcept
	{
		if (type == _priv_VariantMap_ClassID || type == _priv_Map_ClassID) {
			return sl_true;
		}
		return Object::checkObjectType(type);
	}
	
	
	extern const char _priv_VariantList_ClassID[];
	
	template <>
	SLIB_INLINE sl_object_type CList<Variant>::ObjectType() noexcept
	{
		return _priv_VariantList_ClassID;
	}
	
	template <>
	SLIB_INLINE sl_bool CList<Variant>::checkObjectType(sl_object_type type) noexcept
	{
		if (type == _priv_VariantList_ClassID || type == _priv_List_ClassID) {
			return sl_true;
		}
		return Object::checkObjectType(type);
	}
	
	template <>
	SLIB_INLINE sl_object_type CList<Variant>::getObjectType() const noexcept
	{
		return _priv_VariantList_ClassID;
	}
	
	template <>
	SLIB_INLINE sl_bool CList<Variant>::isInstanceOf(sl_object_type type) const noexcept
	{
		if (type == _priv_VariantList_ClassID || type == _priv_List_ClassID) {
			return sl_true;
		}
		return Object::checkObjectType(type);
	}
	
	
	extern const char _priv_VariantMapList_ClassID[];
	
	template <>
	SLIB_INLINE sl_object_type CList< Map<String, Variant> >::ObjectType() noexcept
	{
		return _priv_VariantMapList_ClassID;
	}
	
	template <>
	SLIB_INLINE sl_bool CList< Map<String, Variant> >::checkObjectType(sl_object_type type) noexcept
	{
		if (type == _priv_VariantMapList_ClassID || type == _priv_Map_ClassID) {
			return sl_true;
		}
		return Object::checkObjectType(type);
	}
	
	template <>
	SLIB_INLINE sl_object_type CList< Map<String, Variant> >::getObjectType() const noexcept
	{
		return _priv_VariantMapList_ClassID;
	}
	
	template <>
	SLIB_INLINE sl_bool CList< Map<String, Variant> >::isInstanceOf(sl_object_type type) const noexcept
	{
		if (type == _priv_VariantMapList_ClassID || type == _priv_Map_ClassID) {
			return sl_true;
		}
		return Object::checkObjectType(type);
	}
	
}
