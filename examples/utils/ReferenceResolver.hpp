#ifndef REFCPP_REFERENCE_RESOLVER_HPP
#define REFCPP_REFERENCE_RESOLVER_HPP

#include <string>

namespace ref
{
    struct ModelClass;
    struct ClassDescriptor;
    struct StructuralContext;

    struct ReferenceResolver
    {
        virtual std::string getIdForObject(ModelClass* obj) = 0;

        virtual ModelClass* resolveId(
            ModelClass* fromObject,
            const ClassDescriptor* referencedClassDesc,
            const std::string& id) = 0;
    };

    struct DefaultReferenceResolver : ReferenceResolver
    {
        DefaultReferenceResolver(const StructuralContext& ctx);
        DefaultReferenceResolver(const DefaultReferenceResolver&) = delete;
        virtual ~DefaultReferenceResolver();

        std::string getIdForObject(ModelClass* obj) override;

        ModelClass* resolveId(ModelClass* fromObject,
                              const ClassDescriptor* referencedClassDesc,
                              const std::string& id) override;
    protected:
        struct Impl;
        Impl * m_impl;
    };
}  // namespace ref

#endif  // REFCPP_REFERENCE_RESOLVER_HPP
