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
            kClass, kPrimitive, kList, kMap, kSet, kPair, kUnsupported
        };

        virtual Kind getKind() const = 0;

        /**
         * @brief Creates an instance of the type associated with this
         * descriptor and returns it inside a holder.
         *
         * @return A valid holder for supported types. An invalid holder
         * for unsupported types and abstract classes.
         */
        virtual Holder create() const = 0;

        /**
         * @brief Copies the value contained in src into dst.
         *
         * Does nothing for unsupported types. Both holders must
         * contain an instance of the type associated with this descriptor.
         *
         * @param src A valid holder.
         * @param dst A valid holder.
         */
        virtual void copy(Holder src, Holder dst) const = 0;

        /**
         * @brief Returns a concrete instance of an implementation
         * for the given type.
         *
         * @tparam T A type.
         *
         * @return A non-null descriptor.
         */
        template < typename T >
        static const TypeDescriptor * getDescriptor();
    };

    struct UnsupportedTypeDescriptor : TypeDescriptor
    {
        Kind getKind() const { return kUnsupported; }
    };

    /**
     * @brief Interface for descriptors assocated with primitive types.
     *
     * @todo Add get and set method for every primitive type supported.
     * @todo Add PrimitiveKind enum and getPrimitiveKind method.
     */
    struct PrimitiveTypeDescriptor : TypeDescriptor
    {
        Kind getKind() const { return kPrimitive; }

        /**
         * @brief Converts the value contained in a holder into a string
         * using boost::lexical_cast.
         *
         * @param h A holder containing a pointer to an instance of
         * the type associated with this descriptor.
         *
         * @return A string.
         */
        virtual std::string getString(Holder h) const = 0;

        virtual void setString(Holder h, std::string value) const = 0;
    };

    struct FeatureDescriptor : Descriptor
    {
        /**
         * @brief Returns the type descriptor for the type associated
         * with the feature.
         *
         * @return A non-null descriptor.
         */
        virtual const TypeDescriptor * getTypeDescriptor() const = 0;

        /**
         * @brief Returns the value of the structural feature.
         *
         * @param obj A valid instance.
         *
         * @return A holder object that contains a pointer to the
         * value of the feature.
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

        /**
         * @brief Returns the list of features defined in this class.
         *
         * @return A vector of feature descriptors.
         */
        virtual FeatureDescriptorVector getFeatureDescriptors() const = 0;

        /**
         * @brief Returns the list with all features of this class,
         * including those defined in parent classes.
         *
         * @return A vector of feature descriptors.
         */
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

        /**
         * @brief Returns the object contained in the passed holder.
         *
         * @param h A holder containing an instance of the class associated
         * to this descriptor.
         *
         * @return A pointer to an instance of a ModelClass' sub-class.
         */
        virtual ModelClass * get(Holder h) const = 0;

        /**
         * @brief Converts an object into a list of pairs.
         *
         * @param obj A non-null pointer to an instance of the class
         * associated to this descriptor.
         *
         * @return A vector of pairs.
         */
        virtual FeatureValueVector getFeatureValues(ModelClass * obj) const = 0;

        Kind getKind() const { return kClass; }
    };

    struct ContainerTypeDescriptor : TypeDescriptor
    {
        /**
         * @brief Returns the descriptor for the type of the elements within
         * a list of the associated type.
         *
         * @return A non-null descriptor.
         */
        virtual const TypeDescriptor * getValueTypeDescriptor() const = 0;

        /**
         * @brief Converts a list of the type associated type to this
         * descriptor into a list of holders containing pointers to
         * the elements of the original list.
         *
         * @param h A holder containing a list of the associated type to
         * this descriptor.
         *
         * @return A vector of holders.
         */
        virtual std::vector< Holder > getValue(Holder h) const = 0;
    };

    struct ListTypeDescriptor : ContainerTypeDescriptor
    {
        virtual void setValue(Holder h, std::vector< Holder > value) const = 0;

        Kind getKind() const { return kList; }
    };

    struct SetTypeDescriptor : ContainerTypeDescriptor
    {
        Kind getKind() const { return kSet; }
    };

    struct PairTypeDescriptor : TypeDescriptor
    {
        Kind getKind() const { return kPair; }
    };

    struct MapTypeDescriptor : TypeDescriptor
    {
        virtual const TypeDescriptor * getKeyTypeDescriptor() const = 0;

        virtual const TypeDescriptor * getMappedTypeDescriptor() const = 0;

        virtual const TypeDescriptor * getValueTypeDescriptor() const = 0;

        virtual std::vector< Holder > getValue(Holder h) const = 0;

        Kind getKind() const { return kMap; }
    };

} // namespace ref

#endif // REF_DESCRIPTORS_HPP
