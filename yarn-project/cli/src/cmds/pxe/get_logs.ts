import type { AztecAddress, LogFilter, LogId, TxHash } from '@aztec/aztec.js';
import { createCompatibleClient } from '@aztec/aztec.js';
import type { LogFn, Logger } from '@aztec/foundation/log';
import { sleep } from '@aztec/foundation/sleep';

export async function getLogs(
  txHash: TxHash,
  fromBlock: number,
  toBlock: number,
  afterLog: LogId,
  contractAddress: AztecAddress,
  rpcUrl: string,
  follow: boolean,
  debugLogger: Logger,
  log: LogFn,
) {
  const pxe = await createCompatibleClient(rpcUrl, debugLogger);

  if (follow) {
    if (txHash) {
      throw Error('Cannot use --follow with --tx-hash');
    }
    if (toBlock) {
      throw Error('Cannot use --follow with --to-block');
    }
  }

  const filter: LogFilter = { txHash, fromBlock, toBlock, afterLog, contractAddress };

  const fetchLogs = async () => {
    const response = await pxe.getPublicLogs(filter);
    const logs = response.logs;

    if (!logs.length) {
      const filterOptions = Object.entries(filter)
        .filter(([, value]) => value !== undefined)
        .map(([key, value]) => `${key}: ${value}`)
        .join(', ');
      if (!follow) {
        log(`No logs found for filter: {${filterOptions}}`);
      }
    } else {
      if (!follow && !filter.afterLog) {
        log('Logs found: \n');
      }
      logs.forEach(publicLog => log(publicLog.toHumanReadable()));
      // Set the continuation parameter for the following requests
      filter.afterLog = logs[logs.length - 1].id;
    }
    return response.maxLogsHit;
  };

  if (follow) {
    log('Fetching logs...');
    while (true) {
      const maxLogsHit = await fetchLogs();
      if (!maxLogsHit) {
        await sleep(1000);
      }
    }
  } else {
    while (await fetchLogs()) {
      // Keep fetching logs until we reach the end.
    }
  }
}
