#ifndef REFCPP_STRUCTURAL_CONTEXT_HPP
#define REFCPP_STRUCTURAL_CONTEXT_HPP

#include <vector>

namespace ref
{
    struct ClassDescriptor;
    struct FeatureDescriptor;

    struct StructuralContext
    {
        StructuralContext(const ClassDescriptor * rootClassDesc);
        ~StructuralContext();

        const ClassDescriptor * getRootClassDescriptor() const;

        std::vector< const ClassDescriptor * > getAllClasses() const;

    protected:
        struct Impl;
        Impl * m_impl;
    };
} // namespace ref

#endif // REFCPP_STRUCTURAL_CONTEXT_HPP
