package com.godotengine.godot_solana_sdk.mwa.plugin

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.fail
import java.io.File

/**
 * Story 4-1 T6 — AC-5 structural enforcement.
 *
 * Reads `GDExtensionAndroidPlugin.kt` from disk and asserts that the
 * `mwaDeauthorize` body contains EXACTLY ONE `finally {` block, and that
 * inside that finally block (brace-balanced range), BOTH the wipe operations
 * appear textually:
 *   - `store.deleteToken(...)` regex match
 *   - `sessionState.clear(...)` regex match
 *
 * AC-NFR-SEC-5 invariant: local cache MUST be cleared regardless of remote
 * outcome. The `finally` block enforces this at runtime; this test enforces
 * it at code-review time — a future maintainer cannot inadvertently move
 * the wipe into the `try` block (where a remote-revoke failure would skip
 * it) without CI catching the regression.
 *
 * Source-grep approach (per story T6, after C-4-1-C resolution): regex on
 * brace-balanced byte ranges. NO PSI/AST library dependency added — Kotlin
 * compiler embeddable PSI is ~30MB transitive bloat for one test. The
 * brace-counting parser handles nested braces (e.g. `try {} catch {}
 * finally { ... try { wipe } catch { ... } }` shape — DD-4-1-3's nested
 * defensive try inside the outer finally) by tracking depth and ignoring
 * braces inside string/char literals.
 *
 * Path resolution reuses Story 3-1 T1's `mwa.plugin.source.root` system
 * property (wired in `android/plugin/build.gradle.kts:160-163`). The
 * property points at `<projectDir>/src/main`, so the relative file path is
 * `java/plugin/walletadapterandroid/GDExtensionAndroidPlugin.kt`.
 *
 * Failure messages cite byte offsets so a maintainer can `vim +<offset>`
 * (or use any editor's go-to-byte) straight to the regression line.
 */
class DeauthorizeStructuralTest {

    private val pluginSourceRoot: File by lazy {
        val prop = System.getProperty("mwa.plugin.source.root")
            ?: error(
                "system property 'mwa.plugin.source.root' must be set " +
                    "(wired in android/plugin/build.gradle.kts via Story 3-1 T1)",
            )
        File(prop).also { require(it.isDirectory) { "plugin source root missing: $it" } }
    }

    @Test
    fun `wipe operations are inside finally block in mwaDeauthorize`() {
        val sourceFile = File(
            pluginSourceRoot,
            "java/plugin/walletadapterandroid/GDExtensionAndroidPlugin.kt",
        )
        require(sourceFile.isFile) {
            "source file missing: ${sourceFile.absolutePath} — " +
                "verify build.gradle.kts wiring of mwa.plugin.source.root"
        }
        val source = sourceFile.readText()

        // Locate `fun mwaDeauthorize(` at method-level indent (4 spaces).
        val sigPattern = Regex("""^    fun mwaDeauthorize\(""", RegexOption.MULTILINE)
        val sigMatch = sigPattern.find(source)
            ?: fail("mwaDeauthorize signature not found via regex `^    fun mwaDeauthorize\\(`")

        // Find the body's opening brace (first '{' after the signature).
        val bodyOpenIdx = source.indexOf('{', sigMatch.range.first)
        require(bodyOpenIdx >= 0) {
            "no opening brace after mwaDeauthorize signature at offset ${sigMatch.range.first}"
        }

        // Brace-balanced walk to find the body's closing brace.
        val bodyEndIdx = findMatchingBrace(source, bodyOpenIdx)
        require(bodyEndIdx > bodyOpenIdx) {
            "no matching closing brace for mwaDeauthorize body opened at offset $bodyOpenIdx"
        }
        val bodyContent = source.substring(bodyOpenIdx + 1, bodyEndIdx)

        // AC-5 part 1: exactly ONE `finally {` lexical occurrence in the body.
        // The idempotent path uses `try { wipe } catch` (no finally); the
        // else-branch's outer `try { remote } catch { } finally { try { wipe }
        // catch { } }` has the only finally.
        val finallyPattern = Regex("""\bfinally\s*\{""")
        val finallyMatches = finallyPattern.findAll(bodyContent).toList()
        assertEquals(
            1,
            finallyMatches.size,
            "AC-5: mwaDeauthorize body must contain exactly ONE `finally {` block " +
                "(found ${finallyMatches.size}). DD-4-1-3 enforces a single outer finally " +
                "in the else-branch; the idempotent path uses `try { wipe } catch` (no finally).",
        )

        val finallyOpenInBody = finallyMatches[0].range.last // index of '{' within bodyContent
        // Translate body-relative idx → absolute idx for findMatchingBrace.
        val finallyOpenAbs = bodyOpenIdx + 1 + finallyOpenInBody
        val finallyCloseAbs = findMatchingBrace(source, finallyOpenAbs)
        require(finallyCloseAbs > finallyOpenAbs) {
            "no matching closing brace for `finally {` opened at offset $finallyOpenAbs"
        }
        val finallyBlock = source.substring(finallyOpenAbs + 1, finallyCloseAbs)

        // AC-5 part 2: `store.deleteToken(...)` regex match inside the
        // finally-block byte range. The local `val store = storeProvider(
        // requireContext())` aliases the SecureTokenStore; `store.deleteToken`
        // is the production-code wipe call (pattern matches DD-4-1-6's
        // `store.listAllKeys().filter { ... }.forEach { store.deleteToken(it) }`).
        val deleteTokenPattern = Regex("""\bstore\.deleteToken\s*\(""")
        val deleteTokenMatch = deleteTokenPattern.find(finallyBlock)
            ?: fail(
                "AC-5: `store.deleteToken(` not found inside finally block of mwaDeauthorize. " +
                    "Finally block at byte offsets [${finallyOpenAbs + 1}..$finallyCloseAbs). " +
                    "AC-NFR-SEC-5 requires the on-disk wipe to be in the finally block — " +
                    "moving it into the try block would skip the wipe on remote-revoke failure.",
            )

        // AC-5 part 3: `sessionState.clear(...)` regex match inside finally.
        // DD-4-1-2 distinction: `clear()` is the FULL wipe (deauthorize path);
        // `clearOnLogout()` is the auth-only wipe (disconnect path, Story 2-3).
        // The regex matches `clear(` boundary — also matches `clearOnLogout(`,
        // so we explicitly verify the call is `clear(` and NOT `clearOnLogout(`.
        val sessionClearPattern = Regex("""\bsessionState\.clear\s*\(""")
        val clearMatches = sessionClearPattern.findAll(finallyBlock).toList()
        assertTrue(
            clearMatches.isNotEmpty(),
            "AC-5: `sessionState.clear(` not found inside finally block of mwaDeauthorize. " +
                "Finally block at byte offsets [${finallyOpenAbs + 1}..$finallyCloseAbs). " +
                "DD-4-1-2 requires the in-memory wipe to be in the finally block.",
        )
        // Disambiguate: must be `clear(`, NOT `clearOnLogout(` (which would be
        // the disconnect-path wipe and a DD-4-1-2 violation).
        clearMatches.forEach { match ->
            val nextChars = finallyBlock.substring(
                match.range.last + 1,
                minOf(match.range.last + 12, finallyBlock.length),
            )
            assertTrue(
                !nextChars.startsWith("nLogout") && !nextChars.startsWith("OnLogout"),
                "AC-5 / DD-4-1-2: deauthorize must use `sessionState.clear()` (full wipe), " +
                    "NOT `sessionState.clearOnLogout()` (which preserves identity for the " +
                    "disconnect path's silent-reauth contract). Found at byte offset " +
                    "${finallyOpenAbs + 1 + match.range.first}.",
            )
        }

        // AC-5 part 4: ordering check — `store.deleteToken` must appear BEFORE
        // `sessionState.clear` in the finally block per DD-4-1-2 step 1
        // (on-disk wipe first; surviving process death is the higher-stakes
        // wipe, so it goes first).
        val firstClearMatch = clearMatches.first()
        assertTrue(
            deleteTokenMatch.range.first < firstClearMatch.range.first,
            "AC-5 / DD-4-1-2: `store.deleteToken(` must appear BEFORE " +
                "`sessionState.clear(` in the finally block. Order matters: the on-disk " +
                "wipe survives process death and must complete first. Found deleteToken at " +
                "offset ${finallyOpenAbs + 1 + deleteTokenMatch.range.first}, " +
                "clear at offset ${finallyOpenAbs + 1 + firstClearMatch.range.first}.",
        )
    }

    /**
     * Walks `source` from the brace at [openIdx] to the matching close brace,
     * tracking depth. Skips braces inside string/char literals so e.g.
     * `"{}"` doesn't break the count. Returns the index of the matching `}`,
     * or -1 if none found before EOF.
     */
    private fun findMatchingBrace(source: String, openIdx: Int): Int {
        require(source[openIdx] == '{') { "openIdx must point at `{`" }
        var depth = 1
        var i = openIdx + 1
        while (i < source.length && depth > 0) {
            when (val c = source[i]) {
                '"' -> i = skipStringLiteral(source, i)
                '\'' -> i = skipCharLiteral(source, i)
                '/' -> {
                    // Skip line/block comments (which can contain unbalanced braces in text).
                    if (i + 1 < source.length) {
                        when (source[i + 1]) {
                            '/' -> {
                                i = source.indexOf('\n', i).let { if (it < 0) source.length else it + 1 }
                                continue
                            }
                            '*' -> {
                                val end = source.indexOf("*/", i + 2)
                                i = if (end < 0) source.length else end + 2
                                continue
                            }
                            else -> i++
                        }
                    } else {
                        i++
                    }
                }
                '{' -> {
                    depth++
                    i++
                }
                '}' -> {
                    depth--
                    if (depth == 0) return i
                    i++
                }
                else -> {
                    // Suppress unused-var warning while keeping `c` named for readability.
                    @Suppress("UNUSED_VARIABLE")
                    val unused = c
                    i++
                }
            }
        }
        return -1
    }

    /**
     * Walks past a `"..."` string literal starting at the opening quote.
     * Handles `\"` escapes and triple-quoted strings (`"""..."""`). Returns
     * the index AFTER the closing quote.
     */
    private fun skipStringLiteral(source: String, startIdx: Int): Int {
        require(source[startIdx] == '"')
        // Triple-quoted: `"""` opens; close is the next `"""`.
        if (startIdx + 2 < source.length &&
            source[startIdx + 1] == '"' &&
            source[startIdx + 2] == '"'
        ) {
            val end = source.indexOf("\"\"\"", startIdx + 3)
            return if (end < 0) source.length else end + 3
        }
        // Regular string: `"..."` with `\"` escapes.
        var i = startIdx + 1
        while (i < source.length) {
            when (source[i]) {
                '\\' -> i += 2 // skip escape + next char
                '"' -> return i + 1
                else -> i++
            }
        }
        return source.length
    }

    /** Walks past a `'.'` char literal. Handles `'\n'` etc. escapes. */
    private fun skipCharLiteral(source: String, startIdx: Int): Int {
        require(source[startIdx] == '\'')
        var i = startIdx + 1
        while (i < source.length) {
            when (source[i]) {
                '\\' -> i += 2
                '\'' -> return i + 1
                else -> i++
            }
        }
        return source.length
    }
}
