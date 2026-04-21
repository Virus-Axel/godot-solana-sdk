import com.android.build.gradle.internal.tasks.factory.dependsOn

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
    id("org.jlleitschuh.gradle.ktlint")
}

val pluginName = "WalletAdapterAndroid"

val pluginPackageName = "plugin.walletadapterandroid"

android {
    namespace = pluginPackageName
    compileSdk = 34

    buildFeatures {
        buildConfig = true
        compose = true
    }
    composeOptions {
        kotlinCompilerExtensionVersion = "1.4.0"
    }

    defaultConfig {
        minSdk = 23
        targetSdk = 34

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

        // Ship the R8 strip rules to downstream AAR consumers (architecture.md §8.5,
        // amended A-6). See consumer-rules.pro for the rule set + rationale.
        consumerProguardFiles("consumer-rules.pro")
    }

    buildTypes {
        release {
            // CR-7 (Story 1-2 Task 6): AGP's default release build type has
            // isMinifyEnabled=false, so R8 never runs during `:plugin:assembleRelease`
            // and the -assumenosideeffects rule in consumer-rules.pro has zero effect
            // on the AAR. Without this block the Task 6 sentinel grep returns
            // `VERBOSE=1, DEBUG=1, INFO=1` (false negative) even with a correctly
            // written consumer-rules.pro.
            //
            // The second proguardFiles argument ("consumer-rules.pro") reuses the
            // same rule set so the AAR's own release build honors the
            // -assumenosideeffects rule during assembly (not just downstream consumer
            // apps via consumerProguardFiles above).
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "consumer-rules.pro",
                // library-self-only rules (e.g. -dontobfuscate) kept out of
                // consumer-rules.pro so downstream apps still get full R8
                // obfuscation. See proguard-library-self.pro header for
                // rationale (ci/verify_r8_strip.sh Gate 2 obfuscation blind
                // spot).
                "proguard-library-self.pro",
            )
        }
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

            // Gradle's default unit-test working directory is the subproject's
            // directory (`android/plugin/`), so a naive `File("testdata/mwa-fixtures")`
            // in a test would resolve to `android/plugin/testdata/mwa-fixtures`
            // which does not exist. Pass the absolute repo-root path via a
            // system property so tests read `File(System.getProperty("mwa.fixtures.dir"))`
            // deterministically regardless of CWD. See Story 1-6 Design Decision 6.
            it.systemProperty(
                "mwa.fixtures.dir",
                rootDir.parentFile.resolve("testdata/mwa-fixtures").absolutePath,
            )
        }
    }

    // Register the codegen output directory from Story 1-1 (`tools/gen_error_codes.py`)
    // as a main-source-set root so the generated `MwaError` sealed class is visible to
    // the plugin's Kotlin code. Story 1-1 (line 334) noted this wiring belonged to
    // Story 1-2, but it was missed there — Story 1-6 Task 1 is the first code to
    // reference `MwaError` from plugin Kotlin, exposing the gap.
    sourceSets.getByName("main") {
        java.srcDirs("src/generated/kotlin")
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
