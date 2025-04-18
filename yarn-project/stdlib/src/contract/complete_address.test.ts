import { Fr, Point } from '@aztec/foundation/fields';

import { AztecAddress } from '../aztec-address/index.js';
import { PublicKeys } from '../keys/public_keys.js';
import { CompleteAddress } from './complete_address.js';

describe('CompleteAddress', () => {
  it('refuses to add an account with incorrect address for given partial address and pubkey', async () => {
    await expect(
      CompleteAddress.create(await AztecAddress.random(), await PublicKeys.random(), Fr.random()),
    ).rejects.toThrow(/cannot be derived/);
  });

  it('equals returns true when 2 instances are equal', async () => {
    const address1 = await CompleteAddress.random();
    const address2 = await CompleteAddress.create(address1.address, address1.publicKeys, address1.partialAddress);
    expect(address1.equals(address2)).toBe(true);
  });

  it('equals returns true when 2 instances are not equal', async () => {
    const address1 = await CompleteAddress.random();
    const address2 = await CompleteAddress.random();
    expect(address1.equals(address2)).toBe(false);
  });

  it('serializes / deserializes correctly', async () => {
    const expectedAddress = await CompleteAddress.random();
    const address = await CompleteAddress.fromBuffer(expectedAddress.toBuffer());
    expect(address.equals(expectedAddress)).toBe(true);
  });

  it('instantiates from string and individual components', async () => {
    const completeAddressFromString = await CompleteAddress.fromString(
      '0x24e4646f58b9fbe7d38e317db8d5636c423fbbdfbe119fc190fe9c64747e0c6222f7fcddfa3ce3e8f0cc8e82d7b94cdd740afa3e77f8e4a63ea78a239432dcab0471657de2b6216ade6c506d28fbc22ba8b8ed95c871ad9f3e3984e90d9723a7111223493147f6785514b1c195bb37a2589f22a6596d30bb2bb145fdc9ca8f1e273bbffd678edce8fe30e0deafc4f66d58357c06fd4a820285294b9746c3be9509115c96e962322ffed6522f57194627136b8d03ac7469109707f5e44190c4840c49773308a13d740a7f0d4f0e6163b02c5a408b6f965856b6a491002d073d5b00d3d81beb009873eb7116327cf47c612d5758ef083d4fda78e9b63980b2a7622f567d22d2b02fe1f4ad42db9d58a36afd1983e7e2909d1cab61cafedad6193a0a7c585381b10f4666044266a02405bf6e01fa564c8517d4ad5823493abd31de',
    );

    const address = new AztecAddress(
      Fr.fromHexString('0x24e4646f58b9fbe7d38e317db8d5636c423fbbdfbe119fc190fe9c64747e0c62'),
    );
    const npkM = Point.fromString(
      '0x22f7fcddfa3ce3e8f0cc8e82d7b94cdd740afa3e77f8e4a63ea78a239432dcab0471657de2b6216ade6c506d28fbc22ba8b8ed95c871ad9f3e3984e90d9723a7',
    );
    const ivpkM = Point.fromString(
      '0x111223493147f6785514b1c195bb37a2589f22a6596d30bb2bb145fdc9ca8f1e273bbffd678edce8fe30e0deafc4f66d58357c06fd4a820285294b9746c3be95',
    );
    const ovpkM = Point.fromString(
      '0x09115c96e962322ffed6522f57194627136b8d03ac7469109707f5e44190c4840c49773308a13d740a7f0d4f0e6163b02c5a408b6f965856b6a491002d073d5b',
    );
    const tpkM = Point.fromString(
      '0x00d3d81beb009873eb7116327cf47c612d5758ef083d4fda78e9b63980b2a7622f567d22d2b02fe1f4ad42db9d58a36afd1983e7e2909d1cab61cafedad6193a',
    );

    const partialAddress = Fr.fromHexString('0x0a7c585381b10f4666044266a02405bf6e01fa564c8517d4ad5823493abd31de');

    const completeAddressFromComponents = await CompleteAddress.create(
      address,
      new PublicKeys(npkM, ivpkM, ovpkM, tpkM),
      partialAddress,
    );

    expect(completeAddressFromComponents.equals(completeAddressFromString)).toBe(true);
  });

  it('has correct size in bytes', async () => {
    const completeAddress = await CompleteAddress.random();
    expect(completeAddress.toBuffer().length).toBe(CompleteAddress.SIZE_IN_BYTES);
  });
});
