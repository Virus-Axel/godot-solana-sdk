# Godot Solana SDK

A GDExtention that gives Godot engine Solana support. This plugin provides the possibility to create Solana games for any platform supported by Godot. Utilize NFTs and Solana smart contracts in your game. The SDK has interface nodes for Anchor programs and instruction builders for Candy Machine, Candy Guard, System programs, SPL tokens and more.

![godot_solana_sdk4](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/1d4c4ff8-0763-4ad2-82f6-4ea3d66cde96)

# How to use

Download the binaries in the release section [here](https://github.com/Virus-Axel/godot-solana-sdk/releases). place the binaries anywhere in your godot project directory under res://bin/. Reload your project and you are good to go. See examples for an up to date usage example. Version 1.0 is built for 4.2 and later. Use the latest official release of godot to avoid problems.

**Note!** It might be dangerous to use mainnet in you applications. Crypto can be stolen. Security is not yet evaluated for the godot-solana-sdk. I can audit security if there is demand.

When the plugin is loaded, the engine will have an additional set of nodes and resources.

## Nodes

These are the nodes provided by this plugin:

- SolanaUtils
- SolanaClient
- Account
- Transaction
- WalletAdapter
- ComputeBudget
- SystemProgram
- TokenProgram
- MplTokenMetadata
- AssociatedTokenAccountProgram
- MplCandyMachine
- MplCandyGuard
- AnchorProgram

![Screenshot from 2023-05-20 17-41-43](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/56464477-dafc-41c6-8c6c-cfe9cd89ca6d)

## Resources
These are the resources provided by this plugin:

- Pubkey
- Keypair
- CompiledInstruction
- Message
- Hash
- AccountMeta
- Instruction
- MetaDataCreator
- MetaDataCollection
- MetaDataUses
- MetaData
- CandyGuardAccessList
- CandyMachineData

![Screenshot from 2023-05-20 17-44-00](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/7ad0212f-f897-43a9-82e5-d0b5351772de)

## Examples

There are example projects under the [example folder](https://github.com/Virus-Axel/godot-solana-sdk/tree/master/example). These are used to test the plugin whenever changes are made to the code. There might be some good references on how to get started. There are also a [demo project on Github](https://github.com/ZenRepublic/GodotSolanaSDKDemos) showcasing the plugin. The source can also be used for reference. The demo project is deployed on [Github pages](https://zenrepublic.github.io/GodotSolanaSDKDemos/), go check it out. Here is how it looks.

![Screenshot from 2024-05-01 18-13-29](https://github.com/Virus-Axel/godot-solana-sdk/assets/14315050/1f77ce05-0a42-4892-8bc5-c8059d900316)


# Contributing

Feel free to open issues or feature requests. Contributions are very welcome. There is a [contribution guideline](https://github.com/Virus-Axel/godot-solana-sdk/blob/master/CONTRIBUTING.md) but get involved by heading to [discord](https://discord.gg/9aFDCvqPgt) or [starting a discussion](https://github.com/Virus-Axel/godot-solana-sdk/discussions).

## Setting up Build Environment

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

# Projects using the Plugin

Hurry up, create some games and list them here!
