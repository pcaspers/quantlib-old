/*
 Copyright (C) 2001, 2002 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file treeswaption.hpp
    \brief Swaption calculated using a tree

    \fullpath
    ql/Pricers/%treeswaption.hpp
*/

// $Id$

#ifndef quantlib_pricers_tree_swaption_h
#define quantlib_pricers_tree_swaption_h

#include <ql/Instruments/swaption.hpp>
#include <ql/Lattices/tree.hpp>

namespace QuantLib {

    namespace Pricers {

        //! Swaption priced in a tree
        class TreeSwaption : public SwaptionPricingEngine {
          public:
            TreeSwaption() {}

            TreeSwaption(Size timeSteps) : timeSteps_(timeSteps) {}
            TreeSwaption(const Handle<Lattices::Tree>& tree) 
            : tree_(tree), timeSteps_(0) {}

            void calculate() const;
          private:
            Handle<Lattices::Tree> tree_;
            Size timeSteps_;
        };

    }

}

#endif
