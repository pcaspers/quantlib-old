/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
  Copyright (C) 2014 Peter Caspers

  This file is part of QuantLib, a free-software/open-source library
  for financial quantitative analysts and developers - http://quantlib.org/

  QuantLib is free software: you can redistribute it and/or modify it
  under the terms of the QuantLib license.  You should have received a
  copy of the license along with this program; if not, please email
  <quantlib-dev@lists.sf.net>. The license is also available online at
  <http://quantlib.org/license.shtml>.


  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

/*! \file cmsspreadpricer.cpp
*/

#include <boost/make_shared.hpp>

#include <ql/cashflows/cmsspreadpricer.hpp>
#include <ql/cashflows/cmsspreadcoupon.hpp>
#include <ql/math/integrals/kronrodintegral.hpp>

namespace QuantLib {

    CmsSpreadPricer::CmsSpreadPricer(
        const boost::shared_ptr<CmsCouponPricer> cmsPricer,
        const Handle<Quote> &correlation,
        const Handle<YieldTermStructure> &couponDiscountCurve,
        const Size integrationPoints)
        : CmsSpreadCouponPricer(correlation), cmsPricer_(cmsPricer),
          couponDiscountCurve_(couponDiscountCurve) {

        registerWith(correlation);
        if (!couponDiscountCurve_.empty())
            registerWith(couponDiscountCurve_);

        QL_REQUIRE(integrationPoints >= 4,
                   "at least 4 integration points should be used ("
                   << integrationPoints << ")");
        integrator_ =
            boost::make_shared<GaussHermiteIntegration>(integrationPoints);

        cnd_ = boost::make_shared<CumulativeNormalDistribution>(0.0, 1.0);

        privateObserver_ = boost::make_shared<PrivateObserver>(this);
        privateObserver_->registerWith(cmsPricer_);
    }

    const Real CmsSpreadPricer::integrand(const Real x) const {

        // this is Brigo, 13.16.2 with x = v/sqrt(2)

        Real v = M_SQRT2 * x;
        Real h =
            k_ - b_ * s2_ *
                          std::exp((m2_ - 0.5 * v2_ * v2_) * fixingTime_ +
                                   v2_ * sqrt(fixingTime_) * v);
        Real phi1, phi2;
        phi1 = cnd_->operator()(
            phi_ * (std::log(a_ * s1_ / h) +
                    (m1_ + (0.5 - rho_ * rho_) * v1_ * v1_) * fixingTime_ +
                    rho_ * v1_ * std::sqrt(fixingTime_) * v) /
            (v1_ * sqrt(fixingTime_ * (1.0 - rho_ * rho_))));
        phi2 = cnd_->operator()(
            phi_ * (std::log(a_ * s1_ / h) +
                    (m1_ - 0.5 * v1_ * v1_) * fixingTime_ +
                    rho_ * v1_ * std::sqrt(fixingTime_) * v) /
            (v1_ * sqrt(fixingTime_ * (1.0 - rho_ * rho_))));
        Real f = a_ * phi_ * s1_ *
                     std::exp(m1_ * fixingTime_ -
                              0.5 * rho_ * rho_ * v1_ * v1_ * fixingTime_ +
                              rho_ * v1_ * sqrt(fixingTime_) * v) *
                     phi1 -
                 phi_ * h * phi2;
        return 1.0 / M_SQRTPI * std::exp(-x * x) * f;

   }

    void CmsSpreadPricer::flushCache() {
        cache_.clear();
    }

    void CmsSpreadPricer::initialize(const FloatingRateCoupon &coupon) {

        coupon_ = dynamic_cast<const CmsSpreadCoupon *>(&coupon);
        QL_REQUIRE(coupon_, "CMS spread coupon needed");
        index_ = coupon_->swapSpreadIndex();
        gearing_ = coupon_->gearing();
        spread_ = coupon_->spread();

        fixingDate_ = coupon_->fixingDate();
        paymentDate_ = coupon_->date();

        // if no coupon discount curve is given just use the discounting curve
        // from the _first_ swap index.
        // for rate calculation this curve cancels out in the computation, so
        // e.g. the discounting
        // swap engine will produce correct results, even if the
        // couponDiscountCurve is not set here.
        // only the price member function in this class will be dependent on the
        // coupon discount curve.

        today_ = QuantLib::Settings::instance().evaluationDate();

        if(couponDiscountCurve_.empty())
            couponDiscountCurve_ = index_->swapIndex1()->exogenousDiscount() ?
                index_->swapIndex1()->discountingTermStructure() :
                index_->swapIndex1()->forwardingTermStructure();

        spreadLegValue_ = spread_ * coupon_->accrualPeriod() *
            couponDiscountCurve_->discount(paymentDate_);

        if (fixingDate_ > today_) {

            gearing1_ = index_->gearing1();
            gearing2_ = index_->gearing2();

            QL_REQUIRE(gearing1_ > 0.0 && gearing2_ < 0.0,
                       "gearing1 (" << gearing1_
                                    << ") should be positive while gearing2 ("
                                    << gearing2_ << ") should be negative");

            fixingTime_ =
                cmsPricer_->swaptionVolatility()->timeFromReference(fixingDate_);

            c1_ = boost::shared_ptr<CmsCoupon>(new CmsCoupon(
                coupon_->date(), coupon_->nominal(),
                coupon_->accrualStartDate(), coupon_->accrualEndDate(),
                coupon_->fixingDays(), index_->swapIndex1(), 1.0, 0.0,
                coupon_->referencePeriodStart(), coupon_->referencePeriodEnd(),
                coupon_->dayCounter(), coupon_->isInArrears()));

            c2_ = boost::shared_ptr<CmsCoupon>(new CmsCoupon(
                coupon_->date(), coupon_->nominal(),
                coupon_->accrualStartDate(), coupon_->accrualEndDate(),
                coupon_->fixingDays(), index_->swapIndex2(), 1.0, 0.0,
                coupon_->referencePeriodStart(), coupon_->referencePeriodEnd(),
                coupon_->dayCounter(), coupon_->isInArrears()));

            c1_->setPricer(cmsPricer_);
            c2_->setPricer(cmsPricer_);

            swapRate1_ = c1_->indexFixing();
            swapRate2_ = c2_->indexFixing();

            // costly part, look up in cache first
            std::pair<std::string,Date> key = std::make_pair(index_->name(), fixingDate_);
            CacheType::const_iterator k = cache_.find(key);
            if(k != cache_.end()) {
                adjustedRate1_ = k->second.first;
                adjustedRate2_ = k->second.second;
            }
            else {
                adjustedRate1_ = c1_->adjustedFixing();
                adjustedRate2_ = c2_->adjustedFixing();
                cache_.insert(std::make_pair(key,std::make_pair(adjustedRate1_,adjustedRate2_)));
            }

            vol1_ = cmsPricer_->swaptionVolatility()->volatility(fixingDate_, index_->swapIndex1()->tenor(), swapRate1_);
            vol2_ = cmsPricer_->swaptionVolatility()->volatility(fixingDate_, index_->swapIndex2()->tenor(), swapRate2_);

            mu1_ = 1.0 / fixingTime_ * std::log(adjustedRate1_ / swapRate1_);
            mu2_ = 1.0 / fixingTime_ * std::log(adjustedRate2_ / swapRate2_);

            rho_ = std::max(std::min(correlation()->value(),0.9999),-0.9999); // avoid division by zero in integrand

        }
    }

    Real CmsSpreadPricer::optionletPrice(Option::Type optionType,
                                         Real strike) const {

        phi_ = optionType == Option::Call ? 1.0 : -1.0;
        Real res = 0.0;
        if(strike >= 0.0) {
            a_ = gearing1_;
            b_ = gearing2_;
            s1_ = swapRate1_;
            s2_ = swapRate2_;
            m1_ = mu1_;
            m2_ = mu2_;
            v1_ = vol1_;
            v2_ = vol2_;
            k_ = strike;
        }
        else {
            a_ = -gearing2_;
            b_ = -gearing1_;
            s1_ = swapRate2_;
            s2_ = swapRate1_;
            m1_ = mu2_;
            m2_ = mu1_;
            v1_ = vol2_;
            v2_ = vol1_;
            k_ = -strike;
            res += phi_ * (swapletRate() - strike);
        }

        res += integrator_->operator()(std::bind1st(std::mem_fun(&CmsSpreadPricer::integrand),this));
        return res * couponDiscountCurve_->discount(paymentDate_) * coupon_->accrualPeriod();

    }

    Rate CmsSpreadPricer::swapletRate() const {
        return swapletPrice() /
               (coupon_->accrualPeriod() *
                couponDiscountCurve_->discount(paymentDate_));
    }

    Real CmsSpreadPricer::capletPrice(Rate effectiveCap) const {
        // caplet is equivalent to call option on fixing
        if (fixingDate_ <= today_) {
            // the fixing is determined
            const Rate Rs = std::max(
                coupon_->index()->fixing(fixingDate_) - effectiveCap, 0.);
            Rate price =
                (gearing_ * Rs) *
                (coupon_->accrualPeriod() *
                 couponDiscountCurve_->discount(paymentDate_));
            return price;
        } else {
            Real capletPrice = optionletPrice(Option::Call, effectiveCap);
            return gearing_ * capletPrice;
        }
    }

    Rate CmsSpreadPricer::capletRate(Rate effectiveCap) const {
        return capletPrice(effectiveCap) /
               (coupon_->accrualPeriod() *
                couponDiscountCurve_->discount(paymentDate_));
    }

    Real CmsSpreadPricer::floorletPrice(Rate effectiveFloor) const {
        // floorlet is equivalent to put option on fixing
        if (fixingDate_ <= today_) {
            // the fixing is determined
            const Rate Rs = std::max(
                effectiveFloor - coupon_->index()->fixing(fixingDate_), 0.);
            Rate price =
                (gearing_ * Rs) *
                (coupon_->accrualPeriod() *
                 couponDiscountCurve_->discount(paymentDate_));
            return price;
        } else {
            Real floorletPrice = optionletPrice(Option::Put, effectiveFloor);
            return gearing_ * floorletPrice;
        }
    }

    Rate CmsSpreadPricer::floorletRate(Rate effectiveFloor) const {
        return floorletPrice(effectiveFloor) /
               (coupon_->accrualPeriod() *
                couponDiscountCurve_->discount(paymentDate_));
    }

    Real CmsSpreadPricer::swapletPrice() const {

        return gearing_ * coupon_->accrualPeriod() *
                   couponDiscountCurve_->discount(paymentDate_) *
                   (gearing1_ * c1_->rate() + gearing2_ * c2_->rate()) +
               spreadLegValue_;
    }
}