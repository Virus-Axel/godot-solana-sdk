//pub mod utils;
pub mod modules;
//pub mod test;



use godot::engine::{Engine, CanvasLayerVirtual, Node};
use godot::prelude::*;
use godot::engine::NodeVirtual;

struct Main;

#[gdextension]
unsafe impl ExtensionLibrary for Main {}
