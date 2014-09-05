#ifndef REF_HOLDER_HPP
#define REF_HOLDER_HPP

#include <memory>

namespace ref
{
    struct TypeDescriptor;

    struct Holder
    {
        /**
         * @brief Creates an invalid holder.
         */
        Holder() : m_descriptor(nullptr) {}

        Holder(const Holder& h)
            : m_impl(h.m_impl)
            , m_descriptor(h.m_descriptor)
        {}

        template < typename T >
        Holder(T * t, const TypeDescriptor * descriptor,
                bool release = false)
            : m_impl(new Impl< T >(t, release))
            , m_descriptor(descriptor)
        {}

        template < typename T >
        T * get()
        {
            return static_cast< Impl< T > * >(m_impl.get())->t;
        }

        const TypeDescriptor * descriptor() const
        {
            return m_descriptor;
        }

        bool isValid() const
        {
            return !!m_impl;
        }

    protected:
        struct ImplBase {};
        template < typename T >
        struct Impl : ImplBase
        {
            T * t;
            bool release;
            Impl(T* t_, bool release_)
                : t(t_), release(release_)
            {}
            ~Impl()
            {
                if (release)
                    delete t;
            }
        };

        std::shared_ptr< ImplBase > m_impl;
        const TypeDescriptor * m_descriptor;
    };

} // namespace ref

#endif // REF_HOLDER_HPP
