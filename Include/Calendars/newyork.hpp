
/*
 * Copyright (C) 2000
 * Ferdinando Ametrano, Luigi Ballabio, Adolfo Benin, Marco Marchioro
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, contact ferdinando@ametrano.net
 *
 * QuantLib license is also available at 
 * http://quantlib.sourceforge.net/LICENSE.TXT
*/

/*! \file newyork.hpp
    \brief New York calendar

    $Source$
    $Log$
    Revision 1.2  2001/04/04 12:13:22  nando
    Headers policy part 2:
    The Include directory is added to the compiler's include search path.
    Then both your code and user code specifies the sub-directory in
    #include directives, as in
    #include <Solvers1d/newton.hpp>

    Revision 1.1  2001/04/04 11:07:21  nando
    Headers policy part 1:
    Headers should have a .hpp (lowercase) filename extension
    All *.h renamed to *.hpp

    Revision 1.5  2001/03/26 08:38:28  lballabio
    fixed line wrappings

    Revision 1.4  2001/01/17 14:37:55  nando
    tabs removed

    Revision 1.3  2000/12/14 12:32:29  lballabio
    Added CVS tags in Doxygen file documentation blocks

*/

#ifndef quantlib_newyork_calendar_h
#define quantlib_newyork_calendar_h

#include "qldefines.hpp"
#include "Calendars/westerncalendar.hpp"

namespace QuantLib {

    namespace Calendars {

        //! New York calendar
        /*! Holidays:
            <ul>
            <li>Saturdays</li>
            <li>Sundays</li>
            <li>New Year's Day, January 1st (possibly moved to Monday if 
                actually on Sunday, or to Friday if on Saturday)</li>
            <li>Martin Luther King's birthday, third Monday in January</li>
            <li>Washington's birthday, third Monday in February</li>
            <li>Memorial Day, last Monday in May</li>
            <li>Independence Day, July 4th (moved to Monday if Sunday or Friday 
                if Saturday)</li>
            <li>Labor Day, first Monday in September</li>
            <li>Columbus Day, second Monday in October</li>
            <li>Veteran's Day, November 11th (moved to Monday if Sunday or 
                Friday if Saturday)</li>
            <li>Thanksgiving Day, fourth Thursday in November</li>
            <li>Christmas, December 25th (moved to Monday if Sunday or Friday if 
                Saturday)</li>
            </ul>
        */
        class NewYork : public WesternCalendar {
          public:
            NewYork() {}
            std::string name() const { return "NewYork"; }
            bool isBusinessDay(const Date&) const;
        };

    }

}


#endif
