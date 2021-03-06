/**
 * @file
 * Declares the base fact value type.
 */
#ifndef FACTER_FACTS_VALUE_HPP_
#define FACTER_FACTS_VALUE_HPP_

#include <string>
#include <functional>
#include <memory>
#include <iostream>

// Forward declare needed yaml-cpp classes.
namespace YAML {
    class Emitter;
}

// Forward delcare needed rapidjson classes.
namespace rapidjson {
    class CrtAllocator;
    template <typename BaseAllocator> class MemoryPoolAllocator;
    template <typename Encoding, typename Allocator> class GenericValue;
    template<typename CharType> struct UTF8;
    typedef GenericValue<UTF8<char>, MemoryPoolAllocator<CrtAllocator>> Value;
    typedef MemoryPoolAllocator<CrtAllocator> Allocator;
    template <typename Encoding, typename Allocator> class GenericDocument;
    typedef GenericDocument<UTF8<char>, MemoryPoolAllocator<CrtAllocator>> Document;
}

extern "C" {
    /**
     * Simple structure to store enumeration callbacks.
     */
    typedef struct _enumeration_callbacks enumeration_callbacks;
}

namespace facter { namespace facts {

    /**
     * Base class for values.
     * This type can be moved but cannot be copied.
     */
    struct value
    {
        /**
         * Constructs a value.
         */
        value() = default;

        /**
         * Destructs a value.
         */
        virtual ~value() = default;

        /**
         * Prevents the value from being copied.
         */
        value(value const&) = delete;
        /**
         * Prevents the value from being copied.
         * @returns Returns this value.
         */
        value& operator=(value const&) = delete;
        /**
         * Moves the given value into this value.
         * @param other The value to move into this value.
         */
        value(value&& other) = default;
        /**
         * Moves the given value into this value.
         * @param other The value to move into this value.
         * @return Returns this value.
         */
        value& operator=(value&& other) = default;

        /**
         * Converts the value to a JSON value.
         * @param allocator The allocator to use for creating the JSON value.
         * @param value The returned JSON value.
         */
        virtual void to_json(rapidjson::Allocator& allocator, rapidjson::Value& value) const = 0;

        /**
         * Notifies the appropriate callback based on the type of the value.
         * @param name The fact name to pass to the callback.
         * @param callbacks The callbacks to use to notify.
         */
        virtual void notify(std::string const& name, enumeration_callbacks const* callbacks) const = 0;

     protected:
        /**
          * Writes the value to the given stream.
          * @param os The stream to write to.
          * @returns Returns the stream being written to.
          */
        virtual std::ostream& write(std::ostream& os) const = 0;
        friend std::ostream& operator<<(std::ostream& os, value const& val);

        /**
          * Writes the value to the given YAML emitter.
          * @param emitter The YAML emitter to write to.
          * @returns Returns the given YAML emitter.
          */
        virtual YAML::Emitter& write(YAML::Emitter& emitter) const = 0;
        friend YAML::Emitter& operator<<(YAML::Emitter& emitter, value const& val);
    };

    /**
     * Utility function for making a value.
     * @tparam T The type of the value being constructed.
     * @tparam Args The variadic types for the value's constructor.
     * @param args The arguments to the value's constructor.
     * @return Returns a unique pointer to the constructed value.
     */
    template<typename T, typename ...Args>
    std::unique_ptr<T> make_value(Args&& ...args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    /**
     * Insertion operator for value.
     * @param os The output stream to write to.
     * @param val The value to write to the stream.
     * @return Returns the given output stream.
     */
    std::ostream& operator<<(std::ostream& os, value const& val);

    /**
     * Insertion operator for value.
     * @param emitter The YAML emitter to write to.
     * @param val The value to write to the YAML emitter.
     * @return Returns the given YAML emitter.
     */
    YAML::Emitter& operator<<(YAML::Emitter& emitter, value const& val);

}}  // namespace facter::facts

#endif  // FACTER_FACTS_VALUE_HPP_
