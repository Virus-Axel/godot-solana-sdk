// Story 1-5 Task 6 fixture — intentional violations of pattern 7
// (mwa-testing-define). Self-test run asserts pattern 7 fires.
//
// MWA_TESTING is a scons-tests-only compile define set via
// `tests_env.Append(CPPDEFINES=["MWA_TESTING"])` at SConstruct. Zero
// legitimate source-level `#define MWA_TESTING` exist — a directive at
// source level would activate the test-mode drain queue + the
// set_bridge_for_testing symbol in release builds, which is an immediate
// behavioral regression.
//
// Pattern 7 matches only the DEFINITION directive. `#ifdef MWA_TESTING`
// (testing for the flag — the legitimate usage all over the production
// code under src/mwa/) is NOT matched. Each line below is a shape the
// pattern MUST catch.
#define MWA_TESTING
#define MWA_TESTING 1
#  define MWA_TESTING 1
