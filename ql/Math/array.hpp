/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl
 Copyright (C) 2004 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file array.hpp
    \brief 1-D array used in linear algebra.
*/

#ifndef quantlib_array_hpp
#define quantlib_array_hpp

#include <ql/types.hpp>
#include <ql/errors.hpp>
#include <ql/Utilities/disposable.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/scoped_array.hpp>
#include <functional>
#include <numeric>
#include <iomanip>

namespace QuantLib {

    //! 1-D array used in linear algebra.
    /*! This class implements the concept of vector as used in linear
        algebra.
        As such, it is <b>not</b> meant to be used as a container -
        <tt>std::vector</tt> should be used instead.

        \test construction of arrays is checked in a number of cases
    */
    class Array {
      public:
        //! \name Constructors, destructor, and assignment
        //@{
        //! creates the array with the given dimension
        explicit Array(Size size = 0);
        //! creates the array and fills it with <tt>value</tt>
        Array(Size size, Real value);
        /*! \brief creates the array and fills it according to
            \f$ a_{0} = value, a_{i}=a_{i-1}+increment \f$
        */
        Array(Size size, Real value, Real increment);
        Array(const Array&);
        Array(const Disposable<Array>&);
        Array& operator=(const Array&);
        Array& operator=(const Disposable<Array>&);
        //@}
        /*! \name Vector algebra

            <tt>v += x</tt> and similar operation involving a scalar value
            are shortcuts for \f$ \forall i : v_i = v_i + x \f$

            <tt>v *= w</tt> and similar operation involving two vectors are
            shortcuts for \f$ \forall i : v_i = v_i \times w_i \f$

            \pre all arrays involved in an algebraic expression must have
            the same size.
        */
        //@{
        const Array& operator+=(const Array&);
        const Array& operator+=(Real);
        const Array& operator-=(const Array&);
        const Array& operator-=(Real);
        const Array& operator*=(const Array&);
        const Array& operator*=(Real);
        const Array& operator/=(const Array&);
        const Array& operator/=(Real);
        //@}
        //! \name Element access
        //@{
        //! read-only
        Real operator[](Size) const;
        Real at(Size) const;
        //! read-write
        Real& operator[](Size);
        Real& at(Size);
        //@}
        //! \name Inspectors
        //@{
        //! dimension of the array
        Size size() const;
        //! whether the array is empty
        bool empty() const;
        //@}
        typedef Real* iterator;
        typedef const Real* const_iterator;
        typedef boost::reverse_iterator<iterator> reverse_iterator;
        typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;
        //! \name Iterator access
        //@{
        const_iterator begin() const;
        iterator begin();
        const_iterator end() const;
        iterator end();
        const_reverse_iterator rbegin() const;
        reverse_iterator rbegin();
        const_reverse_iterator rend() const;
        reverse_iterator rend();
        //@}
        //! \name Utilities
        //@{
        void swap(Array&);  // never throws
        //@}

      private:
        boost::scoped_array<Real> data_;
        Size n_;
    };

    /*! \relates Array */
    Real DotProduct(const Array&, const Array&);

    // unary operators
    /*! \relates Array */
    const Disposable<Array> operator+(const Array& v);
    /*! \relates Array */
    const Disposable<Array> operator-(const Array& v);

    // binary operators
    /*! \relates Array */
    const Disposable<Array> operator+(const Array&, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator+(const Array&, Real);
    /*! \relates Array */
    const Disposable<Array> operator+(Real, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator-(const Array&, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator-(const Array&, Real);
    /*! \relates Array */
    const Disposable<Array> operator-(Real, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator*(const Array&, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator*(const Array&, Real);
    /*! \relates Array */
    const Disposable<Array> operator*(Real, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator/(const Array&, const Array&);
    /*! \relates Array */
    const Disposable<Array> operator/(const Array&, Real);
    /*! \relates Array */
    const Disposable<Array> operator/(Real, const Array&);

    // math functions
    /*! \relates Array */
    const Disposable<Array> Abs(const Array&);
    /*! \relates Array */
    const Disposable<Array> Sqrt(const Array&);
    /*! \relates Array */
    const Disposable<Array> Log(const Array&);
    /*! \relates Array */
    const Disposable<Array> Exp(const Array&);

    // utilities
    /*! \relates Array */
    void swap(Array&, Array&);

    // format
    /*! \relates Array */
    std::ostream& operator<<(std::ostream&, const Array&);


    // inline definitions

    inline Array::Array(Size size)
    : data_(size ? new Real[size] : (Real*)(0)), n_(size) {}

    inline Array::Array(Size size, Real value)
    : data_(size ? new Real[size] : (Real*)(0)), n_(size) {
        std::fill(begin(),end(),value);
    }

    inline Array::Array(Size size, Real value, Real increment)
    : data_(size ? new Real[size] : (Real*)(0)), n_(size) {
        for (iterator i=begin(); i!=end(); i++,value+=increment)
            *i = value;
    }

    inline Array::Array(const Array& from)
    : data_(from.n_ ? new Real[from.n_] : (Real*)(0)), n_(from.n_) {
        std::copy(from.begin(),from.end(),begin());
    }

    inline Array::Array(const Disposable<Array>& from)
    : data_((Real*)(0)), n_(0) {
        swap(const_cast<Disposable<Array>&>(from));
    }

    inline Array& Array::operator=(const Array& from) {
        // strong guarantee
        Array temp(from);
        swap(temp);
        return *this;
    }

    inline Array& Array::operator=(const Disposable<Array>& from) {
        swap(const_cast<Disposable<Array>&>(from));
        return *this;
    }

    inline const Array& Array::operator+=(const Array& v) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(n_ == v.n_,
                   "arrays with different sizes (" << n_ << ", "
                   << v.n_ << ") cannot be added");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        std::transform(begin(),end(),v.begin(),begin(),
                       std::plus<Real>());
        return *this;
    }


    inline const Array& Array::operator+=(Real x) {
        std::transform(begin(),end(),begin(),
                       std::bind2nd(std::plus<Real>(),x));
        return *this;
    }

    inline const Array& Array::operator-=(const Array& v) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(n_ == v.n_,
                   "arrays with different sizes (" << n_ << ", "
                   << v.n_ << ") cannot be subtracted");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        std::transform(begin(),end(),v.begin(),begin(),
                       std::minus<Real>());
        return *this;
    }

    inline const Array& Array::operator-=(Real x) {
        std::transform(begin(),end(),begin(),
                       std::bind2nd(std::minus<Real>(),x));
        return *this;
    }

    inline const Array& Array::operator*=(const Array& v) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(n_ == v.n_,
                   "arrays with different sizes (" << n_ << ", "
                   << v.n_ << ") cannot be multiplied");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        std::transform(begin(),end(),v.begin(),begin(),
                       std::multiplies<Real>());
        return *this;
    }

    inline const Array& Array::operator*=(Real x) {
        std::transform(begin(),end(),begin(),
                       std::bind2nd(std::multiplies<Real>(),x));
        return *this;
    }

    inline const Array& Array::operator/=(const Array& v) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(n_ == v.n_,
                   "arrays with different sizes (" << n_ << ", "
                   << v.n_ << ") cannot be divided");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        std::transform(begin(),end(),v.begin(),begin(),
                       std::divides<Real>());
        return *this;
    }

    inline const Array& Array::operator/=(Real x) {
        std::transform(begin(),end(),begin(),
                       std::bind2nd(std::divides<Real>(),x));
        return *this;
    }

    inline Real Array::operator[](Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<n_, "array access out of range");
        #endif
        return data_.get()[i];
    }

    inline Real Array::at(Size i) const {
        QL_REQUIRE(i<n_, "array access out of range");
        return data_.get()[i];
    }

    inline Real& Array::operator[](Size i) {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<n_, "array access out of range");
        #endif
        return data_.get()[i];
    }

    inline Real& Array::at(Size i) {
        QL_REQUIRE(i<n_, "array access out of range");
        return data_.get()[i];
    }

    inline Size Array::size() const {
        return n_;
    }

    inline bool Array::empty() const {
        return n_ == 0;
    }

    inline Array::const_iterator Array::begin() const {
        return data_.get();
    }

    inline Array::iterator Array::begin() {
        return data_.get();
    }

    inline Array::const_iterator Array::end() const {
        return data_.get()+n_;
    }

    inline Array::iterator Array::end() {
        return data_.get()+n_;
    }

    inline Array::const_reverse_iterator Array::rbegin() const {
        return const_reverse_iterator(end());
    }

    inline Array::reverse_iterator Array::rbegin() {
        return reverse_iterator(end());
    }

    inline Array::const_reverse_iterator Array::rend() const {
        return const_reverse_iterator(begin());
    }

    inline Array::reverse_iterator Array::rend() {
        return reverse_iterator(begin());
    }

    inline void Array::swap(Array& from) {
        using std::swap;
        data_.swap(from.data_);
        swap(n_,from.n_);
    }

    // dot product

    inline Real DotProduct(const Array& v1, const Array& v2) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(v1.size() == v2.size(),
                   "arrays with different sizes (" << v1.size() << ", "
                   << v2.size() << ") cannot be multiplied");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        return std::inner_product(v1.begin(),v1.end(),v2.begin(),0.0);
    }

    // overloaded operators

    // unary

    inline const Disposable<Array> operator+(const Array& v) {
        Array result = v;
        return result;
    }

    inline const Disposable<Array> operator-(const Array& v) {
        Array result(v.size());
        std::transform(v.begin(),v.end(),result.begin(),
                       std::negate<Real>());
        return result;
    }


    // binary operators

    inline const Disposable<Array> operator+(const Array& v1,
                                             const Array& v2) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(v1.size() == v2.size(),
                   "arrays with different sizes (" << v1.size() << ", "
                   << v2.size() << ") cannot be added");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                       std::plus<Real>());
        return result;
    }

    inline const Disposable<Array> operator+(const Array& v1, Real a) {
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),result.begin(),
                       std::bind2nd(std::plus<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator+(Real a, const Array& v2) {
        Array result(v2.size());
        std::transform(v2.begin(),v2.end(),result.begin(),
                       std::bind1st(std::plus<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator-(const Array& v1,
                                             const Array& v2) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(v1.size() == v2.size(),
                   "arrays with different sizes (" << v1.size() << ", "
                   << v2.size() << ") cannot be subtracted");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                       std::minus<Real>());
        return result;
    }

    inline const Disposable<Array> operator-(const Array& v1, Real a) {
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),result.begin(),
                       std::bind2nd(std::minus<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator-(Real a, const Array& v2) {
        Array result(v2.size());
        std::transform(v2.begin(),v2.end(),result.begin(),
                       std::bind1st(std::minus<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator*(const Array& v1,
                                             const Array& v2) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(v1.size() == v2.size(),
                   "arrays with different sizes (" << v1.size() << ", "
                   << v2.size() << ") cannot be multiplied");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                       std::multiplies<Real>());
        return result;
    }

    inline const Disposable<Array> operator*(const Array& v1, Real a) {
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),result.begin(),
                       std::bind2nd(std::multiplies<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator*(Real a, const Array& v2) {
        Array result(v2.size());
        std::transform(v2.begin(),v2.end(),result.begin(),
                       std::bind1st(std::multiplies<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator/(const Array& v1,
                                             const Array& v2) {
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(disable:4267)
        #endif
        QL_REQUIRE(v1.size() == v2.size(),
                   "arrays with different sizes (" << v1.size() << ", "
                   << v2.size() << ") cannot be divided");
        #if defined(QL_PATCH_MSVC71)
        #pragma warning(default:4267)
        #endif
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),v2.begin(),result.begin(),
                       std::divides<Real>());
        return result;
    }

    inline const Disposable<Array> operator/(const Array& v1, Real a) {
        Array result(v1.size());
        std::transform(v1.begin(),v1.end(),result.begin(),
                       std::bind2nd(std::divides<Real>(),a));
        return result;
    }

    inline const Disposable<Array> operator/(Real a, const Array& v2) {
        Array result(v2.size());
        std::transform(v2.begin(),v2.end(),result.begin(),
                       std::bind1st(std::divides<Real>(),a));
        return result;
    }

    // functions

    inline const Disposable<Array> Abs(const Array& v) {
        Array result(v.size());
        std::transform(v.begin(),v.end(),result.begin(),
                       std::ptr_fun<Real,Real>(std::fabs));
        return result;
    }

    inline const Disposable<Array> Sqrt(const Array& v) {
        Array result(v.size());
        std::transform(v.begin(),v.end(),result.begin(),
                       std::ptr_fun<Real,Real>(std::sqrt));
        return result;
    }

    inline const Disposable<Array> Log(const Array& v) {
        Array result(v.size());
        std::transform(v.begin(),v.end(),result.begin(),
                       std::ptr_fun<Real,Real>(std::log));
        return result;
    }

    inline const Disposable<Array> Exp(const Array& v) {
        Array result(v.size());
        std::transform(v.begin(),v.end(),result.begin(),
                       std::ptr_fun<Real,Real>(std::exp));
        return result;
    }

    inline void swap(Array& v, Array& w) {
        v.swap(w);
    }

    inline std::ostream& operator<<(std::ostream& out, const Array& a) {
        std::streamsize width = out.width();
        out << "[ ";
        for (Integer n=0; n<Integer(a.size())-1; ++n)
            out << std::setw(width) << a[n] << "; ";
        out << std::setw(width) << a[a.size()-1];
        out << " ]";
        return out;
    }

}


#endif
