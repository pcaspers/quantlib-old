# configure gcc, with open ad
./configure --with-boost-include=/home/peter/boost_1_59_0 --with-boost-lib=/home/peter/boost_1_59_0/stage/lib --enable-error-lines --enable-indexed-coupons CXXFLAGS="-m64 -O3 -g -Wall -Wno-unused-local-typedefs -Wno-unknown-pragmas -Wno-deprecated-declarations -std=c++11" LDFLAGS="-L/home/peter/quantlib/QuantLibOAD/lgm" LIBS="-llgmad -lntl -lgmp -lm -lboost_timer -lboost_chrono -lboost_system"