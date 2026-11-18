#include <BNM/Delegates.hpp>

BNM::MethodBase BNM::DelegateBase::GetMethod() const {
    if (!CheckForNull(this)) return {};
    auto method = MethodBase(this->method);
    auto instance = GetInstance();
    if (instance) method.SetInstance(instance);
    return method;
}

BNM::DelegateBase *BNM::DelegateBase::Create(BNM::MethodBase method) {
    return (BNM::DelegateBase *) BNM::Class(object.klass).CreateNewObjectParameters(method._instance, method._data);
}

std::vector<BNM::MethodBase> BNM::MulticastDelegateBase::GetMethods() const {
    if (!CheckForNull(this)) return {};

    auto delegates = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
    if (!delegates || delegates->capacity == 0) return {((DelegateBase *)this)->GetMethod()};

    std::vector<MethodBase> ret{};
    ret.reserve(delegates->capacity);
    for (IL2CPP::il2cpp_array_size_t i = 0; i < delegates->capacity; ++i) {
        auto d = delegates->At(i);
        if (d) ret.push_back(d->GetMethod());
    }
    return ret;
}

void BNM::MulticastDelegateBase::Add(BNM::DelegateBase *del)  {
    if (!CheckForNull(this) || !del) return;

    auto delegatesArr = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
    if (!delegatesArr) {
        auto elemClass = delegate.object.klass ? delegate.object.klass : del->object.klass;
        auto arr = BNM::Class(elemClass).NewArray<DelegateBase *>(2);
        if (arr) {
            arr->m_Items[0] = (DelegateBase *)this;
            arr->m_Items[1] = del;
            this->delegates = (decltype(this->delegates)) arr;
        }
        return;
    }

    auto elemClass = delegatesArr->klass ? delegatesArr->klass->element_class : delegate.object.klass;
    auto arr = BNM::Class(elemClass).NewArray<DelegateBase *>(delegatesArr->capacity + 1);
    if (!arr) return;
    arr->CopyFrom(delegatesArr->m_Items, delegatesArr->capacity);
    arr->m_Items[delegatesArr->capacity] = del;
    this->delegates = (decltype(this->delegates)) arr;
}

void BNM::MulticastDelegateBase::Remove(BNM::DelegateBase *del)  {
    if (!CheckForNull(this) || !del) return;

    auto delegatesArr = (Structures::Mono::Array<DelegateBase *> *) this->delegates;
    if (!delegatesArr) return;

    IL2CPP::il2cpp_array_size_t index = 0;
    bool found = false;
    for (IL2CPP::il2cpp_array_size_t i = 0; i < delegatesArr->capacity; ++i) {
        if (delegatesArr->m_Items[i] != del) continue;

        found = true;
        index = i;
        break;
    }
    if (!found) return;

    if (delegatesArr->capacity <= 1) {
        this->delegates = nullptr;
        return;
    }

    auto elemClass = delegatesArr->klass ? delegatesArr->klass->element_class : delegate.object.klass;
    auto arr = BNM::Class(elemClass).NewArray<DelegateBase *>(delegatesArr->capacity - 1);
    if (!arr) return;
    for (IL2CPP::il2cpp_array_size_t i = 0, j = 0; i < delegatesArr->capacity; ++i) {
        if (i != index) {
            arr->m_Items[j++] = delegatesArr->m_Items[i];
        }
    }
    this->delegates = (decltype(this->delegates)) arr;
}

BNM::DelegateBase *BNM::MulticastDelegateBase::Add(BNM::MethodBase method) {
    if (!CheckForNull(this) || !method.IsValid()) return {};

    auto delegate = ((BNM::DelegateBase *)this)->Create(method);

    Add(delegate);

    return delegate;
}