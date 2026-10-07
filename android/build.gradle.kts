// Top-level build file where you can add configuration options common to all sub-projects/modules.

// Story 2-1 CI fixup round 3 (D-CI-3, Rule 2): pin R8/D8 8.5.35 via buildscript
// classpath to override AGP 7.4.1's bundled R8 4.0.48. Bundled 4.0.48 throws
// `com.android.tools.r8.kotlin.H` (Kotlin-metadata parsing failure) during
// `:plugin:dexDebugAndroidTestFileDependencies` on third-party jars compiled
// with newer Kotlin metadata — observed on kotlinx-coroutines 1.7.3, ktor 2.3.11,
// mockk-agent 1.13.13, com.solanamobile:web3-core-jvm 0.3.0-beta4. R8 8.x
// understands modern Kotlin metadata and fixes the emulator androidTest build.
//
// AGP 7.4.1 + Kotlin plugin 1.8.0 remain pinned per D-10 (Story 1-2); R8 is an
// independently-versioned artifact that AGP picks up via buildscript classpath
// priority. Release R8 smoke (A-10) re-validates @UsedByGodot survival under
// the new R8. PermittedSubclasses workaround in proguard-library-self.pro is
// retained — R8 8.x handles the attribute natively but the keep rules are inert
// when they're not needed. Revisit alongside AGP 8.x bump tracked by CR-5.
buildscript {
    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath("com.android.tools:r8:8.5.35")
    }
}

plugins {
    id("com.android.library") version "7.4.1" apply false
    id("org.jetbrains.kotlin.android") version "1.8.0" apply false
    id("org.jlleitschuh.gradle.ktlint") version "12.1.1" apply false
    // Plan (Story 1-2 §Tooling Notes) pins ktlint-gradle 14.2.0, but 14.x requires Gradle 8.x + Kotlin 2.x
    // while this module is on Gradle 7.5 / Kotlin plugin 1.8.0 (scaffold-owned, bumping is out of
    // story scope). 12.1.1 is the last 12.x release that targets Gradle 7 + Kotlin 1.x; logged as
    // D10 (Rule 1) under Story 1-2 §Deviations Expected. Revisit alongside the Kotlin plugin 2.x
    // upgrade (tracked in concerns.md CR-5).
}
