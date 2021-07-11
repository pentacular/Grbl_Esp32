#pragma once

#include "../Pin.h"
#include <WString.h>

template <typename T>
class Setting {
    T _value;

    Setting(const Setting<T>& o) = delete;
    Setting<T>& operator=(const Setting<T>& o) = delete;

public:
    Setting() : _value() {}
    Setting(const T& value) : _value(value) {}
    Setting(T&& value) : _value(value) {}

    Setting<T>& operator=(const T& value) {
        _value = value;
        return *this;
    }

    Setting(Setting<T>&& o) = default;
    Setting<T>& operator=(Setting<T>&& o) = default;

    // Implicit conversion, so we don't have to keep on casting and using methods when
    // we use settings. Note that value is returned by value. Most of the times these
    // 2 operators do the job; the only notable exception is when using members:
    operator const T&() const { return _value; }
    operator T() { return _value; }

    // When using members, we have to make an explicit call. Casting is inconvenient:
    T&       get() { return _value; }
    const T& get() const { return _value; }
};

template <>
class Setting<String> {
    String _value;

    Setting(const Setting<String>& o) = delete;
    Setting<String>& operator=(const Setting<String>& o) = delete;

public:
    // String needs some different constructors:

    Setting() : _value() {}
    Setting(const String& value) : _value(value) {}
    Setting(const char* value) : _value(value) {}
    Setting(String&& value) : _value(std::move(value)) {}

    Setting(Setting<String>&& o) = default;
    Setting<String>& operator=(Setting<String>&& o) = default;

    Setting<String>& operator=(const String& value) {
        _value = value;
        return *this;
    }

    Setting<String>& operator=(const char* value) {
        _value = value;
        return *this;
    }

    // Implicit conversion, so we don't have to keep on casting and using methods when
    // we use settings. Note that value is returned by value. Most of the times these
    // 2 operators do the job; the only notable exception is when using members:
    operator const String&() const { return _value; }
    operator String() { return _value; }

    // When using members, we have to make an explicit call. Casting is inconvenient:
    String&       get() { return _value; }
    const String& get() const { return _value; }
};

template <>
class Setting<Pin> {
    Pin _value;

    Setting(const Setting<Pin>& o) = delete;
    Setting<Pin>& operator=(const Setting<Pin>& o) = delete;

public:
    Setting() : _value() {}
    Setting(Pin&& pin) : _value(std::move(pin)) {}

    // Implicit conversion, so we don't have to keep on casting and using methods when
    // we use settings. Note that value is returned by value. Most of the times these
    // 2 operators do the job; the only notable exception is when using members:
    operator const Pin&() const { return _value; }
    operator Pin&() { return _value; }

    // When using members, we have to make an explicit call. Casting is inconvenient:
    Pin&       get() { return _value; }
    const Pin& get() const { return _value; }

    // 99% of the calls to Setting<Pin> uses these methods. We just forward these for
    // convenience. This is not a complete list; for the other 1%, we can use 'get':
    inline bool              undefined() const { return _value.undefined(); }
    inline bool              defined() const { return _value.defined(); }
    inline uint8_t           getNative(Pin::Capabilities expectedBehavior) const { return _value.capabilities(); }
    inline void              write(bool value) const { _value.write(value); }
    inline void              synchronousWrite(bool value) const { _value.synchronousWrite(value); }
    inline bool              read() const { return _value.read(); }
    inline void              setAttr(Pin::Attr attributes) const { _value.setAttr(attributes); }
    inline Pin::Attr         getAttr() const { return _value.getAttr(); }
    inline void              on() const { _value.on(); }
    inline void              off() const { _value.off(); }
    inline Pin::Capabilities capabilities() const { return _value.capabilities(); }
    inline String            name() const { return _value.name(); }
    inline void              report(const char* legend) { _value.report(legend); }
};
