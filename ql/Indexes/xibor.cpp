
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/Indexes/xibor.hpp>
#include <ql/Indexes/xibormanager.hpp>

namespace QuantLib {

    std::string Xibor::name() const {
        std::string tenor;
        switch (units_) {
          case Days:
            tenor = IntegerFormatter::toString(n_)+"d";
            break;
          case Weeks:
            tenor = IntegerFormatter::toString(n_)+"w";
            break;
          case Months:
            tenor = IntegerFormatter::toString(n_)+"m";
            break;
          case Years:
            tenor = IntegerFormatter::toString(n_)+"y";
            break;
          default:
            QL_FAIL("invalid time unit");
        }
        return familyName_+tenor+" "+dayCounter_.name();
    }

    Frequency Xibor::frequency() const {
        switch (units_) {
          case Months:
            QL_REQUIRE(12%n_ == 0, "undefined frequency");
            return Frequency(12/n_);
          case Years:
            QL_REQUIRE(n_ == 1, "undefined frequency");
            return Annual;
          default:
            QL_FAIL("undefined frequency");
        }
        QL_DUMMY_RETURN(Once)
    }

    Rate Xibor::fixing(const Date& fixingDate) const {
        QL_REQUIRE(!termStructure_.isNull(),
                   "null term structure set");
        Date today = termStructure_->todaysDate();
        if (fixingDate < today) {
            // must have been fixed
            Rate pastFixing =
                XiborManager::getHistory(name())[fixingDate];
            QL_REQUIRE(pastFixing != Null<Real>(),
                       "Missing " + name() + " fixing for " +
                       DateFormatter::toString(fixingDate));
            return pastFixing;
        }
        if (fixingDate == today) {
            // might have been fixed
            try {
                Rate pastFixing =
                    XiborManager::getHistory(name())[fixingDate];
                if (pastFixing != Null<Real>())
                    return pastFixing;
                else
                    ;   // fall through and forecast
            } catch (Error&) {
                ;       // fall through and forecast
            }
        }
        Date fixingValueDate = calendar_.advance(fixingDate,
                                                 settlementDays_,Days);
        Date endValueDate = calendar_.advance(fixingValueDate,n_,units_,
                                              rollingConvention_);
        DiscountFactor fixingDiscount =
            termStructure_->discount(fixingValueDate);
        DiscountFactor endDiscount =
            termStructure_->discount(endValueDate);
        Time fixingPeriod =
            dayCounter_.yearFraction(fixingValueDate, endValueDate);
        return (fixingDiscount/endDiscount-1.0) / fixingPeriod;
    }

}

