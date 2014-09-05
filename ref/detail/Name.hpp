#ifndef REF_DETAIL_NAME_HPP
#define REF_DETAIL_NAME_HPP

#include <cstdlib>
#include <boost/units/detail/utility.hpp>

namespace ref
{
namespace detail
{
    template < typename T >
    std::string get_fqn()
    {
        return boost::units::detail::demangle(typeid(T).name());
    }

    template < typename T >
    std::string get_name()
    {
        const std::string res = get_fqn< T >();
        size_t pos = res.rfind(':');
        return pos == std::string::npos? res : res.substr(pos + 1);
    }
} // namespace detail
} // namespace ref

#endif // REF_DETAIL_NAME_HPP
