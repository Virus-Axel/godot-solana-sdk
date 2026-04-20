// Top-level build file where you can add configuration options common to all sub-projects/modules.
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
