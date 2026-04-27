import com.android.build.gradle.internal.tasks.factory.dependsOn

plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
    id("org.jlleitschuh.gradle.ktlint")
}

val pluginName = "WalletAdapterAndroid"

val pluginPackageName = "plugin.walletadapterandroid"

// Story 4-3 T5 (AC-6 / DD-4-3-4) — pin transitive deps to specific resolved
// versions via gradle.lockfile + per-artifact SHA-256 verification via
// gradle/verification-metadata.xml. PGP signatures are best-effort
// (DD-4-3-4.a — some long-tail jars like org.json:json have no Maven Central
// signature). Regenerate both files on dep bumps via:
//   ./gradlew :plugin:assembleRelease :plugin:dependencies \
//       --write-locks --write-verification-metadata pgp,sha256 --refresh-dependencies
// (full procedure in docs/dev/lockfile-regeneration.md). DO NOT place this
// block inside `android { }` — that form does not exist (C-4-3-D); the AGP
// DSL does NOT recognize `dependencyLocking { ... }` and would fail at
// script-resolve time.
//
// `lockFile` is overridden to `$rootDir/gradle.lockfile` (i.e. android/gradle.lockfile)
// per Story 4-3 §Tasks T5 must_have ("android/gradle.lockfile exists"). Default
// would write to `$projectDir/gradle.lockfile` (i.e. android/plugin/gradle.lockfile).
dependencyLocking {
    lockAllConfigurations()
    lockFile.set(file("$rootDir/gradle.lockfile"))
}

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
            // Story 2-1 CI fixup round 3c (D-CI-5, Rule 1): x86_64 alongside
            // arm64-v8a so the androidTest APK installs on GitHub's ubuntu +
            // KVM x86_64 emulator (D-F5). Without x86_64 in the filter,
            // transitive native libs (via godot-lib-android) are packaged
            // arm64-only and `adb install` on the emulator fails with
            // `INSTALL_FAILED_NO_MATCHING_ABIS: res=-113 (Failed to extract
            // native libraries)`. The plugin AAR itself has no native code;
            // the release pipeline stays arm64-primary — downstream Godot
            // games re-filter per their own build targets.
            abiFilters.add("arm64-v8a")
            abiFilters.add("x86_64")
        }

        manifestPlaceholders["godotPluginName"] = pluginName
        manifestPlaceholders["godotPluginPackageName"] = pluginPackageName
        buildConfigField("String", "GODOT_PLUGIN_NAME", "\"${pluginName}\"")
        setProperty("archivesBaseName", pluginName)

        // Ship the R8 strip rules to downstream AAR consumers (architecture.md §8.5,
        // amended A-6). See consumer-rules.pro for the rule set + rationale.
        consumerProguardFiles("consumer-rules.pro")

        // Story 2-1 T9 — androidTest suite (AC-1 plaintext scan, AC-3/5/6 on
        // real coroutines + real EncryptedSharedPreferences + real Keystore,
        // AC-7 fingerprint idempotence, AC-8 logcat+filesystem scan). The Fake
        // Wallet APK route (installed via adb) is replaced by the in-process
        // FakeMwaClient injected through the @VisibleForTesting ctor — same
        // pattern as T4 unit tests (D-T9-1 Rule 2). AC-4 no-wallet scenario is
        // exercised via FakeMwaClient.withScenario("no_wallet") rather than a
        // real uninstall-before-test step.
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
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
        // Story 2-1 T10 fixup (D-F1 Rule 1): lower Kotlin bytecode target from
        // 17 to 11 so sealed-class output does NOT carry the Java 17
        // `PermittedSubclasses` class-file attribute. AGP 7.4.1's bundled R8
        // rejects `PermittedSubclasses` under `minifyReleaseWithR8` when the
        // output target is class files (library/AAR mode) — the exact error
        // is "Sealed classes are not supported as program classes when
        // generating class files". Bundled R8 can't be upgraded without
        // bumping AGP past 7.4.1 (which the plan currently locks, per Story
        // 1-2 §Tooling Notes + D10 / CR-5). Sealed-class semantics stay
        // identical at Kotlin bytecode level 11 (metadata-only
        // representation); runtime behavior unchanged. Revisit alongside the
        // AGP 8.x + Kotlin plugin 2.x upgrade pair.
        jvmTarget = "11"
    }

    // Story 2-1 CI fixup round 3 (D-CI-4, Rule 1): exclude META-INF license/notice
    // files from the merged androidTest APK. Six transitive junit-jupiter jars
    // (junit-jupiter-5.8.2, junit-jupiter-api-5.8.2, junit-jupiter-engine-5.8.2,
    // junit-jupiter-params-5.8.2, junit-platform-commons-1.8.2,
    // junit-platform-engine-1.8.2) — pulled in by the androidTest runtime —
    // each ship their own `META-INF/LICENSE.md`, causing
    // `DuplicateRelativeFileException` in `:plugin:mergeDebugAndroidTestJavaResource`.
    // The LICENSE/NOTICE files are informational and not required at runtime;
    // excluding them is the standard AGP remedy. Also excluding the common
    // paired files (LICENSE-notice.md, NOTICE.md, AL2.0, LGPL2.1) to avoid
    // re-hitting this in future dependency updates.
    packagingOptions {
        resources {
            excludes += "META-INF/LICENSE.md"
            excludes += "META-INF/LICENSE-notice.md"
            excludes += "META-INF/NOTICE.md"
            excludes += "META-INF/AL2.0"
            excludes += "META-INF/LGPL2.1"
        }
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

            // Story 3-1 T1 — AC-1 LOC-budget test reads `GDExtensionAndroidPlugin.kt`
            // source at runtime to assert `mwaSignMessages` body is ≤20 LOC. Standard
            // Gradle test classpath contains compiled `.class` files only, not `.kt`
            // sources. This system property exposes the absolute path to the plugin's
            // `src/main` directory so the test can read source files via
            // `File(System.getProperty("mwa.plugin.source.root"), "java/...").readText()`.
            // System-property pattern (vs. `sourceSets.test.resources.srcDir(...)`) keeps
            // .kt source out of the test JAR — referenced only by absolute path during
            // test execution. Same pattern as `mwa.fixtures.dir` above (Story 1-6 D-6).
            it.systemProperty(
                "mwa.plugin.source.root",
                project.projectDir.resolve("src/main").absolutePath,
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

    // Story 1-6 Task 3 — FakeMwaClient parses testdata/mwa-fixtures/*.json. org.json
    // is pure-Java (no Kotlin-version coupling; no kotlinx-serialization compiler
    // plugin required) and works identically in :plugin:testDebugUnitTest JVM runs
    // and Android runtime. FakeMwaClient is test-only but ships in src/main/ per
    // Story 1-6 Dev Notes so Epic 2+ test source-sets can consume it.
    implementation("org.json:json:20231013")

    // Story 2-1 Task 1 (D-T1-1, Rule 2) — Keystore-backed MasterKey + EncryptedSharedPreferences
    // for SecureTokenStore (arch §8.3). 1.1.0-alpha06 is the Google-documented EncryptedSharedPreferences
    // API surface; 1.0.0 is stale by five years. The alpha tag is load-bearing but API-stable in practice
    // (no breaking changes since alpha01). Transitively brings in tink-android (shaded Tink), which
    // exposes com.google.crypto.tink.shaded.protobuf.InvalidProtocolBufferException for the
    // corrupt-recovery catch path in SecureTokenStore.
    implementation("androidx.security:security-crypto:1.1.0-alpha06")

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

    // Story 2-1 T9 — androidTest (instrumented tests on emulator).
    //
    // JUnit 4 (not 5) is the AndroidX Test runner's supported framework. AGP 7.x
    // ships no @RunWith(AndroidJUnit4::class) adapter for Jupiter, and building
    // a custom one is out of scope. T9 uses JUnit 4 idioms for androidTest;
    // unitTest (JVM) keeps JUnit 5. D-T9-2 (Rule 1) logs the split.
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test:runner:1.5.2")
    androidTestImplementation("androidx.test:rules:1.5.0")
    // mockk-android packages the agent for device runtime; same 1.13.13 pin
    // (Kotlin 1.8 compatibility) as the unit-test plate.
    androidTestImplementation("io.mockk:mockk-android:1.13.13")
    // Coroutines test utilities (runTest, TestScope) — AC-3/5/6 exercise
    // suspend paths under realistic dispatchers on device.
    androidTestImplementation("org.jetbrains.kotlinx:kotlinx-coroutines-test:1.7.3")
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
