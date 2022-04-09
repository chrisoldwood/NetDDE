const { NetDDEClient, Constants } = require('netdde');

let excel = new NetDDEClient('EXCEL', { host: 'localhost', port: 8888 });
excel.connect().then(async () => {
    let testValue = 'Hello, World!'

    await excel.poke('[Empty.xls]Sheet', 'R1C1', Constants.dataType.CF_TEXT, testValue);
    let value = await excel.request('[Empty.xls]Sheet', 'R1C1');
    if (value.trim() !== testValue) {
        console.log(`ERROR: Poke or Request failed: expected '${testValue}', received '${value}'`);
        process.exit(1);
    }

    await excel.disconnect();
});
