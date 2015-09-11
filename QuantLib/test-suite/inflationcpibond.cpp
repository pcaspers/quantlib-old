/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2011 Chris Kenyon
 Copyright (C) 2012 StatPro Italia srl
 
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

#include "inflationcpibond.hpp"
#include "utilities.hpp"
#include <ql/indexes/inflation/ukrpi.hpp>
#include <ql/time/calendars/unitedkingdom.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/indexes/ibor/gbplibor.hpp>
#include <ql/termstructures/inflation/inflationhelpers.hpp>
#include <ql/termstructures/inflation/piecewisezeroinflationcurve.hpp>
#include <ql/cashflows/indexedcashflow.hpp>
#include <ql/pricingengines/swap/discountingswapengine.hpp>
#include <ql/instruments/zerocouponinflationswap.hpp>
#include <ql/pricingengines/bond/discountingbondengine.hpp>
#include <ql/cashflows/cpicoupon.hpp>
#include <ql/cashflows/cpicouponpricer.hpp>
#include <ql/instruments/cpiswap.hpp>
#include <ql/instruments/bonds/cpibond.hpp>

using namespace QuantLib;
using namespace boost::unit_test_framework;

#include <iostream>

namespace {

    struct Datum {
        Date date;
        Rate rate;
    };

    typedef BootstrapHelper<ZeroInflationTermStructure> Helper;

    std::vector<boost::shared_ptr<Helper> > makeHelpers(
        Datum iiData[], Size N,
        const boost::shared_ptr<ZeroInflationIndex>& ii,
        const Period& observationLag,
        const Calendar& calendar,
        const BusinessDayConvention& bdc,
        const DayCounter& dc) {

        std::vector<boost::shared_ptr<Helper> > instruments;
        for (Size i=0; i<N; i++) {
            Date maturity = iiData[i].date;
            Handle<Quote> quote(boost::shared_ptr<Quote>(
                                new SimpleQuote(iiData[i].rate/100.0)));
            boost::shared_ptr<Helper> h(
                      new ZeroCouponInflationSwapHelper(quote, observationLag,
                                                        maturity, calendar,
                                                        bdc, dc, ii));
            instruments.push_back(h);
        }
        return instruments;
    }


    struct CommonVars {

        Calendar calendar;
        BusinessDayConvention convention;
        Date evaluationDate;
        Period observationLag;
        DayCounter dayCounter;

        boost::shared_ptr<UKRPI> ii;

        RelinkableHandle<YieldTermStructure> yTS;
        RelinkableHandle<ZeroInflationTermStructure> cpiTS;

        // cleanup

        SavedSettings backup;
        IndexHistoryCleaner cleaner;

        CommonVars() { }

        // setup
        static CommonVars UKVars() {
            CommonVars uk;
            // usual setup
            uk.calendar = UnitedKingdom();
            uk.convention = ModifiedFollowing;
            Date today(25, November, 2009);
            uk.evaluationDate = uk.calendar.adjust(today);
            Settings::instance().evaluationDate() = uk.evaluationDate;
            uk.dayCounter = ActualActual();

            Date from(20, July, 2007);
            Date to(20, November, 2009);
            Schedule rpiSchedule =
                MakeSchedule().from(from).to(to)
                .withTenor(1*Months)
                .withCalendar(UnitedKingdom())
                .withConvention(ModifiedFollowing);

            bool interp = false;
            uk.ii = boost::shared_ptr<UKRPI>(new UKRPI(interp, uk.cpiTS));

            Real fixData[] = {
                206.1, 207.3, 208.0, 208.9, 209.7, 210.9,
                209.8, 211.4, 212.1, 214.0, 215.1, 216.8,
                216.5, 217.2, 218.4, 217.7, 216,
                212.9, 210.1, 211.4, 211.3, 211.5,
                212.8, 213.4, 213.4, 213.4, 214.4
            };
            for (Size i=0; i<LENGTH(fixData); ++i) {
                uk.ii->addFixing(rpiSchedule[i], fixData[i]);
            }

            uk.yTS.linkTo(boost::shared_ptr<YieldTermStructure>(
                          new FlatForward(uk.evaluationDate, 0.05, uk.dayCounter)));

            // now build the zero inflation curve
            uk.observationLag = Period(2,Months);

            Datum zciisData[] = {
                { Date(25, November, 2010), 3.0495 },
                { Date(25, November, 2011), 2.93 },
                { Date(26, November, 2012), 2.9795 },
                { Date(25, November, 2013), 3.029 },
                { Date(25, November, 2014), 3.1425 },
                { Date(25, November, 2015), 3.211 },
                { Date(25, November, 2016), 3.2675 },
                { Date(25, November, 2017), 3.3625 },
                { Date(25, November, 2018), 3.405 },
                { Date(25, November, 2019), 3.48 },
                { Date(25, November, 2021), 3.576 },
                { Date(25, November, 2024), 3.649 },
                { Date(26, November, 2029), 3.751 },
                { Date(27, November, 2034), 3.77225 },
                { Date(25, November, 2039), 3.77 },
                { Date(25, November, 2049), 3.734 },
                { Date(25, November, 2059), 3.714 },
            };

            std::vector<boost::shared_ptr<Helper> > helpers =
                makeHelpers(zciisData, LENGTH(zciisData), uk.ii,
                    uk.observationLag, uk.calendar, uk.convention, uk.dayCounter);

            Rate baseZeroRate = zciisData[0].rate/100.0;
            uk.cpiTS.linkTo(boost::shared_ptr<ZeroInflationTermStructure>(
                  new PiecewiseZeroInflationCurve<Linear>(
                      uk.evaluationDate, uk.calendar, uk.dayCounter, uk.observationLag,
                      uk.ii->frequency(), uk.ii->interpolated(), baseZeroRate,
                         Handle<YieldTermStructure>(uk.yTS), helpers)));

            return uk;
        }

        // teardown
        ~CommonVars() {
            // break circular references and allow curves to be destroyed
            cpiTS.linkTo(boost::shared_ptr<ZeroInflationTermStructure>());
        }
    };

}


void InflationCPIBondTest::testCleanPrice() {
    CommonVars common = CommonVars::UKVars();

    Real notional = 1000000.0;
    std::vector<Rate> fixedRates(1, 0.1);
    DayCounter fixedDayCount = Actual365Fixed();
    BusinessDayConvention fixedPaymentConvention = ModifiedFollowing;
    Calendar fixedPaymentCalendar = UnitedKingdom();
    boost::shared_ptr<ZeroInflationIndex> fixedIndex = common.ii;
    Period contractObservationLag = Period(3,Months);
    CPI::InterpolationType observationInterpolation = CPI::Flat;
    Natural settlementDays = 3;
    bool growthOnly = true;

    Real baseCPI = 206.1;
    // set the schedules
    Date startDate(2, October, 2007);
    Date endDate(2, October, 2052);
    Schedule fixedSchedule =
        MakeSchedule().from(startDate).to(endDate)
                      .withTenor(Period(6,Months))
                      .withCalendar(UnitedKingdom())
                      .withConvention(Unadjusted)
                      .backwards();

    CPIBond bond(settlementDays, notional, growthOnly,
                 baseCPI, contractObservationLag, fixedIndex,
                 observationInterpolation, fixedSchedule,
                 fixedRates, fixedDayCount, fixedPaymentConvention);

    boost::shared_ptr<DiscountingBondEngine> engine(
                                 new DiscountingBondEngine(common.yTS));
    bond.setPricingEngine(engine);

    Real storedPrice = 383.01816406;
    Real calculated = bond.cleanPrice();
    Real tolerance = 1.0e-8;
    if (std::fabs(calculated-storedPrice) > tolerance) {
        BOOST_FAIL("failed to reproduce expected CPI-bond clean price"
                   << QL_FIXED << std::setprecision(12)
                   << "\n  expected:   " << storedPrice
                   << "\n  calculated: " << calculated);
    }
}


test_suite* InflationCPIBondTest::suite() {
    test_suite* suite = BOOST_TEST_SUITE("CPI bond tests");

    suite->add(QUANTLIB_TEST_CASE(&InflationCPIBondTest::testCleanPrice));

    return suite;
}

