package com.godotengine.godot_solana_sdk.mwa.util

// Compile-time sentinel used by Task 6 to prove that R8's
// -assumenosideeffects rule on SdkLog.v/d strips both call sites and lambda
// bodies in the release AAR. The three unique strings below must:
//   - MWA_R8_SENTINEL_VERBOSE_64cf3a1b  --> 0 occurrences (verbose stripped)
//   - MWA_R8_SENTINEL_DEBUG_9b2e07f4    --> 0 occurrences (debug stripped)
//   - MWA_R8_SENTINEL_INFO_KEEP_4a11c8dd --> >=1 occurrences (info retained,
//                                            confirms strip is selective not
//                                            wholesale DCE of this class).
//
// exerciseLogs() is never invoked from production code -- the sole purpose of
// this class is to provide grep-able byte patterns inside the release AAR.
// `consumer-rules.pro` `-keep class ...R8Sentinel { *; }` prevents R8's
// default dead-code-elimination pass from removing the class wholesale.
//
// Object-level @Suppress: nothing in the module references R8Sentinel, so
// IntelliJ / Android Studio's "unused declaration" inspection would otherwise
// flag the object identifier itself (in addition to the method below) and a
// cleanup-pass maintainer could delete the whole object, silently breaking
// the Task 6 R8 evidence. See exerciseLogs() comment for the same rationale
// at method level; both suppressions are required because the warning is
// emitted at both declaration levels independently.
@Suppress("unused")
internal object R8Sentinel {
    // Intentionally never called from production code -- exerciseLogs() exists
    // ONLY to plant grep-able sentinel strings inside the release AAR that the
    // Task 6 decompile evidence can pattern-match against. IntelliJ / Android
    // Studio's "unused declaration" inspection will otherwise flag this method
    // with a yellow gutter warning, and a future maintainer doing a cleanup
    // pass would happily delete it, silently breaking the R8 strip evidence
    // on subsequent releases. @Suppress hard-pins that KDoc intent into the
    // IDE's inspector so the cleanup pass does not touch this method. Do not
    // remove either the annotation or the method without also updating the
    // / Task 6 verification protocol and docs/amendments.md.
    @Suppress("unused")
    fun exerciseLogs(corrId: String) {
        SdkLog.v("MWA", corrId) { "MWA_R8_SENTINEL_VERBOSE_64cf3a1b" }
        SdkLog.d("MWA", corrId) { "MWA_R8_SENTINEL_DEBUG_9b2e07f4" }
        SdkLog.i("MWA", corrId) { "MWA_R8_SENTINEL_INFO_KEEP_4a11c8dd" }
    }
}
