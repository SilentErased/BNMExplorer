#include <BNM/PropertyBase.hpp>
#include <BNM/DebugMessages.hpp>

using namespace BNM;

PropertyBase::PropertyBase(const IL2CPP::PropertyInfo *info) {
    if (!info) return;

    _data = (IL2CPP::PropertyInfo *) info;

    if (info->get && info->get->methodPointer) {
        _hasGetter = true;
        _getter = info->get;
    }

    if (info->set && info->set->methodPointer) {
        _hasSetter = true;
        _setter = info->set;
    }
}

PropertyBase &PropertyBase::SetInstance(IL2CPP::Il2CppObject *val) {
    if (_hasGetter) _getter.SetInstance(val);
    if (_hasSetter) _setter.SetInstance(val);
    return *this;
}

BNM::Class PropertyBase::GetType() const {
    if (!_data) return {};
    if (_data->get) return _data->get->return_type;
    if (_data->set && _data->set->parameters_count > 0) {
#if UNITY_VER < 212
        return (_data->set->parameters + _data->set->parameters_count - 1)->parameter_type;
#else
        return _data->set->parameters[_data->set->parameters_count - 1];
#endif
    }
    return {};
}

BNM::Class PropertyBase::GetParentClass() const {
    if (!_data) return {};
    return _data->parent;
}
