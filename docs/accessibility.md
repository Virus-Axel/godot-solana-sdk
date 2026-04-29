---
Owner: @ValentinVPK
Drift-check: verify_docs.yml
---

# Accessibility Guide

This document covers accessibility considerations for Godot games consuming the MWA SDK. The SDK itself is GDScript-callable and emits no UI; the accessibility surface lives in **your game** — the buttons that drive `MWA.connect()`, the log areas that render `mwa_error.user_message`, the cluster-selection UI. This guide describes the patterns; [`../REVIEW.md`](../REVIEW.md) hosts the canonical reference implementation in the example repo.

## 1. Godot 4.3 accessibility surface

Godot 4.3 ships an in-progress accessibility track. The most relevant addition for MWA-consuming games is `Button.accessible_name` — a Control-level property that screen readers (Android TalkBack) announce when the focused control is a Button.

**Version caveat (`CR-5-4-B`).** [`../REVIEW.md`](../REVIEW.md) Design guidance (line 171) marks the exact GDScript property name (`accessible_name` vs `accessibility_name`) as `⚠ VERSION NOT VERIFIED` — the name depends on the 4.3.x point release. Confirm via `Object.get_property_list()` on a Button instance, or check the Godot release notes for your pinned version. The example repo's CI is the authoritative version-verification step per `A-17` (out-of-LaiM-scope).

The example repo's main scene wires nine buttons with descriptive accessible names per [`../REVIEW.md`](../REVIEW.md) Design guidance (line 159). The pattern, paraphrased:

```gdscript
$ConnectButton.accessible_name = "Connect to wallet"
$DisconnectButton.accessible_name = "Disconnect from wallet"
$ForgetAllButton.accessible_name = "Cancel pending op + GDPR wipe"
# ... etc per the 9-button list in REVIEW.md
```

Button labels alone (`[Connect]`) are insufficient — TalkBack reads the visible text, but a button labelled `[Sign and Send]` benefits from the more descriptive `accessible_name = "Sign and submit a test transaction"` for users who need explicit verb-object context.

## 2. WCAG AA contrast

Target ratios from WCAG 2.1 AA:

- **Normal text:** 4.5:1 contrast ratio against the background.
- **Large text (≥18pt or ≥14pt bold):** 3:1.
- **UI components and graphics:** 3:1.

Godot's default dark theme passes WCAG AA by-construction for the standard Control nodes — buttons, labels, and panels render with contrast ratios above 4.5:1 against the default panel background. This is per [`../REVIEW.md`](../REVIEW.md) Design guidance.

If your game ships a **custom theme**, verify contrast manually:

- Use a contrast-checking tool (e.g., browser dev-tools, dedicated contrast extensions) on a screenshot of each MWA-driven UI surface.
- Pay special attention to the log area that renders `mwa_error.user_message` — it must remain legible across all error states.
- Avoid theme-color choices that look fine for sighted users but fail contrast for low-vision users (light gray on white, dark navy on black).

## 3. Text-not-color rule for `mwa_error`

Per [`./stories/5-4.md`](./stories/5-4.md) `AC-4` and [`../REVIEW.md`](../REVIEW.md) Design guidance, every `mwa_error` event MUST render `payload["user_message"]` as **text** in the in-game log area. **Color is decoration, not the only signal.**

Wrong:

```gdscript
func _on_mwa_error(payload: Dictionary) -> void:
    $LogArea.modulate = Color.RED
    # ⚠ no text — colorblind users have no way to know an error happened
```

Right:

```gdscript
func _on_mwa_error(payload: Dictionary) -> void:
    var msg: String = payload["user_message"]
    $LogArea.append_text("[error] %s\n" % msg)
    # Optional decoration: also tint red. Text remains the primary signal.
    $LogArea.modulate = Color.RED
```

The 10-key `mwa_error` payload (per [`./amendments.md`](./amendments.md) `A-14`) carries `user_message` (human-readable, safe for in-game display) and `developer_details` (technical, safe for logs). Render `user_message` to the player; log `developer_details` for support diagnostics. See [`./error-codes.md`](./error-codes.md) for the canonical default user_message strings per code.

## 4. Keyboard navigation

Godot 4.3 Control nodes have a `focus_mode` property. Set it to `FOCUS_ALL` for any Control the user must reach via keyboard (this is the default for Button, but custom controls may default to `FOCUS_NONE`).

Verify:

- **Tab order is logical.** Configure each Control's `focus_neighbor_*` properties so Tab progresses in reading order (top-to-bottom, left-to-right in LTR locales).
- **Enter / Space activates buttons.** Default behavior in Godot 4.3 — verify no custom `_input()` handler is shadowing.
- **Escape dismisses dialogs.** The cluster-confirmation dialog (`"I understand — use mainnet"` per [`../REVIEW.md`](../REVIEW.md) Design guidance) should respond to Escape as a Cancel.

For the 9-button main scene in the example repo, the implicit tab order matches the visual order (`[Connect]` → `[Disconnect]` → ... → `[Forget All]`); no `focus_neighbor_*` overrides are required. Custom layouts may need explicit configuration.

## 5. Screen reader compatibility

Android TalkBack is the primary Android screen reader. Godot 4.3's accessibility track is **incomplete** — full TalkBack support is an ongoing upstream effort. Today's reality:

- `Button.accessible_name` (Section 1) provides the announcement string.
- Focus changes are announced by TalkBack when `focus_mode = FOCUS_ALL`.
- Live-region announcements (e.g., "transaction sent") for non-focused log updates are NOT yet broadly supported in Godot 4.3; consumers may need to drive focus to a hidden Label as a workaround.

Track upstream Godot 4.3 accessibility work in the Godot release notes for your pinned version. The example repo's CI (per [`../REVIEW.md`](../REVIEW.md) Section CI status) covers AC-4 — scene loads in 4.3.x with screen-reader-visible accessible names — but does NOT exercise full TalkBack interaction. That coverage gap is acceptable for this SDK's scope (the SDK is GDScript-callable; it is the **game** that owns its accessibility); if your game targets accessibility-first audiences, plan a manual TalkBack pass on a real Android device.

## See also

- [`../REVIEW.md`](../REVIEW.md) Design guidance — canonical 9-button list with `accessible_name` strings, mainnet-confirmation dialog, text-not-color rule.
- [`./error-codes.md`](./error-codes.md) — `user_message` field per code, the text rendered in the log area.
- [`./usage.md`](./usage.md) — error-handling patterns including the `mwa_error` text-rendering snippet.
- [`./concerns.md`](./concerns.md) `CR-5-4-B` — Godot 4.3.x point-release verification of `Button.accessible_name` property name.
- Godot 4.3 release notes — upstream accessibility track progress (check the version pinned in [`./compatibility-matrix.md`](./compatibility-matrix.md)).
