pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        maven("https://plugins.gradle.org/m2/")
        maven("https://s01.oss.sonatype.org/content/repositories/snapshots/")
    }
}

// TODO: Update project's name.
rootProject.name = "WalletAdapterAndroid"
include(":plugin")

// Story 4-3 T6 — synthetic consumer subprojects under tools/consumer-merge-audit/.
// They depend on `:plugin` to exercise manifest-merger and emit AC-2 (positive)
// + AC-7 (negative) merged-manifest evidence. Located under repo-root/tools/
// (not android/) but mounted as Gradle subprojects of this build to reuse the
// pinned AGP / Kotlin / R8 toolchain. CI workflow:
// .github/workflows/manifest_merge_audit.yml.
include(":synthetic-consumer")
project(":synthetic-consumer").projectDir =
    file("../tools/consumer-merge-audit/synthetic-consumer")
include(":synthetic-warning")
project(":synthetic-warning").projectDir =
    file("../tools/consumer-merge-audit/synthetic-warning")
