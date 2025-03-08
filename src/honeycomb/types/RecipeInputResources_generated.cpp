#include "honeycomb/types/RecipeInputResources_generated.hpp"

#include "honeycomb/types/RecipeInputResource_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void RecipeInputResources::set_inputOne(const Variant &val) {
	this->inputOne = val;
}

Variant RecipeInputResources::get_inputOne() const {
	return this->inputOne;
}

void RecipeInputResources::set_inputTwo(const Variant &val) {
	this->inputTwo = val;
}

Variant RecipeInputResources::get_inputTwo() const {
	return this->inputTwo;
}

void RecipeInputResources::set_inputThree(const Variant &val) {
	this->inputThree = val;
}

Variant RecipeInputResources::get_inputThree() const {
	return this->inputThree;
}

void RecipeInputResources::set_inputFour(const Variant &val) {
	this->inputFour = val;
}

Variant RecipeInputResources::get_inputFour() const {
	return this->inputFour;
}

void RecipeInputResources::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_inputOne"), &RecipeInputResources::get_inputOne);
	ClassDB::bind_method(D_METHOD("set_inputOne", "value"), &RecipeInputResources::set_inputOne);
	ClassDB::add_property("RecipeInputResources", PropertyInfo(Variant::Type::OBJECT, "inputOne"), "set_inputOne", "get_inputOne");
	ClassDB::bind_method(D_METHOD("get_inputTwo"), &RecipeInputResources::get_inputTwo);
	ClassDB::bind_method(D_METHOD("set_inputTwo", "value"), &RecipeInputResources::set_inputTwo);
	ClassDB::add_property("RecipeInputResources", PropertyInfo(Variant::Type::OBJECT, "inputTwo"), "set_inputTwo", "get_inputTwo");
	ClassDB::bind_method(D_METHOD("get_inputThree"), &RecipeInputResources::get_inputThree);
	ClassDB::bind_method(D_METHOD("set_inputThree", "value"), &RecipeInputResources::set_inputThree);
	ClassDB::add_property("RecipeInputResources", PropertyInfo(Variant::Type::OBJECT, "inputThree"), "set_inputThree", "get_inputThree");
	ClassDB::bind_method(D_METHOD("get_inputFour"), &RecipeInputResources::get_inputFour);
	ClassDB::bind_method(D_METHOD("set_inputFour", "value"), &RecipeInputResources::set_inputFour);
	ClassDB::add_property("RecipeInputResources", PropertyInfo(Variant::Type::OBJECT, "inputFour"), "set_inputFour", "get_inputFour");
	ClassDB::bind_method(D_METHOD("to_dict"), &RecipeInputResources::to_dict);
}
Dictionary RecipeInputResources::to_dict() {
	Dictionary res;

	if (inputOne.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::RecipeInputResource>(inputOne);
		if (ptr) {
			res["inputOne"] = ptr->to_dict();
		}
	}

	if (inputTwo.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::RecipeInputResource>(inputTwo);
		if (ptr) {
			res["inputTwo"] = ptr->to_dict();
		}
	}

	if (inputThree.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::RecipeInputResource>(inputThree);
		if (ptr) {
			res["inputThree"] = ptr->to_dict();
		}
	}

	if (inputFour.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::RecipeInputResource>(inputFour);
		if (ptr) {
			res["inputFour"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot