/**
 * @file
 * Declares the base class for fact resolvers.
 */
#ifndef FACTER_FACTS_FACT_RESOLVER_HPP_
#define FACTER_FACTS_FACT_RESOLVER_HPP_

#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

// Forward declare RE2 so users of this header don't have to include re2
namespace re2 {
    class RE2;
}

namespace facter { namespace facts {

    /**
     * Thrown when a circular fact resolution is detected.
     */
    struct circular_resolution_exception : std::runtime_error
    {
        /**
         * Constructs a circular_resolution_exception.
         * @param message The exception message.
         */
        explicit circular_resolution_exception(std::string const& message);
    };

    /**
     * Thrown when a resolver is constructed with an invalid fact name pattern.
     */
    struct invalid_name_pattern_exception : std::runtime_error
    {
        /**
         * Constructs a invalid_name_pattern_exception.
         * @param message The exception message.
         */
        explicit invalid_name_pattern_exception(std::string const& message);
    };

    struct fact_map;

    /**
     * Base class for fact resolvers.
     * A fact resolver is responsible for resolving one or more facts.
     * This type can be moved but cannot be copied.
     */
    struct fact_resolver
    {
        /**
         * Constructs a fact_resolver.
         * @param name The fact resolver name.
         * @param names The fact names the resolver is responsible for.
         * @param patterns Regular expression patterns for additional ("dynamic") facts the resolver is responsible for.
         */
        fact_resolver(std::string&& name, std::vector<std::string>&& names, std::vector<std::string> const& patterns = {});

        /**
         * Destructs the fact_resolver.
         */
        virtual ~fact_resolver();

        /**
         * Prevents the fact_resolver from being copied.
         */
        fact_resolver(fact_resolver const&) = delete;
        /**
         * Prevents the fact_resolver from being copied.
         * @returns Returns this fact_resolver.
         */
        fact_resolver& operator=(fact_resolver const&) = delete;
        /**
         * Moves the given fact_resolver into this fact_resolver.
         * @param other The fact_resolver to move into this fact_resolver.
         */
        fact_resolver(fact_resolver&& other) = default;
        /**
         * Moves the given fact_resolver into this fact_resolver.
         * @param other The fact_resolver to move into this fact_resolver.
         * @return Returns this fact_resolver.
         */
        fact_resolver& operator=(fact_resolver&& other) = default;

        /**
         * Gets the name of the fact resolver.
         * @return Returns the fact resolver's name.
         */
        std::string const& name() const;

        /**
         * Gets the fact names the resolver is responsible for resolving.
         * @return Returns a vector of fact names.
         */
        std::vector<std::string> const& names() const;

        /**
         * Called to resolve all facts the resolver is responsible for.
         * @param facts The fact map that is resolving facts.
         */
        void resolve(fact_map& facts);

        /**
         * Checks whether or not the resolver can resolve the given fact name.
         * Note that it is not required to return true if the given name is in the names vector.
         * @param name The fact name to check.
         * @return Returns true if the resolver can resolve the given name or false if it cannot.
         */
        bool can_resolve(std::string const& name) const;

     protected:
        /**
         * Called to resolve all facts the resolver is responsible for.
         * @param facts The fact map that is resolving facts.
         */
        virtual void resolve_facts(fact_map& facts) = 0;

     private:
        std::string _name;
        std::vector<std::string> _names;
        std::vector<std::unique_ptr<re2::RE2>> _regexes;
        bool _resolving;
    };

}}  // namespace facter::facts

#endif  // FACTER_FACTS_FACT_RESOLVER_HPP_

