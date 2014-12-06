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

        std::string getName() const override;

        std::string getFqn() const override;

        std::string getXmlTag() const override;
    };

    template <typename Class>
    struct ClassDescriptorImpl
        : DescriptorImplBase<ClassDescriptor, ClassDescriptorImpl<Class>, Class>
    {
        ClassDescriptorImpl();

        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        const ClassDescriptor* getParentClassDescriptor() const override;

        FeatureDescriptorVector getFeatureDescriptors() const override;

        FeatureDescriptorVector getAllFeatureDescriptors() const override;

        bool isAbstract() const override;

        const FeatureDescriptor* getFeatureDescriptor(
            std::string name) const override;

        ModelClass* get(Holder h) const override;

        FeatureValueVector getFeatureValues(ModelClass* obj) const override;

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
        const TypeDescriptor* getTypeDescriptor() const override;

        Holder getValue(ModelClass* obj) const override;

        ModelClass * getObject(Holder h) const override;

        const ClassDescriptor* getDefinedIn() const override;
    };

    template <typename T>
    struct PrimitiveTypeDescriptorImpl
        : DescriptorImplBase<PrimitiveTypeDescriptor,
                             PrimitiveTypeDescriptorImpl<T>, T>
    {
        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        std::string getString(Holder h) const override;

        void setString(Holder h, const std::string& value) const override;
    };

    template <typename T>
    struct ListTypeDescriptorImpl
        : DescriptorImplBase<ListTypeDescriptor, ListTypeDescriptorImpl<T>, T>
    {
        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        const TypeDescriptor* getValueTypeDescriptor() const override;

        std::vector<Holder> getValue(Holder h) const override;

        void setValue(Holder h,
                      const std::vector<Holder>& value) const override;
    };

    template <typename T>
    struct SetTypeDescriptorImpl
        : DescriptorImplBase<SetTypeDescriptor, SetTypeDescriptorImpl<T>, T>
    {
        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        const TypeDescriptor* getValueTypeDescriptor() const override;

        std::vector<Holder> getValue(Holder h) const override;

        void setValue(Holder h,
                      const std::vector<Holder>& value) const override;
    };

    template <typename T>
    struct MapTypeDescriptorImpl
        : DescriptorImplBase<MapTypeDescriptor, MapTypeDescriptorImpl<T>, T>
    {
        typedef std::pair<typename T::key_type, typename T::mapped_type>
            value_type;

        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        const TypeDescriptor* getKeyTypeDescriptor() const override;

        const TypeDescriptor* getMappedTypeDescriptor() const override;

        const TypeDescriptor* getValueTypeDescriptor() const override;

        std::vector<Holder> getValue(Holder h) const override;

        void setValue(Holder h,
                      const std::vector<Holder>& value) const override;
    };

    template <typename T>
    struct PairTypeDescriptorImpl
        : DescriptorImplBase<PairTypeDescriptor, PairTypeDescriptorImpl<T>, T>
    {
        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        const TypeDescriptor* getFirstTypeDescriptor() const override;

        const TypeDescriptor* getSecondTypeDescriptor() const override;

        std::pair<Holder, Holder> getValue(Holder h) const override;
    };

    template <typename T>
    struct PointerTypeDescriptorImpl
        : DescriptorImplBase<PointerTypeDescriptor,
                             PointerTypeDescriptorImpl<T>, T>
    {
        Holder create() const override;

        void copy(Holder src, Holder dst) const override;

        PointerTypeDescriptor::PointerType getPointerType() const override;

        const TypeDescriptor* getPointedTypeDescriptor() const override;

        bool isNull(Holder h) const override;

        Holder dereference(Holder h) const override;
    };

    template <typename T>
    struct UnsupportedTypeDescriptorImpl
        : DescriptorImplBase<UnsupportedTypeDescriptor,
                             UnsupportedTypeDescriptorImpl<T>, T>
    {
        Holder create() const override;

        void copy(Holder src, Holder dst) const override;
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
