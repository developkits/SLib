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
	
	template <class T>
	CArray<T>::CArray(const std::initializer_list<T>& l) noexcept : CArray(l.begin(), l.size())
	{
	}

	template <class T>
	CArray<T>* CArray<T>::create(const std::initializer_list<T>& l) noexcept
	{
		return create(l.begin(), l.size());
	}
	
	
	template <class T>
	Array<T>::Array(const std::initializer_list<T>& l) noexcept : ref(CArray<T>::create(l.begin(), l.size()))
	{
	}
	
	template <class T>
	Array<T> Array<T>::create(const std::initializer_list<T>& l) noexcept
	{
		return create(l.begin(), l.size());
	}
	
	template <class T>
	Array<T>& Array<T>::operator=(const std::initializer_list<T>& l) noexcept
	{
		ref = CArray<T>::create(l.begin(), l.size());
		return *this;
	}
	
	
	template <class T>
	Atomic< Array<T> >::Atomic(const std::initializer_list<T>& l) noexcept : ref(CArray<T>::create(l.begin(), l.size()))
	{
	}
	
	template <class T>
	Atomic< Array<T> >& Atomic< Array<T> >::operator=(const std::initializer_list<T>& l) noexcept
	{
		ref = CArray<T>::create(l.begin(), l.size());
		return *this;
	}
		
}
