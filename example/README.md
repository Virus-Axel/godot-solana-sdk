# Examples

The examples are built and run as tests in github workflows. The scripts in the examples are proved to work as intended before each commit.

## Setup

Place a bin folder with godot-solana-sdk and restart Godot. The projects default to use a local Solana cluster. Change it in project settings.

## Developing

To create an example make sure you include the implementation of PASS and timeout timer. Each test should have its ID in PASS and TOTAL_CASES should reflect the total number of PASS calls (see existing example projects).

### Test Accounts
A test validator will be started when running the example projects in github workflows. The test validator will include accounts from the directory *res://test_accounts*. The files names should be *<ACCOUNT_ADDRESS>.json* with the account content.

### Web targets
If the project name ends with *"Web"* it will be run in a chrome browser. In this case, a [puppeteer script](https://github.com/Virus-Axel/godot-solana-sdk/blob/master/.github/workflows/puppeteer/puppeteer.js) will run to interact with the browser.
