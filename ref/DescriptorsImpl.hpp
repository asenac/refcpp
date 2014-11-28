#ifndef REF_DESCRIPTORS_IMPL_HPP
#define REF_DESCRIPTORS_IMPL_HPP

#include <map>
#include <set>
#include <memory>
#include <ref/Descriptors.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility.hpp>

namespace ref
{
    template <typename Descriptor, typename Impl, typename T>
    struct DescriptorImplBase : Descriptor
    {
        static const Descriptor* instance();

        std::string getName() const;

        std::string getFqn() const;
    };

    template <typename Class>
    struct ClassDescriptorImpl
        : DescriptorImplBase<ClassDescriptor, ClassDescriptorImpl<Class>, Class>
    {
        ClassDescriptorImpl();

        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const ClassDescriptor* getParentClassDescriptor() const;

        FeatureDescriptorVector getFeatureDescriptors() const;

        FeatureDescriptorVector getAllFeatureDescriptors() const;

        bool isAbstract() const;

        const FeatureDescriptor* getFeatureDescriptor(std::string name) const;

        ModelClass* get(Holder h) const;

        FeatureValueVector getFeatureValues(ModelClass* obj) const;

        static const ClassDescriptorImpl* instance();

    protected:
        struct Initializer;

        typedef std::map<std::string, const FeatureDescriptor*>
            FeatureDescriptorMap;

        FeatureDescriptorVector m_featureVec;
        FeatureDescriptorVector m_allFeatureVec;
        FeatureDescriptorMap m_featureMap;
    };

    template <typename Class, typename Feature>
    struct FeatureDescriptorImpl
        : DescriptorImplBase<FeatureDescriptor,
                             FeatureDescriptorImpl<Class, Feature>, Feature>
    {
        const TypeDescriptor* getTypeDescriptor() const;

        Holder getValue(ModelClass* obj) const;

        const ClassDescriptor* getDefinedIn() const;
    };

    template <typename T>
    struct PrimitiveTypeDescriptorImpl
        : DescriptorImplBase<PrimitiveTypeDescriptor,
                             PrimitiveTypeDescriptorImpl<T>, T>
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        std::string getString(Holder h) const;

        void setString(Holder h, std::string value) const;
    };

    template <typename T>
    struct ListTypeDescriptorImpl
        : DescriptorImplBase<ListTypeDescriptor, ListTypeDescriptorImpl<T>, T>
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const TypeDescriptor* getValueTypeDescriptor() const;

        std::vector<Holder> getValue(Holder h) const;

        void setValue(Holder h, std::vector<Holder> value) const;
    };

    template <typename T>
    struct SetTypeDescriptorImpl
        : DescriptorImplBase<SetTypeDescriptor, SetTypeDescriptorImpl<T>, T>
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const TypeDescriptor* getValueTypeDescriptor() const;

        std::vector<Holder> getValue(Holder h) const;

        void setValue(Holder h, std::vector<Holder> value) const;
    };

    template <typename T>
    struct MapTypeDescriptorImpl
        : DescriptorImplBase<MapTypeDescriptor, MapTypeDescriptorImpl<T>, T>
    {
        typedef std::pair<typename T::key_type, typename T::mapped_type>
            value_type;

        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const TypeDescriptor* getKeyTypeDescriptor() const;

        const TypeDescriptor* getMappedTypeDescriptor() const;

        const TypeDescriptor* getValueTypeDescriptor() const;

        std::vector<Holder> getValue(Holder h) const;

        void setValue(Holder h, std::vector<Holder> value) const;
    };

    template <typename T>
    struct PairTypeDescriptorImpl
        : DescriptorImplBase<PairTypeDescriptor, PairTypeDescriptorImpl<T>, T>
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        const TypeDescriptor* getFirstTypeDescriptor() const;

        const TypeDescriptor* getSecondTypeDescriptor() const;

        std::pair<Holder, Holder> getValue(Holder h) const;
    };

    template <typename T>
    struct PointerTypeDescriptorImpl
        : DescriptorImplBase<PointerTypeDescriptor,
                             PointerTypeDescriptorImpl<T>, T>
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;

        PointerTypeDescriptor::PointerType getPointerType() const;

        const TypeDescriptor* getPointedTypeDescriptor() const;

        bool isNull(Holder h) const;

        Holder dereference(Holder h) const;
    };

    template <typename T>
    struct UnsupportedTypeDescriptorImpl
        : DescriptorImplBase<UnsupportedTypeDescriptor,
                             UnsupportedTypeDescriptorImpl<T>, T>
    {
        Holder create() const;

        void copy(Holder src, Holder dst) const;
    };

    namespace detail
    {
        template <typename T, typename Enabled = void>
        struct GetDescriptorType
        {
            typedef UnsupportedTypeDescriptorImpl<T> type;
        };

        template <typename T>
        struct GetDescriptorType<
            T, typename boost::enable_if<
                   typename boost::is_arithmetic<T>::type>::type>
        {
            typedef PrimitiveTypeDescriptorImpl<T> type;
        };

        template <>
        struct GetDescriptorType<std::string>
        {
            typedef PrimitiveTypeDescriptorImpl<std::string> type;
        };

        template <typename T>
        struct GetDescriptorType<
            T, typename boost::enable_if<
                   typename boost::is_base_of<ModelClass, T>::type>::type>
        {
            typedef ClassDescriptorImpl<T> type;
        };

        template <typename K, typename T>
        struct GetDescriptorType<std::map<K, T> >
        {
            typedef MapTypeDescriptorImpl<std::map<K, T> > type;
        };

        template <typename K, typename T>
        struct GetDescriptorType<std::pair<K, T> >
        {
            typedef PairTypeDescriptorImpl<std::pair<K, T> > type;
        };

        template <typename T>
        struct GetDescriptorType<std::vector<T> >
        {
            typedef ListTypeDescriptorImpl<std::vector<T> > type;
        };

        template <typename T>
        struct GetDescriptorType<std::set<T> >
        {
            typedef SetTypeDescriptorImpl<std::set<T> > type;
        };

        template <typename T>
        struct GetDescriptorType<T*>
        {
            typedef PointerTypeDescriptorImpl<T*> type;
        };

        template <typename T>
        struct GetDescriptorType<std::shared_ptr<T> >
        {
            typedef PointerTypeDescriptorImpl<std::shared_ptr<T> > type;
        };

        template <typename T>
        struct GetDescriptorType<std::unique_ptr<T> >
        {
            typedef PointerTypeDescriptorImpl<std::unique_ptr<T> > type;
        };

        template <typename T>
        struct GetDescriptorType<std::weak_ptr<T> >
        {
            typedef PointerTypeDescriptorImpl<std::weak_ptr<T> > type;
        };
    }  // namespace detail

}  // namespace ref

#endif  // REF_DESCRIPTORS_IMPL_HPP
