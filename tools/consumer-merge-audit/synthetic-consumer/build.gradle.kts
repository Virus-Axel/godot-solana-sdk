// Story 4-3 T6 — synthetic-consumer (AC-2 positive case)
//
// Minimal Android application that depends on the WalletAdapterAndroid plugin
// via the multi-project `:plugin` reference. The consumer's AndroidManifest.xml
// declares NO `android:allowBackup` attribute on `<application>`, so the
// plugin's `android:allowBackup="false"` (with `tools:replace`) is the merged
// value. The merged release-variant manifest at
// `build/intermediates/merged_manifest/release/AndroidManifest.xml` is the
// AC-2 evidence target — `manifest_merge_audit.yml::audit-positive` greps it
// for `allowBackup="false"`.
//
// Why an Application (not Library): library manifests do not run the full
// final-merge phase that an end-consumer would; manifest-merger emits its
// "consumer wins" / "library wins" logic only when an Application is the
// merge root. AC-2 is precisely about end-consumer behavior, so we model an
// end-consumer.

plugins {
    id("com.android.application")
}

android {
    namespace = "com.godotengine.godot_solana_sdk.synthetic.consumer"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.godotengine.godot_solana_sdk.synthetic.consumer"
        // minSdk = 24 (not 23) to satisfy the transitive clientlib-ktx 2.0.3
        // declared minSdk = 24. The plugin itself ships minSdk = 23 because
        // an Android *library* manifest does not enforce the floor — only
        // *application* manifests do. The synthetic-consumer is an application,
        // so it inherits the strictest transitive floor. End consumers in the
        // Godot ecosystem (which target Godot's own minSdk) ship at minSdk
        // ≥ 24 in practice (Godot 4.x default), so this does not narrow the
        // plugin's effective surface — it just makes the synthetic merge
        // representative of a real downstream app.
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"
    }

    buildTypes {
        release {
            // Disable R8 — we are exercising the manifest-merger phase only,
            // not the codegen/minify phase. Keeps the build cheap.
            isMinifyEnabled = false
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
}

dependencies {
    // Depend on the plugin AAR via the multi-project reference. This pulls in
    // the plugin's hardened AndroidManifest.xml as a manifest-merger input.
    implementation(project(":plugin"))
}
