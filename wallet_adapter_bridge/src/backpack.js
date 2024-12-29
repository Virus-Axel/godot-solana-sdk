export default class Backpack {
    constructor() {
        const { backpack } = window;
        this.handler = backpack;
        if (backpack) {
            if (backpack.isBackpack) {
                this.readyState = "Installed";
                this._readyState = "Installed";
            }
        }
        else {
            this.readyState = "NotInstalled";
            this._readyState = "NotInstalled";
        }
    }

    on() {

    }

    async connect() {
        await this.handler.connect();
        this.publicKey = this.handler.publicKey;
    }
    async signTransaction(tx) {
        return await this.handler.signTransaction(tx);
    }
    async signMessage(buffer_message) {
        return await this.handler.signMessage(buffer_message);
    }
}
