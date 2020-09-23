/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
**
** This file and the associated product may be used and distributed
** under the terms of a current License as defined by AtOnce Technologies
** and appearing in the file LICENSE.TXT included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.atonce-technologies.com or email info@atonce-technologies.com
** for informations about Sugar Framework and AtOnce Technologies others products
** License Agreements.
**
** Contact info@atonce-technologies.com if any conditions of this license
** are not clear to you.
**
*****************************************************************LIC-HDR*/



#ifndef _su_ttrait_h_
#define _su_ttrait_h_



namespace su
{


	// integral_constant
	template <typename T, T v>
		struct integral_constant {
			typedef T						value_type;
			static const T					value = v;
			typedef	integral_constant<T,v>	type;
		};


	// sizetype
	template <int v>
		struct sizetype
			: public integral_constant<int,v> {};


	// bool_type
	template <bool v>
		struct bool_type
			: public integral_constant<bool,v> {};
	typedef bool_type<false>		false_type;
	typedef bool_type<true>			true_type;


	// support_type
	template <int v>
		struct support_type
			: public integral_constant<int,v> {};
	typedef support_type<0>			no_type;
	typedef support_type<1>			yes_type;
	typedef support_type<2>			undef_type;
	// support_eq
	template <typename S1, typename S2>
		struct support_eq
			: public bool_type< S1::value == S2::value > {};
	// support_ne
	template <typename S1, typename S2>
		struct support_ne
			: public bool_type< S1::value != S2::value > {};
	// support_defined
	template <typename S>
		struct support_defined
			: public bool_type< S::value != undef_type::value > {};
	// support_undefined
	template <typename S>
		struct support_undefined
			: public bool_type< S::value == undef_type::value > {};
	// support_yes
	template <typename S>
		struct support_yes
			: public bool_type< S::value == yes_type::value > {};
	// support_no
	template <typename S>
		struct support_no
			: public bool_type< S::value == no_type::value > {};
	// support_yes_or_undefined
	template <typename S>
		struct support_yes_or_undefined
			: public bool_type< S::value != no_type::value > {};
	// support_no_or_undefined
	template <typename S>
		struct support_no_or_undefined
			: public bool_type< S::value != yes_type::value > {};
	// support_or
	template < int v1, int v2 >
		struct support_or_v
			: public undef_type {};
	template < int v >
		struct support_or_v<yes_type::value,v>
			: public yes_type {};
	template < int v >
		struct support_or_v<v,yes_type::value>
			: public yes_type {};
	template <>
		struct support_or_v<yes_type::value,yes_type::value>
			: public yes_type {};
	template <>
		struct support_or_v<no_type::value,no_type::value>
			: public no_type {};
	template <typename S1, typename S2>
		struct support_or
			: public support_or_v<S1::value,S2::value> {};
	// support_and
	template < int v1, int v2 >
		struct support_and_v
			: public undef_type {};
	template <>
		struct support_and_v<yes_type::value,yes_type::value>
			: public yes_type {};
	template <>
		struct support_and_v<yes_type::value,no_type::value>
			: public no_type {};
	template <>
		struct support_and_v<no_type::value,yes_type::value>
			: public no_type {};
	template <>
		struct support_and_v<no_type::value,no_type::value>
			: public no_type {};
	template <typename S1, typename S2>
		struct support_and
			: public support_and_v<S1::value,S2::value> {};
	// support_not
	template <int v>
		struct support_not_v
			: public undef_type {};
	template <>
		struct support_not_v<yes_type::value>
			: public no_type {};
	template <>
		struct support_not_v<no_type::value>
			: public yes_type {};
	template <typename S>
		struct support_not
			: public support_not_v<S::value> {};
	// support_bool
	template <bool v>
		struct support_bool
			: public undef_type {};
	template <>
		struct support_bool<true>
			: public yes_type {};
	template <>
		struct support_bool<false>
			: public no_type {};


	// remove_const
	template <typename T>
		struct remove_const {
			typedef T type;
		};
	template <typename T>
		struct remove_const<T const> {
			typedef T type;
		};


	// remove_volatile
	template <typename T>
		struct remove_volatile {
			typedef T type;
		};
	template <typename T>
		struct remove_volatile<T volatile> {
			typedef T type;
		};


	// remove_cv
	template <typename T>
		struct remove_cv {
			typedef typename remove_const<
				typename remove_volatile<T>::type
			>::type type;
		};


	// remove_pointer
	template <typename T>
		struct remove_pointer {
			typedef T type;
		};
	template <typename T>
		struct remove_pointer<T*> {
			typedef T type;
		};
	template <typename T>
		struct remove_pointer<T* const> {
			typedef T type;
		};
	template <typename T>
		struct remove_pointer<T* volatile> {
			typedef T type;
		};
	template <typename T>
		struct remove_pointer<T* const volatile> {
			typedef T type;
		};


	// remove_reference
	template <typename T>
		struct remove_reference {
			typedef T type;
		};
	template <typename T>
		struct remove_reference<T&> {
			typedef T type;
		};


	// remove_all
	template <typename T>
		struct remove_all {
			typedef typename remove_const<
				typename remove_volatile<
					typename remove_pointer<
						typename remove_reference<T>::type
					>::type
				>::type
			>::type type;
		};


	// add_reference
	template <typename T>
		struct add_reference {
			typedef T& type;
		};
	template <typename T>
		struct add_reference<T&> {
			typedef T& type;
		};


	// remove_bounds
	template <typename T>
		struct remove_bounds {
			typedef T type;
		};
	template <typename T, uint N>
		struct remove_bounds<T[N]> {
			typedef T type;
		};
	template <typename T>
		struct remove_bounds<T[]> {
			typedef T type;
		};


	// bounds
	template <typename T>
		struct bounds
			: public integral_constant<uint,0> {};
	template <typename T, uint N>
		struct bounds<T[N]>
			: public integral_constant<uint,N> {};
	template <typename T>
		struct bounds<T[]>
			: public integral_constant<uint,0> {};


	// is_void
	template <typename T>
		struct is_void
			: public false_type {};
	template <>
		struct is_void<void>
			: public true_type {};

		
	// is_same
	template <typename U, typename V>
		struct is_same
			: public false_type {};
	template <typename T>
		struct is_same<T,T>
			: public true_type {};


	// is_const
	template <typename T>
		struct is_const
			: public bool_type< !is_same<T, typename remove_const<T>::type>::value > {};


	// is_volatile
	template <typename T>
		struct is_volatile
			: public bool_type< !is_same<T, typename remove_volatile<T>::type>::value > {};


	// is_signed/unsigned
	template <typename T>
		struct is_signed
			: public bool_type< (T(-1)<T(0)) > {};
	template <typename T>
		struct is_unsigned
			: public bool_type< (T(-1)>T(0)) > {};


	// is_array
	template <typename T>
		struct is_array
			: public false_type {};
	template <typename T, uint N>
		struct is_array<T[N]>
			: public true_type {};
	template<typename T>
		struct is_array<T[]>
			: public true_type {};


	// is_pointer
	template <typename T>
		struct is_pointer
			: public false_type {};
	template <typename T>
		struct is_pointer<T*>
			: public true_type {};
	template <typename T>
		struct is_pointer<T* const>
			: public true_type {};
	template <typename T>
		struct is_pointer<T* volatile>
			: public true_type {};
	template <typename T>
		struct is_pointer<T* const volatile>
			: public true_type {};


	// is_integral
	template <typename T>
		struct is_integral
			: public false_type {};
	template <>
		struct is_integral<unsigned char>
			: public true_type {};
	template <>
		struct is_integral<unsigned short>
			: public true_type {};
	template <>
		struct is_integral<unsigned int>
			: public true_type {};
	template <>
		struct is_integral<unsigned long>
			: public true_type {};
	template <>
		struct is_integral<signed char>
			: public true_type {};
	template <>
		struct is_integral<signed short>
			: public true_type {};
	template <>
		struct is_integral<signed int>
			: public true_type {};
	template <>
		struct is_integral<signed long>
			: public true_type {};
	template <>
		struct is_integral<bool>
			: public true_type {};
	template <>
		struct is_integral<char>
			: public true_type {};
	template <>
		struct is_integral<int64>
			: public true_type {};
	template <>
		struct is_integral<int128>
			: public true_type {};
	template <>
		struct is_integral<uint64>
			: public true_type {};
	template <>
		struct is_integral<uint128>
			: public true_type {};


	// is_floating_point
	template <typename T>
		struct is_floating_point
			: public false_type {};
	template <>
		struct is_floating_point<float> 
			: public true_type {};
	template <>
		struct is_floating_point<double>
			: public true_type {};


	// is_arithmetic
	template <typename T>
		struct is_arithmetic
			: public bool_type<		is_integral<T>::value
								||	is_floating_point<T>::value		> {};


	// is_fundamental
	template <typename T>
		struct is_fundamental
			: public bool_type<		is_arithmetic<T>::value
								||	is_void<T>::value		> {};


	// is_compound
	template <typename T>
		struct is_compound
			: public bool_type< !is_fundamental<T>::value > {};


	// is_pod
	#if defined(SU_CC_IS_POD)
	template <typename T>
		struct is_pod
			: public support_bool< SU_CC_IS_POD(T) > {};
	#else
		template <typename T>
		struct is_pod
			: public undef_type {};
	#endif


	// is_enum
	#if defined(SU_CC_IS_ENUM)
	template <typename T>
		struct is_enum
			: public support_bool< SU_CC_IS_ENUM(T) > {};
	#else
	template <typename T>
		struct is_enum
			: public undef_type {};
	#endif


	// is_union
	#if defined(SU_CC_IS_UNION)
	template <typename T>
		struct is_union
			: public support_bool< SU_CC_IS_UNION(T) > {};
	#else
	template <typename T>
		struct is_union
			: public undef_type {};
	#endif


	// is_class
	#if defined(SU_CC_IS_CLASS)
	template <typename T>
		struct is_class
			: public support_bool< SU_CC_IS_CLASS(T) > {};
	#else
	template <typename T>
		struct is_class
			: public undef_type {};
	#endif


	// is_empty
	#if defined(SU_CC_IS_EMPTY)
	template <typename T>
		struct is_empty
			: public support_bool< SU_CC_IS_EMPTY(T) > {};
	#else
	template <typename T>
		struct is_empty
			: public undef_type {};
	#endif


	// is_polymorphic
	#if defined(SU_CC_IS_POLYMORPHIC)
	template <typename T>
		struct is_polymorphic
			: public support_bool< SU_CC_IS_POLYMORPHIC(T) > {};
	#else
	template <typename T>
		struct is_polymorphic
			: public undef_type {};
	#endif


	// is_abstract
	#if defined(SU_CC_IS_ABSTRACT)
	template <typename T>
		struct is_abstract
			: public support_bool< SU_CC_IS_ABSTRACT(T) > {};
	#else
	template <typename T>
		struct is_abstract
			: public undef_type {};
	#endif


	// has_trivial_ctor
	// is it a trivial type with compiler-generated constructor ?
	#if defined(SU_CC_HAS_TRIVIAL_CTOR)
	template <typename T>
		struct has_trivial_ctor
			: public support_bool<		is_fundamental<T>::value
									||	SU_CC_HAS_TRIVIAL_CTOR(T) > {};
	#else
	template <typename T>
		struct has_trivial_ctor
			: public support_or<		support_bool< is_fundamental<T>::value >
									,	undef_type	> {};
	#endif


	// has_trivial_dtor
	// is it a trivial type with compiler-generated destructor ?
	#if defined(SU_CC_HAS_TRIVIAL_DTOR)
	template <typename T>
		struct has_trivial_dtor
			: public support_bool<		is_fundamental<T>::value
									||	SU_CC_HAS_TRIVIAL_DTOR(T) > {};
	#else
	template <typename T>
		struct has_trivial_dtor
			: public support_or<		support_bool< is_fundamental<T>::value >
									,	undef_type	> {};
	#endif


	// has_virtual_dtor
	// is it a type has a virtual destructor ?
	#if defined(SU_CC_HAS_VIRTUAL_DTOR)
	template <typename T>
		struct has_virtual_dtor
			: public support_bool< SU_CC_HAS_VIRTUAL_DTOR(T) > {};
	#else
	template <typename T>
		struct has_virtual_dtor
			: public undef_type {};
	#endif


	// has_trivial_copy
	// is a trivial type with compiler-generated copy constructor ?
	#if defined(SU_CC_HAS_TRIVIAL_COPY)
	template <typename T>
		struct has_trivial_copy
			: public support_bool<		is_fundamental<T>::value
									||	SU_CC_HAS_TRIVIAL_COPY(T) > {};
	#else
	template <typename T>
		struct has_trivial_copy
			: public support_or<		support_bool< is_fundamental<T>::value >
									,	undef_type	> {};
	#endif


	// has_trivial_assign
	// is a trivial type with compiler-generated copy assignment operator ?
	#if defined(SU_CC_HAS_TRIVIAL_ASSIGN)
	template <typename T>
		struct has_trivial_assign
			: public support_bool<		is_fundamental<T>::value
									||	SU_CC_HAS_TRIVIAL_ASSIGN(T) > {};
	#else
	template <typename T>
		struct has_trivial_assign
			: public support_or<		support_bool< is_fundamental<T>::value >
									,	undef_type	> {};
	#endif


	// is_trivial
	template <typename T>
		struct is_trivial
			: public support_and<		has_trivial_ctor<T>,
					 support_and<		has_trivial_copy<T>,
					 support_and<		has_trivial_assign<T>
									,	has_trivial_dtor<T>		> > > {};


	// has_user_cdtor
	template <typename T>
		struct has_user_cdtor
			: public support_or<		support_not< has_trivial_ctor<T> >
									,	support_not< has_trivial_dtor<T> > > {};


}




#endif // _su_ttrait_h_


