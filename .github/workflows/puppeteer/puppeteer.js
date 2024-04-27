const puppeteer = require('puppeteer');

const PLUGIN_PATH = process.env.GITHUB_WORKSPACE + '/backpack/packages/app-extension/build'
var PLUGIN_ID = ''

const CREATE_NEW_WALLET_PATH = '#options > span > span > div > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-16px._w-420px > button:nth-child(1)'
const ACCOUNT_NAME_PATH = '#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-40px._w-10037 > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._mih-0px._miw-0px._fs-1._pos-relative._jc-flex-start._fg-1 > span > input'
const ACCOUNT_NAME_CONFIRM_PATH = '#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-40px._w-10037 > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._maw-420px > button:nth-child(1)'
const UNDERSTAND_CHECKBOX_PATH = '#options > span > span > div > div > div > div > div._ai-center._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-40px > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-16px._w-420px > button.MuiButtonBase-root.MuiButton-root.MuiButton-text.MuiButton-textPrimary.MuiButton-sizeMedium.MuiButton-textSizeMedium.MuiButton-root.MuiButton-text.MuiButton-textPrimary.MuiButton-sizeMedium.MuiButton-textSizeMedium.css-ouv8db.css-v9coz5 > div:nth-child(1) > span > input'
const UNDERSTAND_CONFIRM_PATH = '#options > span > span > div > div > div > div > div._ai-center._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-40px > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-16px._w-420px > button.is_Button'
const COPIED_CHECKBOX_PATH = '#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-20px._w-10037 > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-4px > button > div:nth-child(1) > span > input'
const COPIED_CONFIRM_PATH = '#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-20px._w-10037 > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._jc-flex-end > button'
const SOLANA_PATH = '#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-40px._w-10037 > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-16px > button:nth-child(2) > div'
const PASS_PATH = '#options > span > span > div > div > div > div > form > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-16px > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._mih-0px._miw-0px._fs-0._pos-relative._w-10037 > span > input'
const PASS2_PATH = '#options > span > span > div > div > div > div > form > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._gap-16px > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._mih-0px._miw-0px._fs-0._pos-relative._jc-center > span > input'
const AGREE_TERMS_PATH = '#options > span > span > div > div > div > div > form > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-16px._maw-420px > button.MuiButtonBase-root.MuiButton-root.MuiButton-text.MuiButton-textPrimary.MuiButton-sizeMedium.MuiButton-textSizeMedium.MuiButton-root.MuiButton-text.MuiButton-textPrimary.MuiButton-sizeMedium.MuiButton-textSizeMedium.css-ouv8db.css-v9coz5 > div:nth-child(1) > span > input'
const PASS_CONFIRM_PATH = '#options > span > span > div > div > div > div > form > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-16px._maw-420px > button.is_Button'
const WAIT_NEXT_PAGE_PATH = '#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-40px > button'

async function clickElement(page, actions){
    for (const action of actions){
        await page.waitForSelector(action["jsPath"])
        await page.click(action["jsPath"])

        if (action["delay"] != null){
            await new Promise(resolve => setTimeout(resolve, action["delay"]));
        }
    }
}

async function writeElement(page, actions){
    for (const action of actions){
        await page.waitForSelector(action["jsPath"])
        await page.click(action["jsPath"])
        await page.keyboard.type(action['text'])

        if (action["delay"] != null){
            await new Promise(resolve => setTimeout(resolve, action["delay"]));
        }
    }
}

async function performOnboarding(page){
    await page.goto('chrome-extension://' + PLUGIN_ID + '/options.html?onboarding=true');
    page.waitForNavigation();
    await new Promise(resolve => setTimeout(resolve, 1000));

    await clickElement(page, [
        {'jsPath': CREATE_NEW_WALLET_PATH, 'delay': 100},
    ])
    await writeElement(page, [
        {'jsPath': ACCOUNT_NAME_PATH, 'delay': 100, 'text': 'axel'},
    ])
    await clickElement(page, [
        {'jsPath': ACCOUNT_NAME_CONFIRM_PATH, 'delay': 100},
        {'jsPath': UNDERSTAND_CHECKBOX_PATH, 'delay': 100},
        {'jsPath': UNDERSTAND_CONFIRM_PATH, 'delay': 100},
        {'jsPath': COPIED_CHECKBOX_PATH, 'delay': 100},
        {'jsPath': COPIED_CONFIRM_PATH, 'delay': 100},
        {'jsPath': SOLANA_PATH, 'delay': 100},
    ])

    await writeElement(page, [
        {'jsPath': PASS_PATH, 'delay': 100, 'text': '12345678'},
        {'jsPath': PASS2_PATH, 'delay': 100, 'text': '12345678'}
    ])

    await clickElement(page, [
        {'jsPath': AGREE_TERMS_PATH, 'delay': 100},
        {'jsPath': PASS_CONFIRM_PATH, 'delay': 100},
    ])
    await page.waitForSelector(WAIT_NEXT_PAGE_PATH)
}

async function confirmConnect(page){
    await page.bringToFront();

    await page.waitForSelector("body > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._mih-0px._miw-0px._fs-0._contain-strict._pos-fixed._t-0px._l-0px._r-0px._b-0px._maw-100vw._mah-100vh._pe-none._zi-101000 > span > span > div.css-175oi2r.r-105ug2t > div.is_Sheet._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._bg-1067792101._btlr-1881200812._btrr-1881200812._w-10037._mah-10037._ox-hidden._oy-hidden._h-570px > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._pt-1316330145 > div._ai-stretch._dsp-flex._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._pt-1481558214._pr-1316330145._pb-1316330145._pl-1316330145._fd-row > div:nth-child(3) > div")
    await new Promise(resolve => setTimeout(resolve, 500));
    await page.click("body > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._mih-0px._miw-0px._fs-0._contain-strict._pos-fixed._t-0px._l-0px._r-0px._b-0px._maw-100vw._mah-100vh._pe-none._zi-101000 > span > span > div.css-175oi2r.r-105ug2t > div.is_Sheet._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._bg-1067792101._btlr-1881200812._btrr-1881200812._w-10037._mah-10037._ox-hidden._oy-hidden._h-570px > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._pt-1316330145 > div._ai-stretch._dsp-flex._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._pt-1481558214._pr-1316330145._pb-1316330145._pl-1316330145._fd-row > div:nth-child(3) > div")
}

async function confirmSignTransaction(page){
    await page.bringToFront();
    
    await clickElement(page, [
        {'jsPath': 'body > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._mih-0px._miw-0px._fs-0._contain-strict._pos-fixed._t-0px._l-0px._r-0px._b-0px._maw-100vw._mah-100vh._pe-none._zi-101000 > span > span > div.css-175oi2r.r-105ug2t > div.is_Sheet._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-1._fg-1._bg-1067792101._btlr-1881200812._btrr-1881200812._w-10037._mah-10037._ox-hidden._oy-hidden._h-570px > div > div > div._ai-stretch._dsp-flex._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._pt-1481558214._pr-1316330145._pb-1316330145._pl-1316330145._fd-row > div:nth-child(3) > div', 'delay': 100},
    ])
}

(async () => {
    try{
        const browser = await puppeteer.launch({ headless: true,
            //executablePath: '/usr/bin/google-chrome',
            ignoreDefaultArgs: false,
            args: [
                '--disable-extensions-except=' + PLUGIN_PATH,
                '--load-extension=' + PLUGIN_PATH,
            ]
        });

        await new Promise(resolve => setTimeout(resolve, 6000));
        var abc = (await browser.pages())[1].url()
        var first_index = abc.indexOf("://")
        var second_index = abc.indexOf("/", first_index + 3)
        PLUGIN_ID = abc.substring(first_index + 3, second_index)
        console.log(PLUGIN_ID)
        console.log(abc)
        console.log("abc")
        var _ret = (await browser.pages())[1].close()
        

        const extensionPage = await browser.newPage();

        await performOnboarding(extensionPage);

        await extensionPage.waitForSelector("#options > span > span > div > div > div > div > div._ai-stretch._dsp-flex._fd-column._fb-auto._bxs-border-box._pos-relative._mih-0px._miw-0px._fs-0._gap-40px > button");

        // Place it in popup page to prevent a real popup.
        await extensionPage.goto('chrome-extension://' + PLUGIN_ID + '/popup.html');
        //await new Promise(resolve => setTimeout(resolve, 3000));

        const gamePage = await browser.newPage();

        // Listen for "console" event and print messages to the node console
        gamePage.on('console', msg => {
            for (let i = 0; i < msg.args().length; ++i) {
                console.log(`${i}: ${msg.args()[i]}`);
            }
        });

        // Navigate to a webpage
        await gamePage.goto('http://localhost:8060/tmp_js_export.html');
        // Allow connecting function to execute.
        await new Promise(resolve => setTimeout(resolve, 3000));

        // Confirm connect.
        await confirmConnect(extensionPage);
        await gamePage.bringToFront();

        // Allow signing function to execute.
        await new Promise(resolve => setTimeout(resolve, 3000));

        // Sign transaction.
        await confirmSignTransaction(extensionPage);
        await gamePage.bringToFront();

        // Timeout before closing the browser.
        await new Promise(resolve => setTimeout(resolve, 17000));
        await browser.close();
    }
    catch(e){
        console.log(e)
        console.log("FAIL")
    }
})();
