#ifndef REFCPP_STRUCTURAL_CONTEXT_HPP
#define REFCPP_STRUCTURAL_CONTEXT_HPP

#include <vector>

namespace ref
{
    struct ClassDescriptor;
    struct FeatureDescriptor;

    struct Reference
    {
        enum ReferenceType
        {
            kContained, kRaw, kOwned, kSharedOwned, kWeak
        };

        ReferenceType type;
        const ClassDescriptor * classDesc;
        const FeatureDescriptor * featureDesc;
        const ClassDescriptor * referencedClassDesc;
    };

    struct StructuralContext
    {
        StructuralContext(const ClassDescriptor * rootClassDesc);
        ~StructuralContext();

        const ClassDescriptor * getRootClassDescriptor() const;

        std::vector< const ClassDescriptor * > getAllClasses() const;

        std::vector< Reference > getIncomingReferences(const ClassDescriptor * classDesc) const;

        std::vector< Reference > getOutgoingReferences(const ClassDescriptor * classDesc) const;

    protected:
        struct Impl;
        Impl * m_impl;
    };
} // namespace ref

#endif // REFCPP_STRUCTURAL_CONTEXT_HPP
