#ifndef REF_DESCRIPTORS_HPP
#define REF_DESCRIPTORS_HPP

#include <vector>
#include <string>
#include <map>

namespace ref
{
    struct ModelClass;
    struct Holder;
    struct ClassDescriptor;

    struct Descriptor
    {
        virtual std::string getName() const = 0;

        virtual std::string getFqn() const = 0;
    };

    struct TypeDescriptor : Descriptor
    {
        enum Kind
        {
            kClass, kPrimitive, kList, kMap, kUnsupported
        };

        virtual Kind getKind() const = 0;

        virtual Holder create() const = 0;

        virtual void copy(Holder src, Holder dst) const = 0;
    };

    struct UnsupportedTypeDescriptor : TypeDescriptor
    {
        Kind getKind() const { return kUnsupported; }
    };

    struct PrimitiveTypeDescriptor : TypeDescriptor
    {
        Kind getKind() const { return kPrimitive; }

        virtual std::string getString(Holder h) const = 0;

        virtual void setString(Holder h, std::string value) const = 0;
    };

    struct FeatureDescriptor : Descriptor
    {
        /**
         * @brief
         *
         * @return The type descriptor for the type of the value of the
         * feature.
         */
        virtual const TypeDescriptor * getTypeDescriptor() const = 0;

        /**
         * @brief
         *
         * @param obj
         *
         * @return The value of the structural feature.
         */
        virtual Holder getValue(ModelClass * obj) const = 0;

        /**
         * @brief Returns the class descriptor for the class this feature
         * is defined in.
         *
         * @return A class descriptor.
         */
        virtual const ClassDescriptor * getDefinedIn() const = 0;
    };

    typedef std::vector< const FeatureDescriptor * > FeatureDescriptorVector;
    typedef std::vector< std::pair< const FeatureDescriptor *, Holder > > FeatureValueVector;

    struct ClassDescriptor : TypeDescriptor
    {
        /**
         * @brief Returns the descriptor of the parent class.
         *
         * @return A class descriptor.
         */
        virtual const ClassDescriptor * getParentClassDescriptor() const = 0;

        virtual FeatureDescriptorVector getFeatureDescriptors() const = 0;

        virtual FeatureDescriptorVector getAllFeatureDescriptors() const = 0;

        virtual bool isAbstract() const = 0;

        /**
         * @brief Find feature descriptor by its name.
         *
         * @param name Name of feature to search descriptor for.
         *
         * @return A pointer to the feature descriptor if found. Otherwise, a null pointer.
         */
        virtual const FeatureDescriptor * getFeatureDescriptor(std::string name) const = 0;

        virtual ModelClass * get(Holder h) const = 0;

        virtual FeatureValueVector getFeatureValues(ModelClass * obj) const = 0;

        Kind getKind() const { return kClass; }
    };

    struct ListTypeDescriptor : TypeDescriptor
    {
        virtual const TypeDescriptor * getValueTypeDescriptor() const = 0;

        virtual std::vector< Holder > getValue(Holder h) const = 0;

        virtual void setValue(Holder h, std::vector< Holder > value) const = 0;

        Kind getKind() const { return kList; }
    };

    struct MapTypeDescriptor : TypeDescriptor
    {
        Kind getKind() const { return kMap; }
    };

} // namespace ref

#endif // REF_DESCRIPTORS_HPP
