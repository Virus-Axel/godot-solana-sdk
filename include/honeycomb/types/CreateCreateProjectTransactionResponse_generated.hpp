#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEPROJECTTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEPROJECTTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateCreateProjectTransactionResponse.
 */
class CreateCreateProjectTransactionResponse : public Resource {
	GDCLASS(CreateCreateProjectTransactionResponse, Resource)

private:
	Variant tx;
	String project;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateCreateProjectTransactionResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the tx property.
	 * @param val New value for tx.
	 */
	void set_tx(const Variant &val);
	/**
	 * @brief Get the tx property.
	 * @return Current value of tx.
	 */
	Variant get_tx() const;

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const String &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	String get_project() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEPROJECTTRANSACTIONRESPONSE
