#ifndef REF_MPL_HPP
#define REF_MPL_HPP

#include <boost/mpl/vector.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/utility.hpp>

namespace ref
{
    typedef boost::mpl::vector<> EmptyList;

    template < typename T, typename Enabled = void >
    struct InheritFromList :
        public boost::mpl::front< T >::type,
        public InheritFromList< typename boost::mpl::pop_front< T >::type >
    {};

    template < typename T >
    struct InheritFromList< T,
        typename boost::enable_if< typename boost::mpl::empty< T >::type >::type >
    {};
} // namespace ref

#endif // REF_MPL_HPP
