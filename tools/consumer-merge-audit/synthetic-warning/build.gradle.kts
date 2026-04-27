// Story 4-3 T6 — synthetic-warning (AC-7 negative case)
//
// Same skeleton as ../synthetic-consumer/ except the AndroidManifest.xml
// declares <application android:allowBackup="true"> WITHOUT tools:replace.
// Per manifest-merger's "consumer wins" precedence when the consumer
// explicitly sets a value, the merged release-variant manifest will surface
// allowBackup="true" — the plugin loses, even though it sets allowBackup
// false with tools:replace.
//
// Wait — actually with tools:replace on the plugin side, the plugin should
// win. The AC-7 case models a downstream consumer that ALSO declares
// tools:replace OR a less-protected merger flow where the plugin's
// tools:replace is overridden by the consumer's explicit value. To exercise
// the warning path cleanly, we add tools:replace="android:allowBackup" on
// the consumer side too — that creates the "two writers, last one wins"
// conflict that manifest-merger resolves in the consumer's favor (consumer
// is the merge root) and surfaces a warning.
//
// AC-7 spec: "the plugin itself does not fail but the warning is visible".
// audit-warning job exits 0 (does NOT fail) but emits a workflow warning
// annotation pointing to the consumer's manifest line that re-enabled
// backup.

plugins {
    id("com.android.application")
}

android {
    namespace = "com.godotengine.godot_solana_sdk.synthetic.warning"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.godotengine.godot_solana_sdk.synthetic.warning"
        // minSdk = 24 — see ../synthetic-consumer/build.gradle.kts for rationale
        // (transitive clientlib-ktx floor).
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
}

dependencies {
    implementation(project(":plugin"))
}
