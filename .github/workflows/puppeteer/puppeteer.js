const puppeteer = require('puppeteer');

const PLUGIN_PATH = process.env.GITHUB_WORKSPACE + '/solana-wallet-mock/extension';


(async () => {
    try {
        const browser = await puppeteer.launch({
            headless: true,
            //executablePath: '/usr/bin/google-chrome',
            ignoreDefaultArgs: false,
            args: [
                '--no-sandbox',
                '--disable-extensions-except=' + PLUGIN_PATH,
                '--load-extension=' + PLUGIN_PATH,
            ]
        });

        const gamePage = await browser.newPage();

        // Listen for "console" event and print messages to the node console.
        gamePage.on('console', msg => {
            for (let i = 0; i < msg.args().length; ++i) {
                console.log(`${i}: ${msg.args()[i]}`);
            }
        });

        // Navigate to the game webpage.
        await gamePage.goto('http://localhost:8060/tmp_js_export.html');

        // Timeout before closing the browser.
        await new Promise(resolve => setTimeout(resolve, 50000));
        await browser.close();
    }
    catch (e) {
        console.log(e)
        console.log("FAIL")
    }
})();
