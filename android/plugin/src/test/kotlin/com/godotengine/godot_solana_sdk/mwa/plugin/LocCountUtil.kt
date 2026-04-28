package com.godotengine.godot_solana_sdk.mwa.plugin

/**
 * DD-3-1-9 + DD-3-2-2 — shared LOC counter for AC-2 budget assertions across signing-path stories.
 *
 * Extracted from [MwaAndroidPluginSignMessagesTest] (Story 3-1 T1) per Story 3-2 T1 PRE-STEP
 * (D-3-2-3, Rule 1) so the same counter rule is reused by Story 3-2's
 * [MwaAndroidPluginSignTransactionsTest] AC-2 budget test (and, transitively, Story 3-3's
 * sign_and_send AC-N budget test if that story keeps the same pattern). The helper is `internal`
 * top-level: same module, same package — visible to every signing-op AC-2 test without an import.
 *
 * Counter rule:
 * - Starts AFTER the signature opening line (excluding signature even if multi-line).
 * - Ends BEFORE the matching closing brace at the method's indentation level (4-space indent —
 *   class-method indent in this codebase's style).
 * - Excludes blank lines, line-comments (`//`), and block-comment lines (`*` continuations).
 * - Excludes kdoc above the signature (already excluded by start-after-signature).
 */
internal fun countMethodLines(source: String, sigStartIndex: Int): Int {
    // Find end of signature line (first '{' or end of opening line — methods open
    // with `... timeoutMs: Long) {` in this codebase's style).
    var idx = sigStartIndex
    while (idx < source.length && source[idx] != '{') idx++
    require(idx < source.length) { "no opening brace found after signature" }
    // Skip past `{` and following newline.
    idx++
    while (idx < source.length && source[idx] != '\n') idx++
    idx++

    // Walk lines until we hit the closing brace at the method's 4-space indent
    // (line that starts with `    }` and nothing else — accounting for trailing newline).
    val methodIndent = "    "
    var loc = 0
    while (idx < source.length) {
        val lineEnd = source.indexOf('\n', idx).let { if (it < 0) source.length else it }
        val line = source.substring(idx, lineEnd)
        // Match the closing brace: exactly the method indent + `}` + optional trailing whitespace.
        if (line == "$methodIndent}" || line == "$methodIndent} ") break
        val trimmed = line.trim()
        val isBlank = trimmed.isEmpty()
        val isLineComment = trimmed.startsWith("//")
        val isBlockComment = trimmed.startsWith("/*") || trimmed.startsWith("*")
        if (!isBlank && !isLineComment && !isBlockComment) loc++
        idx = lineEnd + 1
    }
    return loc
}
