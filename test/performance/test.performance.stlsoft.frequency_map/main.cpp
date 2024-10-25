/* /////////////////////////////////////////////////////////////////////////
 * File:    test.performance.stlsoft.frequency_map/main.cpp
 *
 * Purpose: Perf-test for `stlsoft::frequency_map<>`.
 *
 * Created: 5th October 2024
 * Updated: 15th October 2024
 *
 * ////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////
 * compatibility
 */


/* /////////////////////////////////////////////////////////////////////////
 * feature control
 */

#include <stlsoft/containers/frequency_map.hpp>

#include <platformstl/diagnostics/stopwatch.hpp>

#if __cplusplus >= 201103L
# include <list>
#endif
#include <iomanip>
#include <iostream>
#include <numeric>

#include <stdlib.h>


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

typedef platformstl::stopwatch::interval_type               interval_t;
using stlsoft::ss_size_t;

typedef stlsoft::frequency_map<int>                         fm_ordered_int_t;
#if __cplusplus >= 201103L

typedef stlsoft::frequency_map<
    int
,   stlsoft::frequency_map_traits_unordered<int>
>                                                           fm_unordered_int_t;
#endif /* C++ */


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

namespace {

    ss_size_t const NUM_ITERATIONS = 1000000;
} // anonymous namespace


/* /////////////////////////////////////////////////////////////////////////
 * functions
 */

template<
    ss_typename_param_k T_fm
>
std::pair<
    interval_t  // total_time_ns
,   ss_size_t   // anchoring_value - this to incline the optimiser to not elide the whole thing
>
test_(
    ss_size_t                   num_iterations
,   std::function<void (
        T_fm&       fm1
    ,   T_fm&       fm2
    )>                          init_fn
,   std::function<ss_size_t (
        ss_size_t   iteration
    ,   T_fm&       fm1
    ,   T_fm&       fm2
    )>                          timed_fn
)
{
    platformstl::stopwatch  sw;
    interval_t              interval        =   0;
    ss_size_t               anchoring_value =   0;

    for (int W = 2; W != 0; --W)
    {
        T_fm fm1;
        T_fm fm2;

        if (init_fn)
        {
            init_fn(fm1, fm2);
        }

        sw.start();

        for (ss_size_t i = 0; i != num_iterations; ++i)
        {
            anchoring_value += timed_fn(i, fm1, fm2);
        }

        sw.stop();

        interval = sw.get_nanoseconds();
    }

    return std::make_pair(interval, anchoring_value);
}

void
display_results(
    std::ostream&       stm
,   ss_size_t           num_iterations
,   char const*         ordering_label
,   char const*         test_name
,   std::pair<
        interval_t  // total_time_ns
    ,   ss_size_t   // anchoring_value
    > const&            r
)
{
    stm
        << '\t'
        << ordering_label << ": " << test_name
        << '\t'
        << num_iterations
        << '\t'
        << std::setw(12) << std::right << r.first
        << '\t'
        << std::setw(12) << std::setfill(' ') << std::right << std::fixed << std::setprecision(3) << (static_cast<double>(r.first) / num_iterations)
        << '\t'
        << r.second
        << std::endl;
}

template<
    ss_typename_param_k T_fm
>
void
run_tests(
    char const*         ordering_label
)
{
    platformstl::stopwatch sw;

    // ctor default
    {
        char const* const   TEST_NAME   =   "ctor (default)";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& /*fm1*/, T_fm& /*fm2*/) {

            T_fm fm;

            return iteration;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // ctor initializer_list<int>
    {
        char const* const   TEST_NAME   =   "ctor (initializer_list<int>)";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& /*fm1*/, T_fm& /*fm2*/) {

            T_fm fm = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, };

            return iteration + fm.size();
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // ctor initializer_list<std::pair<int, uintptr_t>>
    {
        char const* const   TEST_NAME   =   "ctor (initializer_list<<int, uintptr_t>>)";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& /*fm1*/, T_fm& /*fm2*/) {

            T_fm fm = { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 }, { 10, 1 }, { 11, 1 }, { 12, 1 }, { 13, 1 }, { 14, 1 }, { 15, 1 }, { 16, 1 }, { 17, 1 }, { 18, 1 }, { 19, 1 }, };

            return iteration + fm.size();
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // ctor iterator range
    {
        char const* const   TEST_NAME   =   "ctor (int const*, int const*)";

        const int VALUES[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, };

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [&VALUES](ss_size_t iteration, T_fm& /*fm1*/, T_fm& /*fm2*/) {

#if __cplusplus >= 201103L

            T_fm fm = { std::begin(VALUES), std::end(VALUES) };
#else

            T_fm fm = { &VALUES[0] + 0, &VALUES[0] + STLSOFT_NUM_ELEMENTS(VALUES) };
#endif

            return iteration + fm.size();
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

#if __cplusplus >= 201103L

    // ctor iterator range (FI)
    {
        char const* const   TEST_NAME   =   "ctor (FI, FI)";

        std::list<int> const VALUES = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, };

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [&VALUES](ss_size_t iteration, T_fm& /*fm1*/, T_fm& /*fm2*/) {

            T_fm fm(VALUES.begin(), VALUES.end());

            return iteration + fm.size();
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }
#endif

    // clear
    {
        char const* const   TEST_NAME   =   "clear()";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& /* fm2 */) {

            for (size_t i = 0; i != 1000000; ++i)
            {
                fm1.push_n(i % 100000, i % 100);
            }
        }
        ,   [](ss_size_t /* iteration */, T_fm& fm1, T_fm& /* fm2 */) {

            auto const n = fm1.size();

            fm1.clear();

            return n;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // push (restricted)
    {
        char const* const   TEST_NAME   =   "push() - restricted";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int const key = static_cast<int>(iteration & 0xfff0);

            fm1.push(key);

            return iteration;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // push (unrestricted)
    {
        char const* const   TEST_NAME   =   "push() - unrestricted";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int const key = static_cast<int>(iteration);

            fm1.push(key);

            return iteration;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // push_n (restricted)
    {
        char const* const   TEST_NAME   =   "push() - restricted";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int const       key     =   static_cast<int>(iteration & 0xfff0);
            ss_size_t const count   =   iteration & 0x1000f;

            fm1.push_n(key, count);

            return iteration;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // push_n (unrestricted)
    {
        char const* const   TEST_NAME   =   "push() - unrestricted";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int const       key     =   static_cast<int>(iteration);
            ss_size_t const count   =   iteration & 0x1000f;

            fm1.push_n(key, count);

            return iteration;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // contains
    {
        char const* const   TEST_NAME   =   "contains()";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& /* fm2 */) {

            for (size_t i = 0; i != 1000000; ++i)
            {
                fm1.push_n(i % 100000, i % 100);
            }
        }
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int const i = static_cast<int>(iteration);

            bool const b = fm1.contains(i);

            return b;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // total
    {
        char const* const   TEST_NAME   =   "total()";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& /* fm2 */) {

            for (size_t i = 0; i != 1000000; ++i)
            {
                fm1.push_n(i % 100000, i % 100);
            }
        }
        ,   [](ss_size_t /* iteration */, T_fm& fm1, T_fm& /* fm2 */) {

            return fm1.total();
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // operator []
    {
        char const* const   TEST_NAME   =   "operator []()";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& /* fm2 */) {

            for (size_t i = 0; i != 1000000; ++i)
            {
                fm1.push_n(i % 100000, i % 100);
            }
        }
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int const i = static_cast<int>(iteration);

            bool const b = 0 != fm1[i];

            return b;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // merge (empty)
    {
        char const* const   TEST_NAME   =   "merge() - empty";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   nullptr
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& fm2) {

            T_fm fm(fm1);

            fm.merge(fm2);

            return iteration % 10000;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // merge (small)
    {
        char const* const   TEST_NAME   =   "merge() - small";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& fm2) {

            for (size_t i = 0; i != 1000; ++i)
            {
                fm1.push_n(i & 0xe, i % 100);
                fm2.push_n(i & 0x7, i % 100);
            }
        }
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& fm2) {

            T_fm fm(fm1);

            fm.merge(fm2);

            return iteration % 10000;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // merge (medium)
    {
        char const* const   TEST_NAME   =   "merge() - medium";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& fm2) {

            for (size_t i = 0; i != 1000; ++i)
            {
                fm1.push_n(i & 0xfc, i % 100);
                fm2.push_n(i & 0x0f, i % 100);
            }
        }
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& fm2) {

            T_fm fm(fm1);

            fm.merge(fm2);

            return iteration % 10000;
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // accumulate (small)
    {
        char const* const   TEST_NAME   =   "accumulate() - small";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& /* fm2 */) {

            for (size_t i = 0; i != 100; ++i)
            {
                int const n = static_cast<int>(i);

                fm1.push(n);
            }
        }
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int total = 0;

            for (ss_typename_type_k T_fm::const_iterator i = fm1.cbegin(); fm1.cend() != i; ++i)
            {
                total += (*i).first;
            }

            return iteration + static_cast<ss_size_t>(total);
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }

    // accumulate (medium)
    {
        char const* const   TEST_NAME   =   "accumulate() - medium";

        std::pair<
            interval_t  // total_time_ns
        ,   ss_size_t   // anchoring_value
        > const r = test_<T_fm>(
            NUM_ITERATIONS
        ,   [](T_fm& fm1, T_fm& /* fm2 */) {

            for (size_t i = 0; i != 1000; ++i)
            {
                int const n = static_cast<int>(i);

                fm1.push(n);
            }
        }
        ,   [](ss_size_t iteration, T_fm& fm1, T_fm& /* fm2 */) {

            int total = 0;

            for (ss_typename_type_k T_fm::const_iterator i = fm1.cbegin(); fm1.cend() != i; ++i)
            {
                total += (*i).first;
            }

            return iteration + static_cast<ss_size_t>(total);
        });

        display_results(std::cout, NUM_ITERATIONS, ordering_label, TEST_NAME, r);
    }
}


/* /////////////////////////////////////////////////////////////////////////
 * main()
 */

int main(int /* argc */, char* /* argv */[])
{
    std::cout << std::endl;
    run_tests<fm_ordered_int_t>("O");
#if __cplusplus >= 201103L
    std::cout << std::endl;
    run_tests<fm_unordered_int_t>("U");
#endif

    return EXIT_SUCCESS;
}


/* ///////////////////////////// end of file //////////////////////////// */
