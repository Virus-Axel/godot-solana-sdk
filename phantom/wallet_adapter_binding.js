async function checkIfWalletIsConnected() {
    try{
        const { solana } = window;
        if (solana) {
            if (solana.isPhantom) {
                console.log('Phantom wallet found!');
                const response = await solana.connect();
                console.log(
                'Connected with Public Key:',
                response.publicKey.toString());
                return 1;
            }
            } else {
                alert('Solana object not found! Get a Phantom Wallet 👻');
                return 3;
            }
        }
        catch (error){
        console.error(error);
        return 2
    }
}
checkIfWalletIsConnected();