#ifndef REF_DESCRIPTORS_IMPL_HPP
#define REF_DESCRIPTORS_IMPL_HPP

#include <map>
#include <ref/Descriptors.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility.hpp>

namespace ref
{
    template < typename Descriptor, typename Impl, typename T >
    struct DescriptorImplBase : Descriptor
    {
        static const Descriptor * instance();

        std::string getName() const;

        std::string getFqn() const;
    };

    template < typename Class >
    struct ClassDescriptorImpl :
        DescriptorImplBase < ClassDescriptor,
                             ClassDescriptorImpl < Class >,
                             Class >
    {
        ClassDescriptorImpl();

        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const ClassDescriptor * getParentClassDescriptor() const;

        FeatureDescriptorVector getFeatureDescriptors() const;

        FeatureDescriptorVector getAllFeatureDescriptors() const;

        bool isAbstract() const;

        const FeatureDescriptor * getFeatureDescriptor(std::string name) const;

        ModelClass * get(Holder h) const;

        FeatureValueVector getFeatureValues(ModelClass * obj) const;

    protected:

        struct Initializer;

        typedef std::map< std::string, const FeatureDescriptor * >
            FeatureDescriptorMap;

        FeatureDescriptorVector m_featureVec;
        FeatureDescriptorVector m_allFeatureVec;
        FeatureDescriptorMap m_featureMap;
    };

    template < typename Class, typename Feature >
    struct FeatureDescriptorImpl :
        DescriptorImplBase < FeatureDescriptor,
                             FeatureDescriptorImpl < Class, Feature >,
                             Feature >
    {
        const TypeDescriptor * getTypeDescriptor() const;

        Holder getValue(ModelClass * obj) const;

        const ClassDescriptor * getDefinedIn() const;
    };

    template < typename T >
    struct PrimitiveTypeDescriptorImpl :
        DescriptorImplBase < PrimitiveTypeDescriptor,
                             PrimitiveTypeDescriptorImpl < T >, T >
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        std::string getString(Holder h) const;

        void setString(Holder h, std::string value) const;
    };

    template < typename T >
    struct ListTypeDescriptorImpl :
        DescriptorImplBase < ListTypeDescriptor,
                             ListTypeDescriptorImpl < T >, T >
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const TypeDescriptor * getValueTypeDescriptor() const;

        std::vector< Holder > getValue(Holder h) const;

        void setValue(Holder h, std::vector< Holder > value) const;
    };

    template < typename T >
    struct MapTypeDescriptorImpl :
        DescriptorImplBase < MapTypeDescriptor,
                             MapTypeDescriptorImpl < T >, T >
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;
    };

    template < typename T >
    struct UnsupportedTypeDescriptorImpl :
        DescriptorImplBase < UnsupportedTypeDescriptor,
                             UnsupportedTypeDescriptorImpl < T >, T >
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;
    };

    namespace detail
    {
        template < typename T, typename Enabled = void >
        struct GetDescriptorType
        {
            typedef UnsupportedTypeDescriptorImpl< T > type;
        };

        template < typename T >
        struct GetDescriptorType< T,
            typename boost::enable_if< typename boost::is_arithmetic< T >::type >::type >
        {
            typedef PrimitiveTypeDescriptorImpl< T > type;
        };

        template < >
        struct GetDescriptorType< std::string >
        {
            typedef PrimitiveTypeDescriptorImpl< std::string > type;
        };

        template < typename T >
        struct GetDescriptorType< T,
            typename boost::enable_if< typename boost::is_base_of< ModelClass, T >::type >::type >
        {
            typedef ClassDescriptorImpl< T > type;
        };

        template < typename K, typename T >
        struct GetDescriptorType< std::map< K, T > >
        {
            typedef MapTypeDescriptorImpl< std::map< K, T > > type;
        };

        template < typename T >
        struct GetDescriptorType< std::vector< T > >
        {
            typedef ListTypeDescriptorImpl< std::vector< T > > type;
        };
    } // namespace detail

} // namespace ref

#endif // REF_DESCRIPTORS_IMPL_HPP
