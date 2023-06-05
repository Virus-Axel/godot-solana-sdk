use godot::engine::Node;
use godot::engine::global::PropertyHint;
use godot::obj::dom::UserDomain;
use godot::obj::mem::{StaticRefCount, DynamicRefCount};
use godot::prelude::*;
use godot::prelude::GodotClass;
use godot::engine::NodeVirtual;
use godot::builtin::GodotString;
use godot::export::Export;

use solana_sdk::pubkey;

struct pubkey_internal{
    key: pubkey::Pubkey,
}

impl Export for pubkey_internal{
    fn default_export_info() -> godot::export::ExportInfo {
        godot::export::ExportInfo {hint: PropertyHint::PROPERTY_HINT_NONE, hint_string: GodotString::new(), variant_type: VariantType::Object}
    }
    fn export(&self) -> Self {
        pubkey_internal { key: pubkey::Pubkey::new_unique() }
    }
}

impl GodotClass for pubkey_internal{
    type Base = Node;
    type Declarer = UserDomain;
    type Mem = DynamicRefCount;
    const CLASS_NAME: &'static str = "public key";
}

#[derive(GodotClass)]
#[class(base=Node)]
struct Pubkey {
    #[export]
    key: Gd<pubkey_internal>,
    #[export]
    seed: GodotString,
    #[export]
    base_key: Gd<pubkey_internal>,

    #[base]
    base: Base<Node>
}

#[godot_api]
impl Pubkey {

    #[func]
    fn increase_speed(&mut self, body: Gd<Pubkey>) {
        
    }
    #[func]
    fn print(&mut self) -> GodotString{
        return GodotString::from(self.key.to_string());
    }

    #[signal]
    fn speed_increased();
}

#[godot_api]
impl NodeVirtual for Pubkey {
    fn init(base: Base<Self::Base>) -> Self {
        Self {
            base,
            key: Gd::new(pubkey_internal { key: pubkey::Pubkey::new_unique() }),
            base_key: Gd::new(pubkey_internal { key: pubkey::Pubkey::new_unique() }),
            seed: GodotString::new(),
        }
    }
}