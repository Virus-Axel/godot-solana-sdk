import com.android.build.gradle.internal.tasks.factory.dependsOn

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
}

val pluginName = "WalletAdapterAndroid"

val pluginPackageName = "plugin.walletadapterandroid"

android {
    namespace = pluginPackageName
    compileSdk = 33

    buildFeatures {
        buildConfig = true
        compose = true
    }
    composeOptions {
        kotlinCompilerExtensionVersion = "1.4.0"
    }

    defaultConfig {
        minSdk = 21

        externalNativeBuild {
            cmake {
                cppFlags("")
            }
        }
        ndk {
            abiFilters.add("arm64-v8a")
        }

        manifestPlaceholders["godotPluginName"] = pluginName
        manifestPlaceholders["godotPluginPackageName"] = pluginPackageName
        buildConfigField("String", "GODOT_PLUGIN_NAME", "\"${pluginName}\"")
        setProperty("archivesBaseName", pluginName)
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }

    testOptions {
        unitTests.all {
            it.useJUnitPlatform()
        }
    }
}

dependencies {
    implementation("org.godotengine:godot:4.3.0.stable")
    implementation("com.solanamobile:mobile-wallet-adapter-clientlib-ktx:2.0.3")
    implementation("com.solanamobile:rpc-core:0.2.8")
    implementation("com.solanamobile:rpc-solana:0.2.8")
    implementation("com.solanamobile:rpc-ktordriver:0.2.8")
    implementation("androidx.compose.ui:ui:1.4.3")
    implementation("androidx.compose.material:material:1.4.3")
    implementation("androidx.compose.ui:ui-tooling-preview:1.4.3")
    implementation("androidx.activity:activity-compose:1.8.0")

    // Plan (Story 1-2 §Tooling Notes) pins JUnit Jupiter 6.0.3, but 6.x is compiled with Kotlin 2.1
    // metadata and fails compileDebugUnitTestKotlin under this module's Kotlin plugin 1.8.0:
    //   "Module was compiled with an incompatible version of Kotlin.
    //    The binary version of its metadata is 2.1.0, expected version is 1.8.0."
    // 5.11.4 is the newest 5.x (last Kotlin-1.x-compatible) release; logged as D1 (Rule 1) under
    // Story 1-2 §Deviations Expected. Revisit after Kotlin plugin 2.x upgrade (tracked in concerns.md).
    testImplementation("org.junit.jupiter:junit-jupiter:5.11.4")

    // Plan §Tooling Notes pins MockK 1.14.9, but 1.14.x is compiled with Kotlin 2.x metadata and
    // hits the same `"expected version is 1.8.0"` failure as JUnit 6.x above. 1.13.13 is the last
    // widely-deployed Kotlin-1.x-compatible MockK release; used for mocking `android.util.Log`
    // statics in SdkLogTest. Logged as D7 (Rule 1) under Story 1-2 §Deviations Expected; unblocks
    // alongside JUnit 6.x when Kotlin plugin bumps to 2.x (tracked in concerns.md CR-5).
    testImplementation("io.mockk:mockk:1.13.13")
}

// BUILD TASKS DEFINITION
val copyDebugAARToDemoAddons by tasks.registering(Copy::class) {
    description = "Copies the generated debug AAR binary to the plugin's addons directory"
    from("build/outputs/aar")
    include("$pluginName-debug.aar")
    into("$pluginName/bin/debug")
}

val copyReleaseAARToDemoAddons by tasks.registering(Copy::class) {
    description = "Copies the generated release AAR binary to the plugin's addons directory"
    from("build/outputs/aar")
    include("$pluginName-release.aar")
    into("$pluginName/bin/release")
}

val cleanDemoAddons by tasks.registering(Delete::class) {
    delete("$pluginName")
}

val copyAddonsToDemo by tasks.registering(Copy::class) {
    description = "Copies the export scripts templates to the plugin's output directory"

    dependsOn(cleanDemoAddons)
    finalizedBy(copyDebugAARToDemoAddons)
    finalizedBy(copyReleaseAARToDemoAddons)
}

tasks.named("assemble").configure {
    finalizedBy(copyAddonsToDemo)
}

tasks.named<Delete>("clean").apply {
    dependsOn(cleanDemoAddons)
}