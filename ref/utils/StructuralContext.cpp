#include <ref/Descriptors.hpp>
#include "StructuralContext.hpp"
#include <unordered_map>
#include <set>
#include <list>
#include <stdexcept>

using namespace ref;

namespace
{
    struct IterationItem
    {
        Reference::ReferenceType referenceType;
        const TypeDescriptor* desc;

        IterationItem() : referenceType(Reference::kContained), desc() {}
    };

    Reference::ReferenceType getReferenceType(
        PointerTypeDescriptor::PointerType type)
    {
        switch (type)
        {
            case PointerTypeDescriptor::kUnique:
                return Reference::kOwned;
            case PointerTypeDescriptor::kShared:
                return Reference::kSharedOwned;
            case PointerTypeDescriptor::kWeak:
                return Reference::kWeak;
            default:
                break;
        }

        return Reference::kRaw;
    }
}  // namespace

struct StructuralContext::Impl
{
    typedef const ClassDescriptor* ClassDesc;
    typedef std::vector<Reference> ReferenceVector;

    ClassDesc m_rootClassDesc;

    struct ClassInfo
    {
        std::set<ClassDesc> subclasses;
        std::set<ClassDesc> allSubclasses;
        // incoming references
        ReferenceVector inReferences;
        ReferenceVector allInReferences;
        // outgoing references
        ReferenceVector outReferences;
        ReferenceVector allOutReferences;
    };

    typedef std::unordered_map<ClassDesc, ClassInfo> ClassInfoMap;
    ClassInfoMap m_classInfoMap;
    std::vector<const ClassDescriptor*> m_allClasses;

    Impl(const ClassDescriptor* rootClassDesc);

    const ClassInfo& getClassInfo(ClassDesc classDesc) const
    {
        ClassInfoMap::const_iterator it = m_classInfoMap.find(classDesc);

        if (it == m_classInfoMap.end())
        {
            throw std::runtime_error("Invalid class");
        }

        return it->second;
    }
};

StructuralContext::Impl::Impl(const ClassDescriptor* rootClassDesc)
    : m_rootClassDesc(rootClassDesc)
{
    std::list<ClassDesc> pending;
    std::set<ClassDesc> processed;
    pending.push_back(rootClassDesc);

    while (!pending.empty())
    {
        ClassDesc current = pending.front();
        pending.pop_front();

        if (processed.count(current))
        {
            continue;
        }

        // Class hierachy
        ClassDesc parent = current->getParentClassDescriptor();
        if (parent)
        {
            if (!processed.count(parent))
            {
                pending.push_back(parent);
            }

            m_classInfoMap[parent].subclasses.insert(current);

            ClassDesc predecessor = current;
            while ((predecessor = predecessor->getParentClassDescriptor()))
            {
                m_classInfoMap[predecessor].allSubclasses.insert(current);
            }
        }

        auto features = current->getFeatureDescriptors();

        for (auto feature : features)
        {
            auto typeDesc = feature->getTypeDescriptor();
            std::list<IterationItem> path;
            IterationItem item;
            item.desc = typeDesc;
            path.push_back(item);

            while (!path.empty())
            {
                IterationItem currentItem = path.front();
                auto typeDesc = currentItem.desc;
                path.pop_front();

                switch (typeDesc->getKind())
                {
                    case TypeDescriptor::kClass:
                    {
                        auto classDesc = typeDesc->as<ClassDescriptor>();
                        const Reference ref{currentItem.referenceType, current,
                                            feature, classDesc};

                        m_classInfoMap[classDesc].inReferences.push_back(ref);
                        m_classInfoMap[current].outReferences.push_back(ref);

                        pending.push_back(classDesc);
                    }
                    break;
                    case TypeDescriptor::kPointer:
                    {
                        if (currentItem.referenceType != Reference::kContained)
                        {
                            throw std::runtime_error("Invalid model");
                        }

                        auto ptrDesc = typeDesc->as<PointerTypeDescriptor>();
                        currentItem.desc = ptrDesc->getPointedTypeDescriptor();
                        currentItem.referenceType =
                            getReferenceType(ptrDesc->getPointerType());

                        path.push_back(currentItem);
                    }
                    break;
                    case TypeDescriptor::kPair:
                    {
                        auto pairDesc = typeDesc->as<PairTypeDescriptor>();

                        currentItem.desc = pairDesc->getFirstTypeDescriptor();
                        path.push_back(currentItem);

                        currentItem.desc = pairDesc->getSecondTypeDescriptor();
                        path.push_back(currentItem);
                    }
                    break;
                    // Containers
                    case TypeDescriptor::kList:
                    case TypeDescriptor::kMap:
                    case TypeDescriptor::kSet:
                    {
                        auto containerDesc =
                            typeDesc->as<ContainerTypeDescriptor>();
                        currentItem.desc =
                            containerDesc->getValueTypeDescriptor();
                        path.push_back(currentItem);
                    }
                    break;
                    default:
                        break;
                }
            }
        }

        processed.insert(current);
    }

    m_allClasses = {processed.begin(), processed.end()};

    // All references
    for (auto& it: m_classInfoMap)
    {
        ClassInfo& classInfo = it.second;
        for (const auto& subclass: classInfo.allSubclasses)
        {
            ClassInfo& subClassInfo = m_classInfoMap[subclass];
            subClassInfo.allOutReferences.insert(
                subClassInfo.allOutReferences.end(),
                classInfo.outReferences.begin(), classInfo.outReferences.end());
            subClassInfo.allInReferences.insert(
                subClassInfo.allInReferences.end(),
                classInfo.inReferences.begin(), classInfo.inReferences.end());
        }
    }

    for (auto& it: m_classInfoMap)
    {
        ClassInfo& classInfo = it.second;
        classInfo.allOutReferences.insert(
            classInfo.allOutReferences.end(),
            classInfo.outReferences.begin(), classInfo.outReferences.end());
        classInfo.allInReferences.insert(
            classInfo.allInReferences.end(),
            classInfo.inReferences.begin(), classInfo.inReferences.end());
    }
}

StructuralContext::StructuralContext(const ClassDescriptor* rootClassDesc)
    : m_impl(new Impl(rootClassDesc))
{
}

StructuralContext::~StructuralContext() { delete m_impl; }

const ClassDescriptor* StructuralContext::getRootClass() const
{
    return m_impl->m_rootClassDesc;
}

const std::vector<const ClassDescriptor*>& StructuralContext::getAllClasses()
    const
{
    return m_impl->m_allClasses;
}

const std::vector<Reference>& StructuralContext::getIncomingReferences(
    const ClassDescriptor* classDesc) const
{
    return m_impl->getClassInfo(classDesc).inReferences;
}

const std::vector<Reference>& StructuralContext::getAllIncomingReferences(
    const ClassDescriptor* classDesc) const
{
    return m_impl->getClassInfo(classDesc).allInReferences;
}

const std::vector<Reference>& StructuralContext::getOutgoingReferences(
    const ClassDescriptor* classDesc) const
{
    return m_impl->getClassInfo(classDesc).outReferences;
}

const std::vector<Reference>& StructuralContext::getAllOutgoingReferences(
    const ClassDescriptor* classDesc) const
{
    return m_impl->getClassInfo(classDesc).allOutReferences;
}
