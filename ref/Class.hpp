#ifndef REF_CLASS_HPP
#define REF_CLASS_HPP

#include <string>
#include <boost/cstdint.hpp>
#include <boost/mpl/joint_view.hpp>
#include <ref/mpl.hpp>
#include <ref/DescriptorsImpl.hpp>

namespace ref
{
    struct ModelClass
    {
        typedef EmptyList features_type;
        typedef EmptyList all_features_type;

        virtual ~ModelClass() {}
        virtual const ClassDescriptor * getClassDescriptor() const = 0;
    };

    template < class Impl,
               typename FeaturesList = EmptyList,
               class BaseClass = ModelClass >
    struct Class :
        public BaseClass,
        public InheritFromList< FeaturesList >
    {
        typedef BaseClass base_class;
        typedef FeaturesList features_type;
        typedef typename boost::mpl::joint_view<
                typename BaseClass::all_features_type,
                features_type
            >::type all_features_type;

        // Attributes

        template < typename Feature >
        const typename Feature::type& get() const
        {
            return Feature::value;
        }

        template < typename Feature >
        typename Feature::type& get()
        {
            return Feature::value;
        }

        template < typename Feature, typename T >
        void set(T t)
        {
            Feature::value = t;
        }

        const ClassDescriptor * getClassDescriptor() const
        {
            return ClassDescriptorImpl< Class >::instance();
        }
    };

    template < typename T >
    struct Feature
    {
        typedef Feature< T> feature_type;
        typedef T type;
        T value;

        Feature() : value() {}

        template < typename Y >
        Feature(Y y) : value(y) {}
    };

    typedef Feature< int8_t > Int8;
    typedef Feature< uint8_t > UInt8;
    typedef Feature< int16_t > Int16;
    typedef Feature< uint16_t > UInt16;
    typedef Feature< int32_t > Int32;
    typedef Feature< uint32_t > UInt32;
    typedef Feature< int64_t > Int64;
    typedef Feature< uint64_t > UInt64;
    typedef Feature< bool > Bool;
    typedef Feature< std::string > String;

    template < typename... FeatureTypes >
    using Features = boost::mpl::vector< FeatureTypes... >;

} // namespace ref

#endif // REF_CLASS_HPP
