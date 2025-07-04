#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLED

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Assembled.
 */
class Assembled : public Resource {
	GDCLASS(Assembled, Resource)

private:
	Variant hash;
	Variant mint;
	String uri;
	Array attributes;
	Variant assemblerConfig;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Assembled.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the hash property.
	 * @param val New value for hash.
	 */
	void set_hash(const Variant &val);
	/**
	 * @brief Get the hash property.
	 * @return Current value of hash.
	 */
	Variant get_hash() const;

	/**
	 * @brief Set the mint property.
	 * @param val New value for mint.
	 */
	void set_mint(const Variant &val);
	/**
	 * @brief Get the mint property.
	 * @return Current value of mint.
	 */
	Variant get_mint() const;

	/**
	 * @brief Set the uri property.
	 * @param val New value for uri.
	 */
	void set_uri(const String &val);
	/**
	 * @brief Get the uri property.
	 * @return Current value of uri.
	 */
	String get_uri() const;

	/**
	 * @brief Set the attributes property.
	 * @param val New value for attributes.
	 */
	void set_attributes(const Array &val);
	/**
	 * @brief Get the attributes property.
	 * @return Current value of attributes.
	 */
	Array get_attributes() const;

	/**
	 * @brief Set the assemblerConfig property.
	 * @param val New value for assemblerConfig.
	 */
	void set_assemblerConfig(const Variant &val);
	/**
	 * @brief Get the assemblerConfig property.
	 * @return Current value of assemblerConfig.
	 */
	Variant get_assemblerConfig() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLED
