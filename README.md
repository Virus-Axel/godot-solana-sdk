# godot-solana-sdk
A GDExtention with bindings to the solana-sdk. This plugin provides the possibility to create Solana transaction in your Godot game. This can be usefull when utilizing NFTs or other Solana smart contracts. The plugin builds for all platforms supported by Godot. The plugin builds for Godot 4.1.

# Version history
| SDK         | Godot     | Platforms |
|--------------|-----------|------------|
| Unstable | 4.1      | Windows, Linux, Web, Android, IOS        |
| v0.2      | 4.0  | Windows, Linux, Web, Android, IOS       |
| v0.1      | 4.0  | Windows, Linux, Web, Android, IOS       |

# How to use
Download the binaries in the release section [here](https://github.com/Virus-Axel/godot-solana-sdk/releases). place the binaries anywhere in your godot project directory under res://bin/. Reload your project and you are good to go.

# Nodes
There is currently only one node provided by this plugin, that is the Transaction node.
## Transaction
This node is a customizable transaction that can be signed and serialized. The transaction consists of:
- A payer Pubkey.
- An array of instructions.
- An array of signing keypairs.

The transaction can be signed and serialized for use with phantom and the Solana RPC API.  
![Screenshot from 2023-05-20 17-41-43](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/56464477-dafc-41c6-8c6c-cfe9cd89ca6d)

# Resources

## Instruction
The instruction resource consists of:
- Program ID
- Data
- List of account metas

The data is sent as it is and no serialization/deserialization is supported. The list of account metas are passed to the program specified by the program ID. The instruction tells the solana program what to do.  
![Screenshot from 2023-05-20 17-44-00](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/7ad0212f-f897-43a9-82e5-d0b5351772de)

## AccountMeta
The AccountMeta consists of an account key (A pubkey or a keypair) and information on weather the account is signer and if it is a writeable account.  
![Screenshot from 2023-05-20 17-49-50](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/ccd8b08a-a134-48ee-8623-0cc049a6b6fa)

## Pubkey
The pubkey is used for many things. It can be created from seeds, from associated tokens and by setting the base58 value or bytes yourself.  
![Screenshot from 2023-05-20 17-50-58](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/7eea01f5-f99a-4174-a27e-9a860d1de82f)

## Keypair
A keypair has a private key and a public key. They are used to sign transactions.  
![Screenshot from 2023-05-20 17-51-45](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/407d47f9-927b-42a2-b820-08a0607edc17)

## Hash
A simpler version of pubkey. Used to pass blockhashes when signing transactions.

# Work in Progress
I will continue to work on this plugin. Feel free to open issues or feature requests.

# Setting up Build Environment
The plugin builds easily for your host platform with [SCons](https://www.scons.org/).

```
git clone --recurse-submodules git@github.com:Virus-Axel/godot-solana-sdk.git
cd godot-solana-sdk
scons
```

This repository also includes means to build all the binaries in containers. This requires [Podman](https://podman.io/) to be installed.

```
scons --container_build
```
