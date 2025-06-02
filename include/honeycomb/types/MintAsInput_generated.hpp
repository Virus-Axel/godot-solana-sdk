#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MintAsInput.
 */
class MintAsInput : public Resource {
	GDCLASS(MintAsInput, Resource)

private:
	String kind;
	Variant mplBubblegum;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MintAsInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the kind property.
	 * @param val New value for kind.
	 */
	void set_kind(const String &val);
	/**
	 * @brief Get the kind property.
	 * @return Current value of kind.
	 */
	String get_kind() const;

	/**
	 * @brief Set the mplBubblegum property.
	 * @param val New value for mplBubblegum.
	 */
	void set_mplBubblegum(const Variant &val);
	/**
	 * @brief Get the mplBubblegum property.
	 * @return Current value of mplBubblegum.
	 */
	Variant get_mplBubblegum() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASINPUT
