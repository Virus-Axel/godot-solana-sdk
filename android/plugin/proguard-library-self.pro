# -----------------------------------------------------------------------------
# proguard-library-self.pro
# -----------------------------------------------------------------------------
# Library-self R8 rules -- applied ONLY to this AAR's own :plugin:assembleRelease
# pass, NOT shipped to downstream consumers. This file is referenced from
# android/plugin/build.gradle.kts under buildTypes.release.proguardFiles but
# is deliberately NOT listed in defaultConfig.consumerProguardFiles.
#
# Why this distinction matters: downstream apps that bundle our AAR into a
# minified release build SHOULD run full R8 obfuscation (shrinking + renaming)
# for binary-size + reverse-engineering-resistance reasons. Our rules in
# consumer-rules.pro govern what THEY need to preserve. But when WE build our
# own AAR for verification + artifact publication, we want human-readable
# names in the bytecode so ci/verify_r8_strip.sh's Gate 2 (literal FQN grep
# for invokestatic to SdkLog.v / SdkLog.d) can detect the "R8 ran + rule
# broken + obfuscation on" failure mode -- otherwise R8 renames SdkLog to
# `a`/`b`/... and the FQN grep returns zero false-passes while the release
# AAR actually contains intact v/d call sites against the renamed class.
# Preserving names closes that blind spot at the source.
#
# Library AARs conventionally keep readable names so downstream consumers
# can debug stack traces into library code; this rule is consistent with
# Android library-author norms.
# -----------------------------------------------------------------------------

-dontobfuscate
