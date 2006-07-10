/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003, 2004 Ferdinando Ametrano

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

/*! \file pseudosqrt.hpp
    \brief pseudo square root of a real symmetric matrix
*/

#ifndef quantlib_pseudo_sqrt_hpp
#define quantlib_pseudo_sqrt_hpp

#include <ql/Math/matrix.hpp>

namespace QuantLib {

    //! algorithm used for matricial pseudo square root
    struct SalvagingAlgorithm {
        enum Type {None, Spectral};
    };

    //! Returns the pseudo square root of a real symmetric matrix
    /*! Given a matrix \f$ M \f$, the result \f$ S \f$ is defined
        as the matrix such that \f$ S S^T = M. \f$
        If the matrix is not positive semi definite, it can
        return an approximation of the pseudo square root
        using a (user selected) salvaging algorithm.

        For more information see: "The most general methodology to create
        a valid correlation matrix for risk management and option pricing
        purposes", by R. Rebonato and P. J�ckel.
        The Journal of Risk, 2(2), Winter 1999/2000
        http://www.rebonato.com/correlationmatrix.pdf

        Revised and extended in "Monte Carlo Methods in Finance",
        by Peter J�ckel, Chapter 6.

        \pre the given matrix must be symmetric.

        \relates Matrix

        \todo
        - implement Hypersphere decomposition:
              -# J�ckel "Monte Carlo Methods in Finance", Chapter 6
              -# Brigo "A Note on Correlation and Rank Reduction"
              -# Rapisarda, Brigo, Mercurio "Parameterizing correlations:
                 a geometric interpretation"
        - implement Higham algorithm:
          Higham "Computing the nearest correlation matrix"

        \test
        - the correctness of the results is tested by reproducing
          known good data.
        - the correctness of the results is tested by checking
          returned values against numerical calculations.
    */
    const Disposable<Matrix> pseudoSqrt(
                        const Matrix&,
                        SalvagingAlgorithm::Type = SalvagingAlgorithm::None);

    //! Returns the rank-reduced pseudo square root of a real symmetric matrix
    /*! The result matrix has rank<=maxRank. If maxRank>=size, then the
        specified percentage of eigenvalues out of the eigenvalues' sum is
        retained.

        If the input matrix is not positive semi definite, it can return an
        approximation of the pseudo square root using a (user selected) 
        salvaging algorithm.
    
        \pre the given matrix must be symmetric.

        \relates Matrix
    */
    const Disposable<Matrix> rankReducedSqrt(const Matrix&,
                                            Size maxRank,
                                            Real componentRetainedPercentage,
                                            SalvagingAlgorithm::Type);

}


#endif
