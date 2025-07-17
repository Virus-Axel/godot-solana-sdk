#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONPOOL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONPOOL

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MissionPool.
 */
class MissionPool : public Resource {
	GDCLASS(MissionPool, Resource)

private:
	PackedByteArray address;
	PackedByteArray program_id;
	PackedByteArray discriminator;
	int32_t bump;
	Variant project;
	String name;
	PackedByteArray factionsMerkleRoot;
	int32_t randomizerRound;
	Array characterModels;
	PackedInt32Array guildKits;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MissionPool.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

	/**
	 * @brief Set the program_id property.
	 * @param val New value for program_id.
	 */
	void set_program_id(const PackedByteArray &val);
	/**
	 * @brief Get the program_id property.
	 * @return Current value of program_id.
	 */
	PackedByteArray get_program_id() const;

	/**
	 * @brief Set the discriminator property.
	 * @param val New value for discriminator.
	 */
	void set_discriminator(const PackedByteArray &val);
	/**
	 * @brief Get the discriminator property.
	 * @return Current value of discriminator.
	 */
	PackedByteArray get_discriminator() const;

	/**
	 * @brief Set the bump property.
	 * @param val New value for bump.
	 */
	void set_bump(const int32_t &val);
	/**
	 * @brief Get the bump property.
	 * @return Current value of bump.
	 */
	int32_t get_bump() const;

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

	/**
	 * @brief Set the name property.
	 * @param val New value for name.
	 */
	void set_name(const String &val);
	/**
	 * @brief Get the name property.
	 * @return Current value of name.
	 */
	String get_name() const;

	/**
	 * @brief Set the factionsMerkleRoot property.
	 * @param val New value for factionsMerkleRoot.
	 */
	void set_factionsMerkleRoot(const PackedByteArray &val);
	/**
	 * @brief Get the factionsMerkleRoot property.
	 * @return Current value of factionsMerkleRoot.
	 */
	PackedByteArray get_factionsMerkleRoot() const;

	/**
	 * @brief Set the randomizerRound property.
	 * @param val New value for randomizerRound.
	 */
	void set_randomizerRound(const int32_t &val);
	/**
	 * @brief Get the randomizerRound property.
	 * @return Current value of randomizerRound.
	 */
	int32_t get_randomizerRound() const;

	/**
	 * @brief Set the characterModels property.
	 * @param val New value for characterModels.
	 */
	void set_characterModels(const Array &val);
	/**
	 * @brief Get the characterModels property.
	 * @return Current value of characterModels.
	 */
	Array get_characterModels() const;

	/**
	 * @brief Set the guildKits property.
	 * @param val New value for guildKits.
	 */
	void set_guildKits(const PackedInt32Array &val);
	/**
	 * @brief Get the guildKits property.
	 * @return Current value of guildKits.
	 */
	PackedInt32Array get_guildKits() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONPOOL
