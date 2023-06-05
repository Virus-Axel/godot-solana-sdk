use godot::engine::Node;
use godot::prelude::*;
use godot::prelude::GodotClass;
use godot::engine::NodeVirtual;
use godot::builtin::GodotString;

use solana_sdk::pubkey;


#[derive(GodotClass)]
#[class(base=Node)]
struct Pubkey {
    #[export]
    key: GodotString,
    #[export]
    seed: GodotString,
    #[export]
    base_key: Gd<Pubkey>,

    #[base]
    base: Base<Node>
}

#[godot_api]
impl Pubkey {
    fn empty() -> Pubkey{
        Pubkey {base: Base::<Node>::from(Node::new_alloc().into()), key: GodotString::new(), seed: GodotString::new(), base_key: Gd::new_default() }
    }

    #[func]
    fn increase_speed(&mut self, body: Gd<Pubkey>) {
        
    }
    #[func]
    fn print(&mut self) -> GodotString{
        return GodotString::from(self.key_object.to_string());
    }

    #[signal]
    fn speed_increased();
}

#[godot_api]
impl NodeVirtual for Pubkey {
    fn init(base: Base<Self::Base>) -> Self {
        Self { base, key: GodotString::new(), base_key: Pubkey::empty() }
    }
}