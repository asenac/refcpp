#include "ReferenceResolver.hpp"
#include <ref/Class.hpp>
#include <ref/Holder.hpp>
#include <cassert>

using namespace ref;

struct DefaultReferenceResolver::Impl
{
    Impl(const StructuralContext& ctx) : m_ctx(ctx) {}

    const StructuralContext& m_ctx;
};

DefaultReferenceResolver::DefaultReferenceResolver(const StructuralContext& ctx)
    : m_impl(new Impl(ctx))
{
}

DefaultReferenceResolver::~DefaultReferenceResolver() { delete m_impl; }

std::string DefaultReferenceResolver::getIdForObject(ModelClass* obj)
{
    assert(obj);

    static const std::string defaultIds[] = {"Id", "Name"};
    const ClassDescriptor* desc = obj->getClassDescriptor();

    for (const auto& id : defaultIds)
    {
        const FeatureDescriptor* f = desc->getFeatureDescriptor(id);
        if (!f) continue;

        const TypeDescriptor* t = f->getTypeDescriptor();
        if (t->getKind() != TypeDescriptor::kPrimitive) continue;

        Holder h = f->getValue(obj);
        return t->as<PrimitiveTypeDescriptor>()->getString(h);
    }

    // Use first primtive feature
    const auto features = desc->getAllFeatureDescriptors();

    for (const auto& f: features)
    {
        const TypeDescriptor* t = f->getTypeDescriptor();

        if (t->getKind() == TypeDescriptor::kPrimitive)
        {
            Holder h = f->getValue(obj);
            return t->as<PrimitiveTypeDescriptor>()->getString(h);
        }
    }

    return std::string();
}

ModelClass* DefaultReferenceResolver::resolveId(
    ModelClass* fromObject, const ClassDescriptor* referencedClassDesc,
    const std::string& id)
{
    // XXX use shortest path from fromObject's class descriptor to
    // referencedClassDesc
    return NULL;
}
