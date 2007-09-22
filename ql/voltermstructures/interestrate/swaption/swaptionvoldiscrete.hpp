/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file swaptionvoldiscrete.hpp
    \brief Discretized swaption volatility
*/

#ifndef quantlib_swaption_volatility_discrete_h
#define quantlib_swaption_volatility_discrete_h

#include <ql/voltermstructures/interestrate/swaption/swaptionvolstructure.hpp>
#include <ql/math/interpolation.hpp>
#include <ql/patterns/lazyobject.hpp>

namespace QuantLib {

    class SwaptionVolatilityDiscrete : public SwaptionVolatilityStructure,
                                       public LazyObject   {
      public:
        SwaptionVolatilityDiscrete(const std::vector<Period>& optionTenors,
                                   const std::vector<Period>& swapTenors,
                                   Natural settlementDays,
                                   const Calendar& cal,
                                   const DayCounter& dc,
                                   BusinessDayConvention bdc = Following);
        SwaptionVolatilityDiscrete(const std::vector<Period>& optionTenors,
                                   const std::vector<Period>& swapTenors,
                                   const Date& referenceDate,
                                   const Calendar& cal,
                                   const DayCounter& dc,
                                   BusinessDayConvention bdc = Following);
        SwaptionVolatilityDiscrete(const std::vector<Date>& optionDates,
                                   const std::vector<Period>& swapTenors,
                                   const Date& referenceDate,
                                   const Calendar& cal,
                                   const DayCounter& dc,
                                   BusinessDayConvention bdc = Following);
        const std::vector<Period>& optionTenors() const;
        const std::vector<Date>& optionDates() const;
        const std::vector<Time>& optionTimes() const;
        const std::vector<Period>& swapTenors() const;
        const std::vector<Time>& swapLengths() const;
        //! implements the conversion between dates and times
        std::pair<Time,Time> convertDates(const Date& optionDates,
                                          const Period& swapTenor) const;
        //@}
        //! \name LazyObject interface
        //@{
        void update();
        void performCalculations() const;
        //@}
      protected:
        void checkOptionTenors() const;
        void checkOptionDates() const;
        void checkSwapTenors() const;
        void initializeOptionTimes() const;
        void initializeOptionDatesAndTimes() const;

        Size nOptionTenors_;
        std::vector<Period> optionTenors_;
        mutable std::vector<Date> optionDates_;
        mutable std::vector<Time> optionTimes_;
        mutable std::vector<Real> optionDatesAsReal_;
        Interpolation optionInterpolator_;

        Size nSwapTenors_;
        std::vector<Period> swapTenors_;
        mutable std::vector<Time> swapLengths_;
        Date evaluationDate_;
    };

    // inline

    inline const std::vector<Period>&
    SwaptionVolatilityDiscrete::optionTenors() const {
         return optionTenors_;
    }

    inline const std::vector<Date>&
    SwaptionVolatilityDiscrete::optionDates() const {
        return optionDates_;
    }

    inline const std::vector<Time>&
    SwaptionVolatilityDiscrete::optionTimes() const {
        return optionTimes_;
    }

    inline const std::vector<Period>&
    SwaptionVolatilityDiscrete::swapTenors() const {
     return swapTenors_;
    }

    inline const std::vector<Time>&
    SwaptionVolatilityDiscrete::swapLengths() const {
        return swapLengths_;
    }

}

#endif
