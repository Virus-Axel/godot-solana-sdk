package com.godotengine.godot_solana_sdk.mwa.util

import android.util.Log
import io.mockk.every
import io.mockk.mockkStatic
import io.mockk.unmockkStatic
import io.mockk.verify
import org.junit.jupiter.api.AfterEach
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.BeforeEach
import org.junit.jupiter.api.Test
import java.lang.reflect.Modifier

class SdkLogTest {

    @BeforeEach
    fun setup() {
        mockkStatic(Log::class)
        every { Log.v(any<String>(), any<String>()) } returns 0
        every { Log.d(any<String>(), any<String>()) } returns 0
        every { Log.i(any<String>(), any<String>()) } returns 0
        every { Log.w(any<String>(), any<String>()) } returns 0
        every { Log.e(any<String>(), any<String>()) } returns 0
    }

    @AfterEach
    fun teardown() {
        unmockkStatic(Log::class)
    }

    @Test
    fun `SdkLog d evaluates msg lambda exactly once and calls Log d with corrId-prefixed message`() {
        var evalCount = 0
        SdkLog.d("MWA", "abc") {
            evalCount++
            "hello"
        }
        assertEquals(1, evalCount, "lambda must evaluate exactly once per call")
        verify(exactly = 1) { Log.d("MWA", "[corrId=abc] hello") }
    }

    @Test
    fun `SdkLog v calls Log v with corrId-prefixed message`() {
        SdkLog.v("MWA", "abc") { "hi" }
        verify(exactly = 1) { Log.v("MWA", "[corrId=abc] hi") }
    }

    @Test
    fun `SdkLog i calls Log i with corrId-prefixed message`() {
        SdkLog.i("MWA", "abc") { "hi" }
        verify(exactly = 1) { Log.i("MWA", "[corrId=abc] hi") }
    }

    @Test
    fun `SdkLog w calls Log w with corrId-prefixed message`() {
        SdkLog.w("MWA", "abc") { "hi" }
        verify(exactly = 1) { Log.w("MWA", "[corrId=abc] hi") }
    }

    @Test
    fun `SdkLog e calls Log e with corrId-prefixed message`() {
        SdkLog.e("MWA", "abc") { "hi" }
        verify(exactly = 1) { Log.e("MWA", "[corrId=abc] hi") }
    }

    @Test
    fun `SdkLog accepts empty corrId without crashing`() {
        SdkLog.d("MWA", "") { "message" }
        verify(exactly = 1) { Log.d("MWA", "[corrId=] message") }
    }

    @Test
    fun `all SdkLog log members are JvmStatic so R8 -assumenosideeffects rule matches`() {
        // R8's `-assumenosideeffects class ... { public static void v(...); public static void d(...); }`
        // rule in consumer-rules.pro (Task 6 / §8.5) is written against static method descriptors.
        // Kotlin `object SdkLog { fun v(...) }` compiles to instance dispatch via `SdkLog.INSTANCE.v(...)`
        // unless every member carries `@JvmStatic`, which generates the additional static-dispatch methods.
        // Without @JvmStatic the R8 rule silently matches nothing and nothing is stripped.
        val methods = SdkLog::class.java.declaredMethods
        val methodSummary = methods.joinToString(", ") { m -> m.name + "(static=" + Modifier.isStatic(m.modifiers) + ")" }
        setOf("v", "d", "i", "w", "e").forEach { name ->
            val isStatic = methods.any { it.name == name && Modifier.isStatic(it.modifiers) }
            assertTrue(
                isStatic,
                "SdkLog.$name must be @JvmStatic to match R8 -assumenosideeffects rule; declaredMethods=[$methodSummary]",
            )
        }
    }
}
