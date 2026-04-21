// Story 1-4 AC-5 fixture — every line is an intentional violation of pattern
// 5 (ifdef-android-outside-selector). Self-test run asserts pattern 5 fires.
#ifdef __ANDROID__
#ifndef __ANDROID__
#if defined(__ANDROID__)
#if !defined(__ANDROID__)
